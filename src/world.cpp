#include "../include/world.h"
#include "../include/npc_factory.h"
#include "../include/battle_visitor.h"
#include "../include/log_observer.h"
#include "../include/console_observer.h"
#include <iostream>
#include <random>
#include <thread>
#include <iomanip>
#include <chrono>
#include <algorithm>

GameWorld::GameWorld() : gameRunning(false), survivorsCount(0) {
    static LogObserver logObserver("log.txt");
    static ConsoleObserver consoleObserver;
    
    addObserver(&logObserver);
    addObserver(&consoleObserver);
    
    generateInitialNPCs();
}

GameWorld::~GameWorld() {
    stopGame();
}

void GameWorld::addObserver(Observer* observer) {
    std::lock_guard<std::mutex> lock(observersMtx);
    observers.push_back(observer);
}

void GameWorld::removeObserver(Observer* observer) {
    std::lock_guard<std::mutex> lock(observersMtx);
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void GameWorld::notifyKill(const std::string& killer, const std::string& victim) {
    std::lock_guard<std::mutex> lock(observersMtx);
    for (auto observer : observers) {
        observer->onKill(killer, victim);
    }
}

void GameWorld::generateInitialNPCs() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> coordDist(0, 99);
    std::uniform_int_distribution<> typeDist(0, 2);
    
    std::vector<std::string> types = {"Bear", "Robber", "Werewolf"};
    
    for (int i = 0; i < TOTAL_NPCS; ++i) {
        int x = coordDist(gen);
        int y = coordDist(gen);
        std::string type = types[typeDist(gen)];
        std::string name = type + "_" + std::to_string(i);
        
        auto npc = NPCFactory::createNPC(type, x, y, name);
        if (npc) {
            std::unique_lock<std::shared_mutex> lock(npcsMtx);
            npcs.push_back(npc);
        }
    }
    
    survivorsCount = TOTAL_NPCS;
}

bool GameWorld::checkBattleCondition(const std::shared_ptr<NPC>& a, const std::shared_ptr<NPC>& b) {
    if (!a->isAlive() || !b->isAlive()) return false;
    
    BattleVisitor visitorA(b.get());
    BattleVisitor visitorB(a.get());
    
    a->accept(visitorA);
    b->accept(visitorB);
    
    return visitorA.getFightResult() || visitorB.getFightResult();
}

void GameWorld::performBattle(const std::shared_ptr<NPC>& npc1, const std::shared_ptr<NPC>& npc2) {
    if (!npc1->isAlive() || !npc2->isAlive()) return;
    
    int killDist1 = npc1->getKillDistance();
    int killDist2 = npc2->getKillDistance();
    
    bool npc1CanAttack = npc1->isWithinDistance(*npc2, killDist1);
    bool npc2CanAttack = npc1->isWithinDistance(*npc2, killDist2);
    double distance = npc1->getDistanceTo(*npc2);
    
    BattleVisitor visitor1(npc2.get());
    BattleVisitor visitor2(npc1.get());
    npc1->accept(visitor1);
    npc2->accept(visitor2);
    
    bool npc1CanKill = visitor1.getFightResult();
    bool npc2CanKill = visitor2.getFightResult();

    if ((!npc1CanAttack || !npc1CanKill) && (!npc2CanAttack || !npc2CanKill)) {
        return;
    }
    
    static std::mutex coutMutex;
    std::lock_guard<std::mutex> coutLock(coutMutex);
    
    std::cout << "[BATTLE] " << npc1->getName() <<  " vs " << npc2->getName() << "\n";
    std::cout << "  Distance: " << distance << "m\n";
    
    int atk1 = npc1CanAttack && npc1CanKill ? npc1->rollDice() : 0;
    int def1 = npc2CanAttack && npc2CanKill ? npc1->rollDice() : 0;
    int atk2 = npc2CanAttack && npc2CanKill ? npc2->rollDice() : 0;
    int def2 = npc1CanAttack && npc1CanKill ? npc2->rollDice() : 0;
    
    std::cout << "  " << npc1->getName() << ": ";
    if (npc1CanAttack && npc1CanKill) std::cout << "ATK=" << atk1 << ", ";
    std::cout << "DEF=" << def1 << "\n";
    
    std::cout << "  " << npc2->getName() << ": ";
    if (npc2CanAttack && npc2CanKill) std::cout << "ATK=" << atk2 << ", ";
    std::cout << "DEF=" << def2 << "\n";
    
    bool npc1Dies = (npc2CanAttack && npc2CanKill && atk2 > def1);
    bool npc2Dies = (npc1CanAttack && npc1CanKill && atk1 > def2);
    
    
    if (npc1Dies) {
        npc1->markDead();
        survivorsCount--;
        std::cout << "  RESULT: " << npc2->getName() << " wins!\n";
        notifyKill(npc2->getName(), npc1->getName());
    }
    else if (npc2Dies) {
        npc2->markDead();
        survivorsCount--;
        std::cout << "  RESULT: " << npc1->getName() << " wins!\n";
        notifyKill(npc1->getName(), npc2->getName());
    }
    else {
        std::cout << "  RESULT: Draw!\n";
    }
}


