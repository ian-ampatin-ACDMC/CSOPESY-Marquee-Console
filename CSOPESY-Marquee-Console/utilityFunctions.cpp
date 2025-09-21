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
	int padding = 20;
	int descriptionSpace = lengthOfDisplay - padding;
	size_t consumedSpace;
	std::string token;

	std::atomic<bool> firstElement;
	std::vector<std::string> stringTokens;
	std::unique_lock<std::mutex> mainDisplayLock(mainDisplayMutex);
	std::vector<std::vector<std::string>> commandList = registeredCommands;
	mainDisplayLock.unlock();

	commandList.push_back({"help", "- display the commands and their decription"});
	commandList.push_back({"start_marquee", "- starts the marquee \"animation\""});
	commandList.push_back({"stop_marquee", "- stops the marquee \"animations\""});
	commandList.push_back({"set_text", "- accepts a text input and displays it as a marquee"});
	commandList.push_back({"set_speed", "- set the marquee animation refresh in milliseconds"});
	commandList.push_back({"exit", "- terminate the console"});

	std::cout << std::string(lengthOfDisplay, '-') << std::endl;
	std::cout << std::left << std::setw(padding) << "COMMANDS" << "DESCRIPTION" << std::endl;
	std::cout << std::string(lengthOfDisplay, '-') << std::endl;

	for (std::vector<std::string> command : commandList) {
		stringTokens = getToken(command.at(1));
		std::cout << std::left << std::setw(padding) << command.at(0); // << command.at(1) << std::endl;
		
		// Print Description
		firstElement = true;
		consumedSpace = 0;
		while (!stringTokens.empty()) {
			token = stringTokens.at(0);
			consumedSpace += token.length() + 1;

			if (consumedSpace <= descriptionSpace) {
				
				if (firstElement) {
					std::cout << token;
					firstElement = false;
				}
				else
					std::cout << " " << token;
			}
			else {
				std::cout << std::endl << std::left << std::setw(padding) << "" << "  " << token;
				consumedSpace = 0;
			}

			stringTokens.erase(stringTokens.begin());
		}
		
		std::cout << std::endl;
	}
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