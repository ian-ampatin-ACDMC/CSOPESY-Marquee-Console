#include "gHeader.hpp"

void setMarqueeText(std::vector<std::string> tokens) {

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
}

void setMarqueeSpeed(std::vector<std::string> tokens) {

	size_t speed = std::stoi(tokens.at(0));

	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	marqueeSpeed = speed;
	mainMarqueeLock.unlock();
}

void systemPrompt(std::string prompt) {
	std::unique_lock<std::mutex> mainMarqueeLock(mainMarqueeMutex);
	systemPromptText = prompt;
	printPrompt = true;
	mainMarqueeLock.unlock();
}