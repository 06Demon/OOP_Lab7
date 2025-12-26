#ifndef NPCFACTORY_H
#define NPCFACTORY_H

#include <memory>
#include <string>
#include "npc.h"

class NPCFactory {
public:
    static std::shared_ptr<NPC> createNPC(const std::string& type, int x, int y, const std::string& name);
    static std::shared_ptr<NPC> loadFromString(const std::string& data);
};
#endif