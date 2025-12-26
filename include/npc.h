#ifndef NPC_H
#define NPC_H

#include <string>
#include <memory>
#include <mutex>

class NPCVisitor;

class NPC {
protected:
    int x, y;
    std::string name;
    bool alive;
    mutable std::mutex mtx;
    int moveDistance;
    int killDistance;

public:
    NPC(int x, int y, const std::string& name, int moveDist, int killDist);
    virtual ~NPC() = default;

    int getX() const;
    int getY() const;
    const std::string& getName() const;
    bool isAlive() const;
    void markDead();
    int getMoveDistance() const;
    int getKillDistance() const;

    virtual void accept(NPCVisitor& visitor) = 0;
    virtual std::string getType() const = 0;

    bool isClose(const NPC& other, int range) const;
    void moveRandom(int maxX, int maxY);
    int rollDice() const;
    bool isWithinDistance(const NPC& other, int distance) const;
    double getDistanceTo(const NPC& other) const;
};

class Bear : public NPC {
public:
    Bear(int x, int y, const std::string& name);
    void accept(NPCVisitor& visitor) override;
    std::string getType() const override;
};

class Robber : public NPC {
public:
    Robber(int x, int y, const std::string& name);
    void accept(NPCVisitor& visitor) override;
    std::string getType() const override;
};

class Werewolf : public NPC {
public:
    Werewolf(int x, int y, const std::string& name);
    void accept(NPCVisitor& visitor) override;
    std::string getType() const override;
};

#endif