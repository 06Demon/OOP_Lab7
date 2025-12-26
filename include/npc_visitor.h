#ifndef NPCVISITOR_H
#define NPCVISITOR_H

class Bear;
class Robber;
class Werewolf;

class NPCVisitor {
public:
    virtual ~NPCVisitor() = default;
    virtual void visit(Bear& bear) = 0;
    virtual void visit(Robber& robber) = 0;
    virtual void visit(Werewolf& werewolf) = 0;
};

#endif