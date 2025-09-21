#include "gHeader.hpp"

// Clears the screen
void clearScreen() {
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");	
	#endif
}

// Moves the cursor to a specific (x, y) coordinate on the console.
void gotoxy(int x, int y) {
	std::cout << "\033[" << y << ";" << x << "H";
	std::cout.flush();
}

// Tokenize a string
std::vector<std::string> getToken(std::string line) {
	std::vector<std::string> tokens;
	std::stringstream SS(line);
	std::string collector;

	while (getline(SS, collector, ' '))
		tokens.push_back(collector);

	return tokens;
}

// Print the commands that are recognized by the program
void printHelpFunction() {
	size_t padding = 20;

	std::unique_lock<std::mutex> mainDisplayLock(mainDisplayMutex);
	std::vector<std::vector<std::string>> commandList = registeredCommands;
	mainDisplayLock.unlock();

	commandList.push_back({"help", "- display the commands and their decription"});
	commandList.push_back({"start_marquee", "- starts the marquee \"animation\""});
	commandList.push_back({"stop_marquee", "- stops the marquee \"animations\""});
	commandList.push_back({"set_text", "- accepts a text input and displays it as a marquee"});
	commandList.push_back({"set_speed", "- set the marquee animation refresh in milliseconds"});
	commandList.push_back({"exit", "- terminate the console"});

	std::cout << std::left << std::setw(padding) << "COMMANDS" << "DESCRIPTION" << std::endl;
	for (std::vector<std::string> command : commandList) {
		std::cout << std::left << std::setw(padding) << command.at(0) << command.at(1) << std::endl;
	}
}

// Reset the variables to clean up the display
void restartDisplay() {
	clearScreen();
	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	mainMarqueeLock.unlock();
	systemPrompt("");
}

void disableEcho() {
	DWORD mode;
	HANDLE handler = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(handler, &mode);
	SetConsoleMode(handler, mode & (~ENABLE_ECHO_INPUT));
}

void enableEcho() {
	DWORD mode;
	HANDLE handler = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(handler, &mode);
	SetConsoleMode(handler, mode | ENABLE_ECHO_INPUT);
}