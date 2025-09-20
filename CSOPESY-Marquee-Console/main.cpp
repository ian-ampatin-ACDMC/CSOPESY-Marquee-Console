#include "gHeader.hpp"

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
				restartDisplay();
				systemPrompt("NOTICE: Marquee animation has started.");
			}
			else if (fToken == "stop_marquee") {
				mainMarqueeLock.lock();
				marqueeRunning = false;
				mainMarqueeLock.unlock();
				restartDisplay();
				systemPrompt("NOTICE: Marquee animation has stopped.");
			}
			else if (fToken == "set_text") {
				if (marqueeRunning)
					systemPrompt("ERROR: Marquee is still running.");
				else {
					setMarqueeText(commandTokens);
					restartDisplay();
					systemPrompt("NOTICE: Marquee text has been edited.");
				}
			}
			else if (fToken == "set_speed") {
				setMarqueeSpeed(commandTokens);
				restartDisplay();
				systemPrompt("NOTICE: Marquee speed has been edited.");
			}
			else if (fToken == "exit") {
				isRunning = false;
				clearScreen();
			}
			else {
				restartDisplay();
				systemPrompt("ERROR: Command is not recognized.");
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