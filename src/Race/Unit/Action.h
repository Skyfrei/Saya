#ifndef ACTION_H
#define ACTION_H

#include <cstddef>
#include <variant>
#include "../../Tools/Enums.h"
#include "../../Tools/Vec2.h"

class TownHall;
class Unit;
class Living;
class Structure;
class Terrain;
struct MoveAction;
struct AttackAction;
struct FarmGoldAction;
struct RecruitSoldierAction;
struct BuildAction;

using actionT = std::variant<std::monostate, AttackAction, MoveAction,
                             BuildAction, FarmGoldAction, RecruitSoldierAction>;
class Action{
    public:
        ActionType type;
        virtual ActionType GetType() = 0;
};
struct MoveAction : public Action {
    MoveAction(Vec2 c);
    MoveAction(Unit* un, Vec2 c);
    ActionType GetType() override;
    bool operator==(const MoveAction &b) const;

    ActionType type = MOVE;
    Vec2 prevCoord;
    Vec2 destCoord;
    Unit* unit;

};
struct AttackAction : public Action{
    AttackAction();
    AttackAction(Living* o);
    AttackAction(Unit* un, Living* o);
    ActionType GetType() override;
    bool operator==(const AttackAction &b) const;

    Vec2 prevCoord;
    Living *object;
    Unit* unit;
    ActionType type = ATTACK;
};
struct BuildAction : public Action{
    BuildAction(Structure *s);
    BuildAction(Unit* p, StructureType s, Vec2 c);
    ActionType type = BUILD;
    ActionType GetType() override;
    bool operator==(const BuildAction &b) const;

    Structure *stru;
    Vec2 prevCoord;
    StructureType struType;
    Vec2 coordinate;
    Unit* peasant;

};
struct FarmGoldAction : public Action {
    FarmGoldAction(Vec2 v, Terrain *te, TownHall *t);
    FarmGoldAction(Unit *p, Vec2 v, TownHall *h);    
    ActionType type = FARMGOLD;
    ActionType GetType() override;
    bool operator==(const FarmGoldAction &a) const;

    Vec2 dest;
    Vec2 prev;
    Terrain *terr;
    TownHall *hall;
    Unit* peasant;
    int gold = 0;
};
struct RecruitSoldierAction : public Action{
    
    RecruitSoldierAction(UnitType type, Structure* s);
    ActionType GetType() override;
    bool operator==(const RecruitSoldierAction &a) const;
    UnitType unitType;
    Structure *stru;
    ActionType type = RECRUIT;
};
#endif
