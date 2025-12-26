#include "../include/npc_factory.h"
#include "../include/npc.h"
#include <sstream>

std::shared_ptr<NPC> NPCFactory::createNPC(const std::string& type, int x, int y, const std::string& name) {
    if (type == "Bear") {
        return std::make_shared<Bear>(x, y, name);
    } else if (type == "Robber") {
        return std::make_shared<Robber>(x, y, name);
    } else if (type == "Werewolf") {
        return std::make_shared<Werewolf>(x, y, name);
    }
    return nullptr;
}

std::shared_ptr<NPC> NPCFactory::loadFromString(const std::string& data) {
    std::string type, name;
    int x, y;
    std::istringstream iss(data);
    iss >> type >> x >> y;
    std::getline(iss, name);
    if (!name.empty() && name[0] == ' ') name.erase(0, 1);
    return createNPC(type, x, y, name);
}