#ifndef CONSOLEOBSERVER_H
#define CONSOLEOBSERVER_H

#include "observer.h"
#include <mutex>

class ConsoleObserver : public Observer {
private:
    std::mutex coutMutex;

public:
    void onKill(const std::string& killer, const std::string& victim) override;
};

#endif