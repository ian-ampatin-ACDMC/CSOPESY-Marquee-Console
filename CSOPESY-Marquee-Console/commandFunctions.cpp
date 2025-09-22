#include "gHeader.hpp"

void setMarqueeText(std::vector<std::string> tokens) {

	if (tokens.empty()) {
		systemPrompt("ERROR: set_text command is missing parameters.");
	}
	else {
		std::string newMarquee = tokens.at(0);
		tokens.erase(tokens.begin());

		// Concatenate Tokens First
		for (std::string subString : tokens) {
			newMarquee.append(" ");
			newMarquee.append(subString);
		}

		std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
		marqueeText = newMarquee;
		mainMarqueeLock.unlock();
		systemPrompt("NOTICE: Marquee text has been edited.");
	}
}

void setMarqueeSpeed(std::vector<std::string> tokens) {

	try {
		if (tokens.empty()) {
			systemPrompt("ERROR: set_speed command is missing parameters.");
		}
		else {
			size_t check;
			int speed = std::stoi(tokens.at(0), &check);

			if (speed < 0)
				systemPrompt("ERROR: set_speed cannot take a negative paramter.");
			else if (check == tokens.at(0).length()) {
				std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
				marqueeSpeed = speed;
				mainMarqueeLock.unlock();
				systemPrompt("NOTICE: Marquee animation speed has been edited.");
			}
			else {
				systemPrompt("ERROR: Invalid Argument.");
			}
		}
	}
	catch (const std::invalid_argument& e) {
		systemPrompt("ERROR: Invalid Argument.");
	}
	catch (const std::out_of_range& e) {
		systemPrompt("ERROR: Number is out of range.");
	}
}

void systemPrompt(std::string prompt) {
	std::unique_lock<std::mutex> promptLock(promptMutex);
	systemPromptText = prompt;
	printPrompt = true;
	promptLock.unlock();
}