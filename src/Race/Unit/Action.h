#ifndef ACTION_H
#define ACTION_H

#include <cstddef>
#include <variant>
#include "../../Tools/Enums.h"
#include "../../Tools/Vec2.h"

class TownHall;
class Unit;
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
    constexpr bool operator==(const MoveAction &b) const {
        if (destCoord.x == b.destCoord.x && destCoord.y == b.destCoord.y)
            return true;
          return false;
    }

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
    constexpr bool operator==(const AttackAction &b) const {
        return object == b.object;
    }

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
    constexpr bool operator==(const BuildAction &b) const {
        if (stru->coordinate.x == b.stru->coordinate.x &&
            stru->coordinate.y == b.stru->coordinate.y && stru == b.stru)
            return true;
        return false;
    }

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
    constexpr bool operator==(const FarmGoldAction &a) const {
        if (a.dest.x == dest.x && a.dest.y == dest.y) return true;
        return false;
    }
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
    constexpr bool operator==(const RecruitSoldierAction &a) const {
        if (stru == a.stru && unitType == a.unitType) return true;
        return false;
    }
    UnitType unitType;
    Structure *stru;
    ActionType type = RECRUIT;
};
struct RecruitAction {
    Unit *un;
    constexpr bool operator==(const RecruitAction& other){
      return un == other.un;
    }
};
#endif
