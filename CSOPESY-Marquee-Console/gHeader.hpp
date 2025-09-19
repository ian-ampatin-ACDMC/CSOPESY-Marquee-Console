#pragma once

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

// --- Shared State and Thread Control ---
// Global flat to signal all threads to exit
std::string marqueeText = "CSOPESY --- Marquee Console";
std::atomic<bool> isRunning{ true };
std::atomic<bool> marqueeRunning{ true };

// The command interpreter and display thread share this variable
std::string promptDisplayBuffer = "";
std::mutex promptMutex;

// Shared state for the keyboard handler and command interpreter
std::queue<std::string> commandQueue;
std::mutex commandQueueMutex;

// The marquee logic thread and display thread share this variable
std::string marqueeDisplayBuffer = "";
std::mutex marqueeDisplayMutex;

// The main thread and marquee logic thread share this variable
std::mutex mainMarqueeMutex;