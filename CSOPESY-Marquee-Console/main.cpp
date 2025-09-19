#include "gHeader.hpp"

// --- Utility Function ---

// Clears the screen
static void clearScreen() {
	// Simple clear screen cross-platform
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}

// Moves the cursor to a specific (x, y) coordinate on the console.
static void gotoxy(int x, int y) {
	std::cout << "\033[" << y << ";" << x << "H";
}

// --- Thread Functions ---
void keyboardHandlerThreadFunction() {
	std::string commandLine;
	while (isRunning) {
		std::getline(std::cin, commandLine);

		if (!commandLine.empty()) {
			std::unique_lock<std::mutex> lock(commandQueueMutex);
			commandQueue.push(commandLine);
			lock.unlock();
		}
	}
}

void marqueeLogicThreadFunction(int displayWidth) {

	int startingPosition = displayWidth;

	std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
	marqueeSubStrings.at(3) = std::string(displayWidth, '-');
	marqueeDisplayLock.unlock();

	while (isRunning) {
		
		std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
		size_t textLength = marqueeText.length();
		std::string marqueeToPrint = marqueeText;
		mainMarqueeLock.unlock();
		
		while (isRunning) { 

			
			marqueeDisplayLock.lock();
			if (!marqueeRunning) {
				marqueeSubStrings.at(0) = std::string(displayWidth, ' ');
				marqueeSubStrings.at(1) = "";
				marqueeSubStrings.at(2) = "";
			}
			else {
				marqueeSubStrings.at(0) = std::string(startingPosition <= 0 ? 0 : startingPosition, ' ');
				
				if (startingPosition > 0)
				{
					int difference = displayWidth - startingPosition;
					if (difference > textLength) {
						marqueeSubStrings.at(1) = marqueeToPrint;
						marqueeSubStrings.at(2) = std::string(displayWidth - (startingPosition + textLength), ' ');
					}
					else {
						marqueeSubStrings.at(1) = marqueeToPrint.substr(0, difference);
						marqueeSubStrings.at(2) = std::string(displayWidth - difference, ' ');
					}
				}
				else {
					marqueeSubStrings.at(1) = marqueeToPrint.substr(abs(startingPosition), startingPosition + textLength);
				}

				startingPosition--;

				// When text disappears
				if (startingPosition + textLength == 0)
					startingPosition = displayWidth;
			}
			marqueeDisplayLock.unlock();

			// Sleep
			std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
			std::this_thread::sleep_for(std::chrono::milliseconds(marqueeSpeed));
			mainMarqueeLock.unlock();
		}
	}
}

void displayThreadFunction() {
	const int refresh_rate_ms = 50;
	while (isRunning) {

		// Marquee
		gotoxy(0, 3);
		std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
		//std::cout << marqueeSubStrings.at(3) << std::endl;
		std::cout << marqueeSubStrings.at(0) << marqueeSubStrings.at(1) << marqueeSubStrings.at(2) << std::endl;
		//std::cout << marqueeSubStrings.at(3);
		marqueeDisplayLock.unlock();

		// Prompt
		gotoxy(0, 7);
		std::unique_lock<std::mutex> mainDisplayLock(mainDisplayMutex);
		std::cout << "Developer: Ampatin, Ian Kenneth" << std::endl
			      << "Version Date: V225.919";
		mainDisplayLock.unlock();

		gotoxy(0, 10);

		std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
	}
}

// --- Main Thread Functions ---
std::vector<std::string> getToken(std::string line) {
	std::vector<std::string> tokens;
	std::stringstream SS(line);
	std::string collector;

	while (getline(SS, collector, ' '))
		tokens.push_back(collector);

	return tokens;
}

// --- Main Function (Command Interpreter Thread) ---
int main() {
	// Start the three worker threads.
	std::thread marqueeLogicThread(marqueeLogicThreadFunction, 60);
	std::thread displayThread(displayThreadFunction);
	std::thread keyboardHandlerThread(keyboardHandlerThreadFunction);

	// Main loop that processes commands from the queue
	while (isRunning) {
		std::string commandLine;
		{
			std::unique_lock<std::mutex> lock(commandQueueMutex);
			
			if (!commandQueue.empty()) {
				commandLine = commandQueue.front();
				commandQueue.pop();
			}
		}

		if (!commandLine.empty()) {
			// Locks
			std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
			mainMarqueeLock.unlock();
			std::vector<std::string> commandTokens = getToken(commandLine);
			std::string fToken = commandTokens.at(0);
			commandTokens.erase(commandTokens.begin());

			if (fToken == "help") {

			}
			else if (fToken == "start_marquee") {
				mainMarqueeLock.lock();
				marqueeRunning = true;
				mainMarqueeLock.unlock();
			}
			else if (fToken == "stop_marquee") {
				mainMarqueeLock.lock();
				marqueeRunning = false;
				mainMarqueeLock.unlock();
			}
			else if (fToken == "set_text") {
				
			}
			else if (fToken == "set_speed") {

			}
			else if (fToken == "exit") {
				clearScreen();
				isRunning = false;
			}
			else {
				// Call prompt here
				clearScreen();
			}
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Join threads to ensure the finish cleanly.
	if (marqueeLogicThread.joinable())
		marqueeLogicThread.join();

	if (displayThread.joinable())
		displayThread.join();

	if (keyboardHandlerThread.joinable())
		keyboardHandlerThread.join();

	clearScreen();
	std::cout << "THANK YOU FOR USING THE MARQUEE" << std::endl;

	return 0;
}