#include "gHeader.hpp"

// --- Main Function (Command Interpreter Thread) ---
int main() {
	// Get the default values 
	std::fstream file("config.txt");

	if (!file.is_open()) 
		return 1;

	std::vector<std::string> fileTokens;
	std::string line;
	std::string fileFirstToken;
	while (std::getline(file, line)) {
		fileTokens = getToken(line);
		fileFirstToken = fileTokens.at(0);

		if (fileFirstToken == "generalDisplayWidth")
			lengthOfDisplay = std::stoi(fileTokens.at(1));
		else if (fileFirstToken == "commandDisplayWidth")
			commandDisplayLength = std::stoi(fileTokens.at(1));
		else if (fileFirstToken == "defaultMarqueeSpeed")
			marqueeSpeed = std::stoi(fileTokens.at(1));
	}

	file.close();


	// Locks
	std::unique_lock<std::mutex> commandMarqueeLock(commandMarqueeMutex, std::defer_lock);

	// Variables
	std::string commandLine;

	// Disable echo
	disableEcho();

	// Hide the cursor
	std::cout << "\x1b[?25l"; 
	std::cout.flush();

	// Start the three worker threads.
	std::thread marqueeLogicThread(marqueeLogicThreadFunction, lengthOfDisplay);
	std::thread displayThread(displayThreadFunction);
	std::thread keyboardHandlerThread(keyboardHandlerThreadFunction);

	// Main loop that processes commands from the queue
	while (isRunning) {
		{
			std::unique_lock<std::mutex> lock(commandQueueMutex);
			
			if (!commandQueue.empty()) {
				commandLine = commandQueue.front();
				commandQueue.pop();
			}

			lock.unlock();
		}

		if (!commandLine.empty()) {
			// Variables
			std::vector<std::string> commandTokens = getToken(commandLine);
			std::string fToken = commandTokens.at(0);

			// Remove first token from the vector
			commandTokens.erase(commandTokens.begin());

			// Restart the display screen
			clearScreen();

			// Command Logic
			if (fToken == "help") {
				commandMarqueeLock.lock();
				printHelp = true;
				commandMarqueeLock.unlock();
			}
			else if (fToken == "start_marquee") {
				commandMarqueeLock.lock();
				marqueeRunning = true;
				commandMarqueeLock.unlock();
				systemPrompt("NOTICE: Marquee animation has started.");
			}
			else if (fToken == "stop_marquee") {
				commandMarqueeLock.lock();
				marqueeRunning = false;
				commandMarqueeLock.unlock();
				systemPrompt("NOTICE: Marquee animation has stopped.");
			}
			else if (fToken == "set_text") {
				setMarqueeText(commandTokens);
			}
			else if (fToken == "set_speed") {
				setMarqueeSpeed(commandTokens);
			}
			else if (fToken == "exit") {
				isRunning = false;
			}
			else {
				systemPrompt("ERROR: Command is not recognized.");
			}

			commandLine = "";
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Exit message
	clearScreen();
	std::cout << "Program has been terminated." << std::endl;

	// Re-enable echo
	enableEcho();

	// Join threads to ensure the finish cleanly.
	if (marqueeLogicThread.joinable())
		marqueeLogicThread.join();

	if (displayThread.joinable())
		displayThread.join();

	if (keyboardHandlerThread.joinable())
		keyboardHandlerThread.join();

	return 0;
}