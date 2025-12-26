#ifndef BATTLEVISITOR_H
#define BATTLEVISITOR_H

#include "npc_visitor.h"
#include "npc.h"
#include <memory>

class BattleVisitor : public NPCVisitor {
private:
    NPC* other;
    bool fightResult;
    bool bothDie;

public:
    BattleVisitor(NPC* other);
    void visit(Bear& bear) override;
    void visit(Werewolf& werewolf) override;
    void visit(Robber& robber) override;

    bool getFightResult() const;
    bool getBothDie() const;
};

#endif