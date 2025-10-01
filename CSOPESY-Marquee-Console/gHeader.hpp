#ifndef GHEADER
#define GHEADER

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <queue>
#include <cstdlib>
#include <conio.h>
#include <Windows.h>
#include <stdexcept>
#include <fstream>

// --- Shared State and Thread Control ---
// Global flag to signal all threads to exit
extern std::atomic<bool> isRunning;

// Multiple Thread Usage
extern std::atomic<bool> marqueeRunning;
extern std::vector<std::string> introductionText;
extern std::string marqueeText;
extern size_t refreshRate;
extern size_t lengthOfDisplay;
extern size_t commandDisplayLength;

// Command Interpreter & Display Thread
extern std::atomic<bool> printHelp;
extern std::atomic<bool> printPrompt;
extern std::mutex promptMutex;
extern std::mutex commandDisplayMutex;
extern std::string systemPromptText;

// Command Interpreter & Marquee Logic Thraed
extern std::mutex commandMarqueeMutex;
extern size_t marqueeSpeed;

// Command Interpreter & Keyborad Handler
extern std::mutex commandQueueMutex;
extern std::queue<std::string> commandQueue;

// Display Thread & Marquee Logic Thread
extern std::mutex marqueeDisplayMutex;
extern std::string displayMarquee;

// Display Thread & Keyboard Handler Thread
extern std::atomic<bool> newCommandStarted;
extern std::atomic<bool> newCommandAccept;
extern std::mutex keyboardDisplayMutex;
extern std::string displayCommand;

// Display Thread Exclusive
extern std::vector<std::vector<std::string>> registeredCommands;
extern size_t startingPrintRow;

// --- Utility Functions ---
void clearScreen();
void gotoxy(size_t x, size_t y);
void printHelpFunction();
void disableEcho();
void enableEcho();
void formattedPrint(std::vector<std::string> lines);
std::vector<std::string> getToken(std::string line);

// --- Thread Function --- 
void keyboardHandlerThreadFunction();
void marqueeLogicThreadFunction(int displayWidth);
void displayThreadFunction();

// --- Command Functions ---
void setMarqueeText(std::vector<std::string> tokens);
void setMarqueeSpeed(std::vector<std::string> tokens);
void systemPrompt(std::string prompt);

#endif GHEADER