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
}

// Reset the variables to clean up the display
void restartDisplay() {
	clearScreen();
	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	printText = true;
	mainMarqueeLock.unlock();
	systemPrompt("");
}