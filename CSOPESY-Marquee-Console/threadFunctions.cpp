#include "gHeader.hpp"

void keyboardHandlerThreadFunction() {
	std::string commandLine = "";
	std::atomic<bool> enterKeyPressed = false;

	char key;

	std::unique_lock<std::mutex> keyboardDisplayLock(keyboardDisplayMutex, std::defer_lock);

	while (isRunning) {
		key = _getch();

		if (key == '\n' || key == '\r') {
			enterKeyPressed = true;
		}
		else if (key == 8) {
			if (!commandLine.empty())
				commandLine.pop_back();

			keyboardDisplayLock.lock();
			if (!displayCommand.empty()) {
				displayCommand.pop_back();
				backspacePressed = true;
			}
			keyboardDisplayLock.unlock();
		}
		else {
			commandLine += key;

			keyboardDisplayLock.lock();
			displayCommand += key;
			keyboardDisplayLock.unlock();
		}

		if (enterKeyPressed) {

			if (!commandLine.empty()) {
				std::unique_lock<std::mutex> lock(commandQueueMutex);
				commandQueue.push(commandLine);
				lock.unlock();
			}

			keyboardDisplayLock.lock();
			displayCommand = "";
			keyboardDisplayLock.unlock();
			enterKeyPressed = false;
			commandLine = "";
		}
	}
}

void marqueeLogicThreadFunction(int displayWidth) {

	int startingPosition = displayWidth;

	std::vector<std::string> marqueeSubString = { "", "", "" };
	std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex, std::defer_lock);

	while (isRunning) {
		std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
		size_t textLength = marqueeText.length();
		std::string marqueeToPrint = marqueeText;
		size_t speed = marqueeSpeed;
		mainMarqueeLock.unlock();

		if (!marqueeRunning) {
			marqueeDisplayLock.lock();
			//marqueeSubStrings.at(0) = std::string(displayWidth, ' ');
			displayMarquee = "";
			marqueeDisplayLock.unlock();
			startingPosition = displayWidth;
		}
		else {
			marqueeDisplayLock.lock();
			marqueeSubString.at(0) = std::string(startingPosition <= 0 ? 0 : startingPosition, ' ');

			if (startingPosition > 0)
			{
				int difference = displayWidth - (startingPosition < 0 ? 0 : startingPosition);
				if (difference >= textLength) {
					marqueeSubString.at(1) = marqueeToPrint;
					marqueeSubString.at(2) = std::string(displayWidth - (startingPosition + textLength), ' ');
					//marqueeSubString.at(2) = (difference == 0) ? "" : " ";
				}
				else {
					marqueeSubString.at(1) = marqueeToPrint.substr(0, difference);
					//marqueeSubString.at(2) = std::string(displayWidth - difference, ' ');
					marqueeSubString.at(2) = "";
				}
			}
			else {
				marqueeSubString.at(1) = marqueeToPrint.substr(abs(startingPosition), startingPosition + textLength);
			}
			displayMarquee = marqueeSubString.at(0) + marqueeSubString.at(1) + marqueeSubString.at(2);
			marqueeDisplayLock.unlock();
			startingPosition--;
		}

		// When text disappears
		if (startingPosition + textLength <= 0)
			startingPosition = displayWidth;

		// Sleep
		std::this_thread::sleep_for(std::chrono::milliseconds(speed));
	}
}

void displayThreadFunction() {
	const int refresh_rate_ms = 10;
	while (isRunning) {

		// Marquee
		if (marqueeRunning) {
			gotoxy(0, 3);
			std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
			//std::cout << marqueeSubStrings.at(0) << marqueeSubStrings.at(1) << marqueeSubStrings.at(2);
			std::cout << displayMarquee;
			marqueeDisplayLock.unlock();
		}

		// Basic Information of the Project
		gotoxy(0, 7);
		std::cout << "Developer: Ampatin, Ian Kenneth" << std::endl
			<< "Version Date: V225.919";

		// Print the contents for the help command
		if (printHelp) {
			gotoxy(0, 10);
			printHelpFunction();
			printHelp = false;
		}

		// One shot print blocks
		if (printPrompt) {
			gotoxy(65, 8);
			std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
			std::cout << systemPromptText;
			mainMarqueeLock.unlock();
			printPrompt = false;
		}

		// Display what the user is typing
		std::unique_lock<std::mutex> keyboardDisplayLock(keyboardDisplayMutex);
		if (backspacePressed) {
			gotoxy(xCoordinateCommand, yCoordinateCommand);
			std::cout << "Command > " << std::string(displayCommand.length() + 1, ' ');
			backspacePressed = false;
		}
		gotoxy(xCoordinateCommand, yCoordinateCommand);
		std::cout << "Command > " << displayCommand;
		keyboardDisplayLock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
	}
}