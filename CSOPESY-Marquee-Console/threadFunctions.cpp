#include "gHeader.hpp"

void keyboardHandlerThreadFunction() {
	std::string commandLine;
	while (isRunning) {
		std::getline(std::cin, commandLine);

		if (!commandLine.empty()) {
			std::unique_lock<std::mutex> lock(commandQueueMutex);
			commandQueue.push(commandLine);
			lock.unlock();
		}
	}
}

void marqueeLogicThreadFunction(int displayWidth) {

	int startingPosition = displayWidth;

	std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
	marqueeSubStrings.at(3) = std::string(displayWidth, '-');
	marqueeDisplayLock.unlock();

	while (isRunning) {
		std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
		size_t textLength = marqueeText.length();
		std::string marqueeToPrint = marqueeText;
		mainMarqueeLock.unlock();

		marqueeDisplayLock.lock();
		if (!marqueeRunning) {
			//marqueeSubStrings.at(0) = std::string(displayWidth, ' ');
			marqueeSubStrings.at(0) = "";
			marqueeSubStrings.at(1) = "";
			marqueeSubStrings.at(2) = "";
			startingPosition = displayWidth;
		}
		else {
			marqueeSubStrings.at(0) = std::string(startingPosition <= 0 ? 0 : startingPosition, ' ');

			if (startingPosition > 0)
			{
				int difference = displayWidth - startingPosition;
				if (difference > textLength) {
					marqueeSubStrings.at(1) = marqueeToPrint;
					marqueeSubStrings.at(2) = std::string(displayWidth - (startingPosition + textLength), ' ');
				}
				else {
					marqueeSubStrings.at(1) = marqueeToPrint.substr(0, difference);
					marqueeSubStrings.at(2) = std::string(displayWidth - difference, ' ');
				}
			}
			else {
				marqueeSubStrings.at(1) = marqueeToPrint.substr(abs(startingPosition), startingPosition + textLength);
			}

			startingPosition--;
		}
		marqueeDisplayLock.unlock();

		// When text disappears
		if (startingPosition + textLength <= 0)
			startingPosition = displayWidth;

		// Sleep
		mainMarqueeLock.lock();
		std::this_thread::sleep_for(std::chrono::milliseconds(marqueeSpeed));
		mainMarqueeLock.unlock();
	}
}

void displayThreadFunction() {
	const int refresh_rate_ms = 50;
	const int padding = 15;
	while (isRunning) {

		// Marquee
		if (marqueeRunning) {
			gotoxy(0, 3);
			std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex);
			std::cout << marqueeSubStrings.at(0) << marqueeSubStrings.at(1) << marqueeSubStrings.at(2);
			marqueeDisplayLock.unlock();
		}

		// Command
		gotoxy(65, 7);

		// One Shot display blocks
		if (printText) {
			gotoxy(0, 7);
			std::cout << "Developer: Ampatin, Ian Kenneth" << std::endl
				<< "Version Date: V225.919";

			printText = false;
		}

		if (printHelp) {
			gotoxy(0, 10);
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

			printHelp = false;
		}

		if (printPrompt) {
			gotoxy(65, 8);
			std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
			std::cout << systemPromptText;
			mainMarqueeLock.unlock();
			printPrompt = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(refresh_rate_ms));
	}
}