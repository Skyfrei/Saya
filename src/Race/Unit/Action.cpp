#include "Action.h"
#include "../../Living.h"
#include "../Structure/Structure.h"
#include "Unit.h"
#include "../Structure/TownHall.h"

std::string Action::Serialize(){
    int actionType = static_cast<int>(type);
    std::string result = std::to_string(actionType) + ",";

    switch(type){
        case MOVE:{
            MoveAction* action = dynamic_cast<MoveAction*>(this);
            result += std::to_string(action->prevCoord.x) + "," +
                    std::to_string(action->prevCoord.y) + "," +
                    std::to_string(action->destCoord.x) + ","+
                    std::to_string(action->destCoord.y);
            break;
        }

        case ATTACK:{
            AttackAction* action = dynamic_cast<AttackAction*>(this);
            result += std::to_string(action->prevCoord.x) + "," +
                    std::to_string(action->prevCoord.y) + "," +
                    action->unit->Serialize() + ","; // not done
            break;
        }

        case BUILD:{
            BuildAction* action = dynamic_cast<BuildAction*>(this);
            result += std::to_string(action->prevCoord.x) + "," +
                    std::to_string(action->prevCoord.y) + "," +
                    action->stru->Serialize() + "," +
                    action->peasant->Serialize();
            break;
        }

        case FARMGOLD:{
            FarmGoldAction* action = dynamic_cast<FarmGoldAction*>(this);
            result += std::to_string(action->prevCoord.x) + "," +
                    std::to_string(action->prevCoord.y) + "," +
                    std::to_string(action->destCoord.x) + "," +
                    std::to_string(action->destCoord.y) + "," +
                    action->peasant->Serialize() + "," +
                    action->hall->Serialize() + ",";
                    //not done, gold and terrain missing

            break;
        }

        case RECRUIT:{
            RecruitSoldierAction* action = dynamic_cast<RecruitSoldierAction*>(this);

            int uType = static_cast<int>(action->unitType);
            result += std::to_string(uType) + "," + 
                    action->stru->Serialize() + ",";
            break;
        }
    }

    return result;
}
actionT Action::Deserialize(){

}
MoveAction::MoveAction(Vec2 c) : destCoord(c) {type = MOVE;}
MoveAction::MoveAction(Unit* un, Vec2 c) : destCoord(c), unit(un){
    type = MOVE;
    prevCoord = un->coordinate;
}
AttackAction::AttackAction(){type = ATTACK;}
AttackAction::AttackAction(Living* o) : object(o){type = ATTACK;}
AttackAction::AttackAction(Unit* un, Living* o) : object(o), unit(un){type = ATTACK;}
BuildAction::BuildAction(Structure *s) : stru(s) {type = BUILD;}
BuildAction::BuildAction(Unit* p, StructureType s, Vec2 c) : peasant(p), struType(s), coordinate(c){type = BUILD;} 
FarmGoldAction::FarmGoldAction(Vec2 v, Terrain *te, TownHall *t) : destCoord(v), terr(te), hall(t){type = FARMGOLD;}
FarmGoldAction::FarmGoldAction(Unit *p, Vec2 v, TownHall *h) : peasant(p), destCoord(v), hall(h){type = FARMGOLD;}
RecruitSoldierAction::RecruitSoldierAction(UnitType typeOfUnit, Structure* s) : unitType(typeOfUnit), stru(s){type = RECRUIT;}
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
    if (a.destCoord.x == destCoord.x && a.destCoord.y == destCoord.y) return true;
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

