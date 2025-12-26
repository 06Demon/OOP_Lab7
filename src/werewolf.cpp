#include "../include/npc.h"
#include "../include/npc_visitor.h"

Werewolf::Werewolf(int x, int y, const std::string& name)
    : NPC(x, y, name, 40, 5) {}

void Werewolf::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

std::string Werewolf::getType() const {
    return "Werewolf";
}