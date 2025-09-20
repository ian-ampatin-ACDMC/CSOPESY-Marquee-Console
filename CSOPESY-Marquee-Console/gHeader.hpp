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

// --- Shared State and Thread Control ---
// Global flat to signal all threads to exit
extern std::string marqueeText;
extern std::atomic<bool> isRunning;
extern std::atomic<bool> marqueeRunning;

// The command interpreter and display thread share this variable
extern std::string promptDisplayBuffer;
extern std::mutex promptMutex;

// Shared state for the keyboard handler and command interpreter
extern std::queue<std::string> commandQueue;
extern std::mutex commandQueueMutex;

// The marquee logic thread and display thread share this variable
extern std::vector<std::string> marqueeSubStrings;
extern std::string marqueeDisplayBuffer;
extern std::mutex marqueeDisplayMutex;

// The main thread and marquee logic thread share this variable
extern std::string systemPromptText;
extern std::mutex mainMarqueeMutex;
extern std::atomic<bool> printHelp;
extern std::atomic<bool> printText;
extern std::atomic<bool> printPrompt;
extern size_t marqueeSpeed;

// The main thread and display logic thread share this variable
extern std::mutex mainDisplayMutex;

// --- Utility Functions ---
void clearScreen();
void gotoxy(int x, int y);
void printHelpFunction();
void restartDisplay();
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