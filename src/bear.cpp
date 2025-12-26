#include "../include/npc.h"
#include "../include/npc_visitor.h"

Bear::Bear(int x, int y, const std::string& name) 
    : NPC(x, y, name, 5, 10) {}

void Bear::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

std::string Bear::getType() const {
    return "Bear";
}