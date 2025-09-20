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
	std::cout.flush();

	/*COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);*/
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
	
		marqueeDisplayLock.lock();
		if (!marqueeRunning) {
			//marqueeSubStrings.at(0) = std::string(displayWidth, ' ');
			marqueeSubStrings.at(0) = "";
			marqueeSubStrings.at(1) = "";
			marqueeSubStrings.at(2) = "";
			startingPosition = displayWidth;
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
		}
		marqueeDisplayLock.unlock();

		// When text disappears
		if (startingPosition + textLength <= 0)
			startingPosition = displayWidth;

		// Sleep
		mainMarqueeLock.lock();
		std::this_thread::sleep_for(std::chrono::milliseconds(marqueeSpeed));
		mainMarqueeLock.unlock();
	}
}

void displayThreadFunction() {
	const int refresh_rate_ms = 50;
	const int padding = 15;
	while (isRunning) {

		// Marquee
		if (marqueeRunning) {
			gotoxy(0, 3);
			std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
			std::cout << marqueeSubStrings.at(0) << marqueeSubStrings.at(1) << marqueeSubStrings.at(2);
			marqueeDisplayLock.unlock();
		}

		// Command
		gotoxy(65, 7);

		// One Shot display blocks
		if (printText) {
			gotoxy(0, 7);
			std::cout << "Developer: Ampatin, Ian Kenneth" << std::endl
				<< "Version Date: V225.919";

			printText = false;
		}

		if (printHelp) {
			gotoxy(0, 10);
			std::cout << std::left << std::setw(padding) << "COMMANDS"
				<< "DESCRIPTION" << std::endl;
			std::cout << std::left << std::setw(padding) << "help"
				<< "- displays the commands and its description" << std::endl;
			std::cout << std::left << std::setw(padding) << "start_marquee"
				<< "- starts the marquee \"animation\"" << std::endl;
			std::cout << std::left << std::setw(padding) << "stop_marquee"
				<< "- stops the marquee \"animations\"" << std::endl;
			std::cout << std::left << std::setw(padding) << "set_text"
				<< "- accepts a text input and displays it as a marquee" << std::endl;
			std::cout << std::left << std::setw(padding) << "set_speed"
				<< "- set the marquee animation refresh in milliseconds" << std::endl;
			std::cout << std::left << std::setw(padding) << "exit"
				<< "- terminate the console";
			
			printHelp = false;
		}

		if (printPrompt) {
			gotoxy(65, 8);
			std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
			std::cout << systemPromptText;
			mainMarqueeLock.unlock();
			printPrompt = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
	}
}

// --- Command Functions
void setMarqueeText(std::vector<std::string> tokens) {

	std::string newMarquee = tokens.at(0);
	tokens.erase(tokens.begin());

	// Concatenate Tokens First
	for (std::string subString : tokens) {
		newMarquee.append(" ");
		newMarquee.append(subString);
	}

	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	marqueeText = newMarquee;
	mainMarqueeLock.unlock();
}

void setMarqueeSpeed(std::vector<std::string> tokens) {

	size_t speed = std::stoi(tokens.at(0));

	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	marqueeSpeed = speed;
	mainMarqueeLock.unlock();
}

void systemPrompt(std::string prompt) {
	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	systemPromptText = prompt;
	printPrompt = true;
	mainMarqueeLock.unlock();
}

// --- Helper Functions ---
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

			// Variables
			std::vector<std::string> commandTokens = getToken(commandLine);
			std::string fToken = commandTokens.at(0);

			// Remove first token from the vector
			commandTokens.erase(commandTokens.begin());

			if (fToken == "help") {
				mainMarqueeLock.lock();
				printHelp = true;
				mainMarqueeLock.unlock();
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

				if (marqueeRunning)
					systemPrompt("ERROR: Marquee is still running.");
				else
					setMarqueeText(commandTokens);
			}
			else if (fToken == "set_speed") {
				setMarqueeSpeed(commandTokens);
			}
			else if (fToken == "exit") {
				isRunning = false;
				clearScreen();
			}
			else {
				clearScreen();
				mainMarqueeLock.lock();
				printText = true;
				mainMarqueeLock.unlock();
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