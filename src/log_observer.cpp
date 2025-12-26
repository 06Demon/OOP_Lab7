#include "../include/log_observer.h"
#include <iostream>

LogObserver::LogObserver(const std::string& filename) {
    logFile.open(filename, std::ios::app);
}

LogObserver::~LogObserver() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void LogObserver::onKill(const std::string& killer, const std::string& victim) {
    if (logFile.is_open()) {
        logFile << killer << " killed " << victim << std::endl;
    }
}