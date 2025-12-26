#include "../include/npc.h"
#include "../include/npc_visitor.h"
#include <cmath>
#include <random>
#include <chrono>

NPC::NPC(int x, int y, const std::string& name, int moveDist, int killDist)
    : x(x), y(y), name(name), alive(true), 
      moveDistance(moveDist), killDistance(killDist) {}

int NPC::getX() const { 
    std::lock_guard<std::mutex> lock(mtx);
    return x; 
}

int NPC::getY() const { 
    std::lock_guard<std::mutex> lock(mtx);
    return y; 
}

const std::string& NPC::getName() const { 
    return name; 
}

bool NPC::isAlive() const { 
    return alive; 
}

void NPC::markDead() { 
    alive = false; 
}

int NPC::getMoveDistance() const { 
    return moveDistance; 
}

int NPC::getKillDistance() const { 
    return killDistance; 
}

bool NPC::isClose(const NPC& other, int range) const {
    if (!alive || !other.alive) return false;
    
    std::lock_guard<std::mutex> lock1(mtx);
    std::lock_guard<std::mutex> lock2(other.mtx);
    
    int dx = x - other.x;
    int dy = y - other.y;
    return (dx * dx + dy * dy) <= (range * range);
}

void NPC::moveRandom(int maxX, int maxY) {
    if (!alive) return;
    
    std::lock_guard<std::mutex> lock(mtx);
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dirDist(-1, 1);
    
    int newX = x + dirDist(gen) * moveDistance;
    int newY = y + dirDist(gen) * moveDistance;
    
    if (newX < 0) newX = 0;
    if (newX >= maxX) newX = maxX - 1;
    if (newY < 0) newY = 0;
    if (newY >= maxY) newY = maxY - 1;
    
    x = newX;
    y = newY;
}

int NPC::rollDice() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 6);
    return dist(gen);
}

bool NPC::isWithinDistance(const NPC& other, int distance) const {
    if (!alive || !other.alive) return false;
    
    std::lock_guard<std::mutex> lock1(mtx);
    std::lock_guard<std::mutex> lock2(other.mtx);
    
    int dx = x - other.x;
    int dy = y - other.y;
    return (dx * dx + dy * dy) <= (distance * distance);
}

double NPC::getDistanceTo(const NPC& other) const {
    if (!alive || !other.alive) return -1.0;
    
    std::lock_guard<std::mutex> lock1(mtx);
    std::lock_guard<std::mutex> lock2(other.mtx);
    
    int dx = x - other.x;
    int dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}