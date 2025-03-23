#include "Action.h"
#include "../../Living.h"
#include "../Structure/Structure.h"

MoveAction::MoveAction(Vec2 c) : destCoord(c) {}
MoveAction::MoveAction(Unit* un, Vec2 c) : destCoord(c), unit(un){}
AttackAction::AttackAction(){}
AttackAction::AttackAction(Living* o) : object(o){}
AttackAction::AttackAction(Unit* un, Living* o) : object(o), unit(un){}
BuildAction::BuildAction(Structure *s) : stru(s) {}
BuildAction::BuildAction(Unit* p, StructureType s, Vec2 c) : peasant(p), struType(s), coordinate(c){} 
FarmGoldAction::FarmGoldAction(Vec2 v, Terrain *te, TownHall *t) : dest(v), terr(te), hall(t){}
FarmGoldAction::FarmGoldAction(Unit *p, Vec2 v, TownHall *h) : peasant(p), dest(v), hall(h){}
RecruitSoldierAction::RecruitSoldierAction(UnitType type, Structure* s) : unitType(type), stru(s){}
bool MoveAction::operator==(const MoveAction &b) const {
    if (destCoord.x == b.destCoord.x && destCoord.y == b.destCoord.y)
        return true;
    return false;
}
bool AttackAction::operator==(const AttackAction &b) const {
    return object == b.object;
}
bool BuildAction::operator==(const BuildAction &b) const {
    if (stru->coordinate.x == b.stru->coordinate.x &&
        stru->coordinate.y == b.stru->coordinate.y && stru == b.stru)
        return true;
    return false;
}
bool FarmGoldAction::operator==(const FarmGoldAction &a) const {
    if (a.dest.x == dest.x && a.dest.y == dest.y) return true;
    return false;
}
bool RecruitSoldierAction::operator==(const RecruitSoldierAction& a) const {
    if (stru == a.stru && unitType == a.unitType) return true;
    return false;
}
ActionType MoveAction::GetType(){
    return type;
}
ActionType AttackAction::GetType(){
    return type;
}
ActionType BuildAction::GetType(){
    return type;
}
ActionType FarmGoldAction::GetType(){
    return type;
}
ActionType RecruitSoldierAction::GetType(){
    return type;
}

