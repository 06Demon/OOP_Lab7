#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include "npc.h"
#include "observer.h"

class GameWorld {
private:
    std::vector<std::shared_ptr<NPC>> npcs;
    mutable std::shared_mutex npcsMtx;
    
    std::vector<std::pair<std::shared_ptr<NPC>, std::shared_ptr<NPC>>> battleQueue;
    std::mutex battleQueueMtx;
    std::condition_variable battleCV;
    
    std::vector<Observer*> observers;
    std::mutex observersMtx;
    
    std::atomic<bool> gameRunning;
    std::atomic<int> survivorsCount;
    
    const int WORLD_SIZE_X = 100;
    const int WORLD_SIZE_Y = 100;
    const int TOTAL_NPCS = 50;
    
    void generateInitialNPCs();
    bool checkBattleCondition(const std::shared_ptr<NPC>& a, const std::shared_ptr<NPC>& b);
    void performBattle(const std::shared_ptr<NPC>& attacker, const std::shared_ptr<NPC>& defender);
    void notifyKill(const std::string& killer, const std::string& victim);
    
public:
    GameWorld();
    ~GameWorld();
    
    void addObserver(Observer* observer);
    void removeObserver(Observer* observer);
    
    void startGame();
    void stopGame();

    void movementThread();
    void battleThread();
    void mainThread();
    
    void printMap() const;
    void printSurvivors() const;
    
    int getWorldSizeX() const { return WORLD_SIZE_X; }
    int getWorldSizeY() const { return WORLD_SIZE_Y; }
};

#endif