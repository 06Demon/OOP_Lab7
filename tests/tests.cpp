#include <gtest/gtest.h>
#include "../include/npc.h"
#include "../include/npc_factory.h"
#include <sstream>
#include <memory>

TEST(npc, Basic) {
    NPCFactory factory;
    std::shared_ptr<NPC> bear = factory.createNPC("Bear", 123, 10, "Bear1");
    std::shared_ptr<NPC> werewolf = factory.createNPC("Werewolf", 111, 10, "Werewolf1");
    std::shared_ptr<NPC> robber = factory.createNPC("Robber", 333, 10, "Robber1");
}
