#include "gHeader.hpp"

// --- Shared State and Thread Control ---
// Global flag to signal all threads to exit
std::atomic<bool> isRunning{ true };

// Multiple Thread Usage
std::atomic<bool> marqueeRunning{ true };
std::vector<std::string> introductionText = { "CSOPESY MCO2 - Marquee Console Program", "Version Date: 2025 / 09 / 25" };
std::string marqueeText = "[CSOPESY] Marquee Console";
size_t refreshRate = 50;
size_t lengthOfDisplay = 60;

// Command Interpreter & Display Thread
std::atomic<bool> printHelp{ false };
std::atomic<bool> printPrompt{ false };
std::mutex promptMutex;
std::mutex commandDisplayMutex;
std::string systemPromptText = "";

// Command Interpreter & Marquee Logic Thread
std::mutex commandMarqueeMutex;
size_t marqueeSpeed = 100;

// Command Interpreter & Keyboard Handler Thread
std::mutex commandQueueMutex;
std::queue<std::string> commandQueue;

// Display Thread & Marquee Logic Thread
std::mutex marqueeDisplayMutex;
std::string displayMarquee = "";

// Display Thread & Keyboard Handler Thread
std::atomic<bool> backspacePressed{ false };
std::mutex keyboardDisplayMutex;
std::string displayCommand = "";

// Display Thread Exclusive
std::vector<std::vector<std::string>> registeredCommands;