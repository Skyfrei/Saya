#ifndef __DEATH_MANAGER_H__
#define __DEATH_MANAGER_H__

#include <vector>
#include <variant>
#include "Player.h"
#include "../Tools/Enums.h"

class DeathManager {
public:
    static DeathManager& GetSingleton();
    static void Init(Player* pl, Player* en);

    void RemoveFromAttackAction(Living* deadEntity, Side side);
    protected:
        static DeathManager* instance;

private:
    DeathManager() = delete;
    DeathManager(Player* p, Player* e) : pl(p), en(e){}
    DeathManager(const DeathManager&) = delete;
    void operator=(const DeathManager&) = delete;

    Player* pl;
    Player* en;
};
#endif
