#ifndef ACTION_H
#define ACTION_H

#include <cstddef>
#include <variant>
#include "../../Tools/Enums.h"
#include "../../Tools/Binary.h"
#include "../../Tools/Vec2.h"
#include <deque>
#include <string>
#include <vector>

class TownHall;
class Unit;
class Living;
class Structure;
class Terrain;
struct MoveAction;
struct AttackAction;
struct FarmGoldAction;
struct RecruitAction;
struct BuildAction;

using actionT = std::variant<AttackAction, MoveAction,
                             BuildAction, FarmGoldAction, RecruitAction>;

Unit* GetUnit(std::vector<binary>& bin);
Structure* GetStructure(std::vector<binary>& bin);

class Action{
    public:
        ActionType type;
        virtual ActionType GetType() = 0;
        std::string Serialize();
        actionT Deserialize();
        std::deque<binary> SerializeBinary();
        actionT DeserializeBinary(std::deque<binary>& bin);
};
struct MoveAction : public Action {
    MoveAction(Vec2 c);
    MoveAction(Unit* un, Vec2 c);
    ActionType GetType() override;
    bool operator==(const MoveAction &b) const;
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
};
struct BuildAction : public Action{
    BuildAction(Structure *s);
    BuildAction(Unit* p, StructureType s, Vec2 c);
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
    FarmGoldAction(Unit *p, Vec2 v);    

    ActionType GetType() override;
    bool operator==(const FarmGoldAction &a) const;

    Vec2 destCoord;
    Vec2 prevCoord;
    Unit* peasant;
    Terrain *terr;
    TownHall *hall;
    
    int gold = 0;
};
struct RecruitAction : public Action{
    
    RecruitAction(UnitType typeOfUnit, Structure* s);
    ActionType GetType() override;
    bool operator==(const RecruitAction &a) const;
    UnitType unitType;
    Structure *stru;
};
#endif
