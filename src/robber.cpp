#include "../include/npc.h"
#include "../include/npc_visitor.h"

Robber::Robber(int x, int y, const std::string& name)
    : NPC(x, y, name, 10, 10) {}

void Robber::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

std::string Robber::getType() const {
    return "Robber";
}