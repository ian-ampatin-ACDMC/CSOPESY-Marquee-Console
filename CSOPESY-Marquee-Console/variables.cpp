#include "gHeader.hpp"

// --- Shared State and Thread Control ---
// Global flat to signal all threads to exit
std::string marqueeText = "[CSOPESY] Marquee Console";
std::atomic<bool> isRunning{ true };
std::atomic<bool> marqueeRunning{ true };
int xCoordinateCommand = 65;
int yCoordinateCommand = 7;
int lengthOfDisplay = 60;
size_t refreshRate = 50;

// The command interpreter and display thread share this variable
std::string promptDisplayBuffer = "";
std::mutex promptMutex;

// Shared state for the keyboard handler and command interpreter
std::queue<std::string> commandQueue;
std::mutex commandQueueMutex;

// The marquee logic thread and display thread share this variable
std::string displayMarquee = "";
std::mutex marqueeDisplayMutex;

// The keyboard handler thread and the display thread share this variable
std::string displayCommand = "";
std::mutex keyboardDisplayMutex;
std::atomic<bool> backspacePressed{ false };

// The main thread and marquee logic thread share this variable
std::string systemPromptText = "";
std::mutex mainMarqueeMutex;
std::atomic<bool> printHelp{ false };
std::atomic<bool> printPrompt{ false };
size_t marqueeSpeed = 100;

// The main thread and display logic thread share this variable
std::mutex mainDisplayMutex;
std::vector<std::vector<std::string>> registeredCommands;