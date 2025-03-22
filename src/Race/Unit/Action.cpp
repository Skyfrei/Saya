#include "Action.h"

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

