#include "../include/console_observer.h"
#include <iostream>

void ConsoleObserver::onKill(const std::string& killer, const std::string& victim) {
    if (victim == "each other") {
        std::cout << "[BATTLE] " << killer << " kill each other" << std::endl;
    } else {
        std::cout << "[BATTLE] " << killer << " kills " << victim << std::endl;
    }
}