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

	/*COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);*/
}

std::vector<std::string> getToken(std::string line) {
	std::vector<std::string> tokens;
	std::stringstream SS(line);
	std::string collector;

	while (getline(SS, collector, ' '))
		tokens.push_back(collector);

	return tokens;
}