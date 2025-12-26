#ifndef LOGOBSERVER_H
#define LOGOBSERVER_H

#include "observer.h"
#include <fstream>
#include <mutex>

class LogObserver : public Observer {
private:
    std::ofstream logFile;
    std::mutex fileMutex;

public:
    LogObserver(const std::string& filename);
    ~LogObserver();
    void onKill(const std::string& killer, const std::string& victim) override;
};

#endif