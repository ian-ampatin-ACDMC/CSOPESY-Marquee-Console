#include "gHeader.hpp"

// --- Shared State and Thread Control ---
// Global flat to signal all threads to exit
std::string marqueeText = "[CSOPESY] Marquee Console";
std::atomic<bool> isRunning{ true };
std::atomic<bool> marqueeRunning{ true };

// The command interpreter and display thread share this variable
std::string promptDisplayBuffer = "";
std::mutex promptMutex;

// Shared state for the keyboard handler and command interpreter
std::queue<std::string> commandQueue;
std::mutex commandQueueMutex;

// The marquee logic thread and display thread share this variable
std::vector<std::string> marqueeSubStrings = { "", "", "", "" };
std::string marqueeDisplayBuffer = "";
std::mutex marqueeDisplayMutex;

// The main thread and marquee logic thread share this variable
std::string systemPromptText = "";
std::mutex mainMarqueeMutex;
std::atomic<bool> printHelp{ false };
std::atomic<bool> printText{ true };
std::atomic<bool> printPrompt{ false };
size_t marqueeSpeed = 200;

// The main thread and display logic thread share this variable
std::mutex mainDisplayMutex;

// --- Thread Functions ---
void displayCommandLogicFunction() {

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