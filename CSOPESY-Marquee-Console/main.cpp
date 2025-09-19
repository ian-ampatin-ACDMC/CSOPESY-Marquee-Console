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
	std::string command_line;
	while (isRunning) {
		std::getline(std::cin, command_line);

		if (!command_line.empty()) {
			std::unique_lock<std::mutex> lock(commandQueueMutex);
			commandQueue.push(command_line);
			lock.unlock();
		}
	}
}

void marqueeLogicThreadFunction(int displayWidth) {

	std::vector<int> marqueeSpecs;

	std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
	marqueeSpecs.push_back(0);
	marqueeSpecs.push_back(0);
	marqueeDisplayLock.unlock();

	while (isRunning) {
		
		std::unique_lock<std::mutex> commandQueueLock(commandQueueMutex);
		size_t textLength = marqueeText.length();
		commandQueueLock.unlock();

		size_t startPos = displayWidth;

		while (isRunning) { 
			
			// CASE when marquee is not running
			// set both specs to 0 and 0
			if (!marqueeRunning) {
				marqueeDisplayLock.lock();
				marqueeSpecs.at(0) = 0;
				marqueeSpecs.at(1) = 0;
				marqueeDisplayLock.unlock();
			}


			// Sleep for 200 milliseconds
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}

void displayThread_func() {
	const int refresh_rate_ms = 50;
	while (isRunning) {
		// Display logic goes here...
		std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
	}
}

// --- Main Function (Command Interpreter Thread) ---
int main() {
	// Start the three worker threads.
	std::thread marqueeLogicThread(marqueeLogicThreadFunction, 80);
	std::thread displayThread(displayThread_func);
	std::thread keyboardHandlerThread(keyboardHandlerThreadFunction);

	// Main loop that processes commands from the queue
	while (isRunning) {
		std::string command_line;
		{
			std::unique_lock<std::mutex> lock(commandQueueMutex);
			
			if (!commandQueue.empty()) {
				command_line = commandQueue.front();
				commandQueue.pop();
			}
		}

		if (!command_line.empty()) {
			// Commad processing logic goes here...
			if (command_line == "exit") {
				clearScreen();
				isRunning = false;
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