#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onKill(const std::string& killer, const std::string& victim) = 0;
};

#endif