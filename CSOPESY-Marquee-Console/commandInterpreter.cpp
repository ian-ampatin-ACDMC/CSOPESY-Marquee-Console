#include "gHeader.hpp"

// --- Main Function (Command Interpreter Thread) ---
int main() {
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

			// Restart the display screen
			clearScreen();

			// Command Logic
			if (fToken == "help") {
				mainMarqueeLock.lock();
				printHelp = true;
				mainMarqueeLock.unlock();
			}
			else if (fToken == "start_marquee") {
				mainMarqueeLock.lock();
				marqueeRunning = true;
				mainMarqueeLock.unlock();
				systemPrompt("NOTICE: Marquee animation has started.");
			}
			else if (fToken == "stop_marquee") {
				mainMarqueeLock.lock();
				marqueeRunning = false;
				mainMarqueeLock.unlock();
				clearScreen();
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