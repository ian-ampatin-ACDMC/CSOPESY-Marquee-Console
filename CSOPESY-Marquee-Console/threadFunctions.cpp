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

			commandLine = "";
			keyboardDisplayLock.lock();
			displayCommand = "";
			keyboardDisplayLock.unlock();
			enterKeyPressed = false;
		}

		//std::this_thread::sleep_for(std::chrono::microseconds(refreshRate));
	}
}

void marqueeLogicThreadFunction(int displayWidth) {

	int startingPosition = displayWidth;

	std::vector<std::string> marqueeSubString = { "", "", "" };
	std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex, std::defer_lock);
	std::unique_lock<std::mutex> commandMarqueeLock(commandMarqueeMutex, std::defer_lock);

	std::string marqueeToPrint;
	size_t textLength;
	size_t speed;

	while (isRunning) {
		commandMarqueeLock.lock();
		marqueeToPrint = marqueeText;
		textLength = marqueeText.length();
		speed = marqueeSpeed;
		commandMarqueeLock.unlock();

		if (!marqueeRunning) {
			marqueeDisplayLock.lock();
			//marqueeSubStrings.at(0) = std::string(displayWidth, ' ');
			displayMarquee = "";
			marqueeDisplayLock.unlock();
			startingPosition = displayWidth;
		}
		else {
			marqueeSubString.at(0) = std::string(startingPosition <= 0 ? 0 : startingPosition, ' ');

			if (startingPosition >= 0)
			{
				int difference = displayWidth - (startingPosition < 0 ? 0 : startingPosition);
				if (difference > textLength) {
					marqueeSubString.at(1) = marqueeToPrint;
					marqueeSubString.at(2) = std::string(displayWidth - (startingPosition + textLength), ' ');
				}
				else {
					marqueeSubString.at(1) = marqueeToPrint.substr(0, difference);
					//marqueeSubString.at(2) = std::string(displayWidth - difference, ' ');
					marqueeSubString.at(2) = "";
				}
			}
			else {
				if (abs(startingPosition) >= textLength)
					startingPosition = displayWidth;
				else
					marqueeSubString.at(1) = marqueeToPrint.substr(abs(startingPosition), startingPosition + textLength);

				//marqueeSubString.at(1) = "Starting Position: " + std::to_string(startingPosition) + " | Condition : " + std::to_string(startingPosition + textLength);
			}

			marqueeDisplayLock.lock();
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
	std::unique_lock<std::mutex> marqueeDisplayLock(marqueeDisplayMutex, std::defer_lock);
	std::unique_lock<std::mutex> promptLock(promptMutex, std::defer_lock);
	std::unique_lock<std::mutex> keyboardDisplayLock(keyboardDisplayMutex, std::defer_lock);
	while (isRunning) {

		// Marquee
		if (marqueeRunning) {
			marqueeDisplayLock.lock();
			gotoxy(0, 3);
			std::cout << displayMarquee;
			marqueeDisplayLock.unlock();
		}

		// Basic Information of the Project
		promptLock.lock();
		gotoxy(0, 5);
		formattedPrint(introductionText);
		promptLock.unlock();

		// Print the contents for the help command
		if (printHelp) {
			promptLock.lock();
			gotoxy(0, 7);
			printHelpFunction();
			printHelp = false;
			promptLock.unlock();
		}

		// Display what the user is typing
		keyboardDisplayLock.lock();
		if (backspacePressed) {
			gotoxy(lengthOfDisplay + 5, 5);
			std::cout << "Command > " << std::string(displayCommand.length() + 1, ' ');
			backspacePressed = false;
		}
		gotoxy(lengthOfDisplay + 5, 5);
		std::cout << "Command > " << displayCommand;
		keyboardDisplayLock.unlock();

		// Command prompts
		if (printPrompt) {
			promptLock.lock();
			gotoxy(65, 6);
			std::cout << systemPromptText;
			printPrompt = false;
			promptLock.unlock();
		}

		std::this_thread::sleep_for(std::chrono::microseconds(refreshRate));
	}
}