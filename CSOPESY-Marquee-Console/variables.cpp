#include "gHeader.hpp"

// --- Shared State and Thread Control ---
// Global flag to signal all threads to exit
std::atomic<bool> isRunning{ true };

// Multiple Thread Usage
std::atomic<bool> marqueeRunning{ false };
std::vector<std::string> introductionText = { "Developer: Ampatin, Ian Kenneth J.", "Version Date: 2025 / 10 / 01" };
std::string marqueeText = "[CSOPESY] Marquee Console";
size_t refreshRate = 10;
size_t lengthOfDisplay;
size_t commandDisplayLength;

// Command Interpreter & Display Thread
std::atomic<bool> printHelp{ false };
std::atomic<bool> printPrompt{ false };
std::mutex promptMutex;
std::mutex commandDisplayMutex;
std::string systemPromptText = "";

// Command Interpreter & Marquee Logic Thread
std::mutex commandMarqueeMutex;
size_t marqueeSpeed;

// Command Interpreter & Keyboard Handler Thread
std::mutex commandQueueMutex;
std::queue<std::string> commandQueue;

// Display Thread & Marquee Logic Thread
std::mutex marqueeDisplayMutex;
std::string displayMarquee = "";

// Display Thread & Keyboard Handler Thread
std::atomic<bool> newCommandStarted{ false };
std::atomic<bool> newCommandAccept { true };
std::mutex keyboardDisplayMutex;
std::string displayCommand = "";

// Display Thread Exclusive
std::vector<std::vector<std::string>> registeredCommands;
size_t startingPrintRow;