void GameWorld::movementThread() {
    while (gameRunning) {
        {
            std::shared_lock<std::shared_mutex> lock(npcsMtx);
            for (auto& npc : npcs) {
                if (npc->isAlive()) {
                    npc->moveRandom(WORLD_SIZE_X, WORLD_SIZE_Y);
                }
            }

            for (size_t i = 0; i < npcs.size(); ++i) {
                if (!npcs[i]->isAlive()) continue;
                
                for (size_t j = i + 1; j < npcs.size(); ++j) {
                    if (!npcs[j]->isAlive()) continue;

                    int killDistI = npcs[i]->getKillDistance();
                    int killDistJ = npcs[j]->getKillDistance();
                    
                    bool iCanAttackJ = npcs[i]->isWithinDistance(*npcs[j], killDistI);
                    bool jCanAttackI = npcs[i]->isWithinDistance(*npcs[j], killDistJ);

                    if (iCanAttackJ || jCanAttackI) {
                        if (checkBattleCondition(npcs[i], npcs[j])) {
                            std::lock_guard<std::mutex> battleLock(battleQueueMtx);

                            battleQueue.push_back({npcs[i], npcs[j]});
                            battleCV.notify_one();
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void GameWorld::battleThread() {
    while (gameRunning) {
        std::pair<std::shared_ptr<NPC>, std::shared_ptr<NPC>> battlePair;
        
        {
            std::unique_lock<std::mutex> lock(battleQueueMtx);
            battleCV.wait_for(lock, std::chrono::milliseconds(100), 
                [this]() { return !battleQueue.empty() || !gameRunning; });
            
            if (!gameRunning) break;
            
            if (!battleQueue.empty()) {
                battlePair = battleQueue.front();
                battleQueue.erase(battleQueue.begin());
            } else {
                continue;
            }
        }
        
        performBattle(battlePair.first, battlePair.second);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void GameWorld::mainThread() {
    auto startTime = std::chrono::steady_clock::now();
    
    while (gameRunning) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
        
        if (elapsed.count() >= 30) {
            stopGame();
            break;
        }
        
        printMap();
        
        {
            std::shared_lock<std::shared_mutex> lock(npcsMtx);
            int count = 0;
            for (const auto& npc : npcs) {
                if (npc->isAlive()) count++;
            }
            survivorsCount = count;
        }
        
        std::cout << "\nTime: " << elapsed.count() << "s, Survivors: " << survivorsCount << "\n";
        std::cout << "========================================\n";
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    printSurvivors();
}

void GameWorld::printMap() const {
    static std::mutex coutMutex;
    std::lock_guard<std::mutex> coutLock(coutMutex);
    
    std::cout << "\n\n   =============== GAME MAP ================\n";
    
    const int DISPLAY_SIZE = 20;
    char map[DISPLAY_SIZE][DISPLAY_SIZE];
    
    for (int i = 0; i < DISPLAY_SIZE; ++i) {
        for (int j = 0; j < DISPLAY_SIZE; ++j) {
            map[i][j] = '.';
        }
    }

    std::shared_lock<std::shared_mutex> lock(npcsMtx);
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            int displayX = (npc->getX() * DISPLAY_SIZE) / WORLD_SIZE_X;
            int displayY = (npc->getY() * DISPLAY_SIZE) / WORLD_SIZE_Y;
            
            if (displayX >= 0 && displayX < DISPLAY_SIZE && 
                displayY >= 0 && displayY < DISPLAY_SIZE) {
                
                char symbol = '.';
                std::string type = npc->getType();
                if (type == "Bear") symbol = 'B';
                else if (type == "Robber") symbol = 'R';
                else if (type == "Werewolf") symbol = 'W';
                
                map[displayY][displayX] = symbol;
            }
        }
    }


    for (int i = 0; i < DISPLAY_SIZE; ++i) {
        std::cout << std::setw(2) << i << "| ";
        for (int j = 0; j < DISPLAY_SIZE; ++j) {
            std::cout << map[i][j] << " ";
        }
        std::cout << "|\n";
    }
    
    std::cout << "   ";
    for (int j = 0; j < DISPLAY_SIZE; ++j) {
        std::cout << "--";
    }
    std::cout << "\n   ";
    for (int j = 0; j < DISPLAY_SIZE; ++j) {
        std::cout << std::setw(2) << j;
    }
}

void GameWorld::printSurvivors() const {
    static std::mutex coutMutex;
    std::lock_guard<std::mutex> coutLock(coutMutex);
    
    std::cout << "\n\n=== GAME OVER ===\n";
    std::cout << "=== SURVIVORS (" << survivorsCount << ") ===\n";
    
    std::shared_lock<std::shared_mutex> lock(npcsMtx);
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            std::cout << npc->getName() 
                      << " at (" << npc->getX() << ", " << npc->getY() << ")\n";
        }
    }
}

void GameWorld::startGame() {
    gameRunning = true;
    
    std::thread movement(&GameWorld::movementThread, this);
    std::thread battle(&GameWorld::battleThread, this);
    std::thread main(&GameWorld::mainThread, this);
    main.join();
    gameRunning = false;
    battleCV.notify_all();
    
    movement.join();
    battle.join();
}

void GameWorld::stopGame() {
    gameRunning = false;
    battleCV.notify_all();
}