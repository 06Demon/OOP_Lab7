#include "../include/battle_visitor.h"
#include "../include/npc.h"

BattleVisitor::BattleVisitor(NPC* other) : other(other), fightResult(false), bothDie(false) {}

void BattleVisitor::visit(Bear& bear) {
    if (dynamic_cast<Werewolf*>(other)) {
        fightResult = true;
    }
}

void BattleVisitor::visit(Robber& robber) {
    if (dynamic_cast<Bear*>(other)) {
        fightResult = true;
    }
}

void BattleVisitor::visit(Werewolf& werewolf) {
    if (dynamic_cast<Robber*>(other)) {
        fightResult = true;
    }
}

bool BattleVisitor::getFightResult() const { return fightResult; }
bool BattleVisitor::getBothDie() const { return bothDie; }