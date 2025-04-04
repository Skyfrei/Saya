#include "Action.h"
#include "../../Living.h"
#include "../Structure/Structure.h"
#include "Unit.h"
#include "../Structure/TownHall.h"
#include "../Structure/Farm.h"
#include "../Structure/Barrack.h"
#include "Peasant.h"
#include "Footman.h"


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
            RecruitAction* action = dynamic_cast<RecruitAction*>(this);

            int uType = static_cast<int>(action->unitType);
            result += std::to_string(uType) + "," + 
                    action->stru->Serialize() + ",";
            break;
        }
    }

    return result;
}
actionT Action::Deserialize(){
    int actionType = static_cast<int>(type);
    std::string result = std::to_string(actionType) + ",";
    return MoveAction(Vec2(4,5));
}

std::deque<binary> Action::SerializeBinary(){
    std::deque<binary> result;
    int actionType = static_cast<int>(type);
    result.push_back(actionType);
    switch(type){
        case MOVE:{
            MoveAction* moveAction = dynamic_cast<MoveAction*>(this);
            std::vector<binary> unBin = moveAction->unit->SerializeBinary();
            result.insert(result.end(), unBin.begin(), unBin.end());
            result.push_back(moveAction->destCoord.x);
            result.push_back(moveAction->destCoord.y);
            break;
        }

        case ATTACK:{
            AttackAction* attackAction = dynamic_cast<AttackAction*>(this);
            std::vector<binary> unBin = attackAction->unit->SerializeBinary();
            result.insert(result.end(), unBin.begin(), unBin.end());
            Structure* objStru = dynamic_cast<Structure*>(attackAction->object);
            Unit* objUn = dynamic_cast<Unit*>(attackAction->object);
            if (objUn != nullptr){
                result.push_back(0);
                std::vector<binary> objectBin = objUn->SerializeBinary();
                result.insert(result.end(), objectBin.begin(), objectBin.end());
            }
            else if (objStru != nullptr){
                result.push_back(1);
                std::vector<binary> objectBin = objStru->SerializeBinary();
                result.insert(result.end(), objectBin.begin(), objectBin.end());
            } 
            break;
        }

        case BUILD:{
            BuildAction* buildAction = dynamic_cast<BuildAction*>(this);
            int structureType = static_cast<int>(buildAction->struType);
            std::vector<binary> unBin = buildAction->peasant->SerializeBinary();
            std::vector<binary> struBin = buildAction->stru->SerializeBinary();
            result.insert(result.end(), unBin.begin(), unBin.end());
            result.push_back(structureType);
            //result.insert(result.end(), struBin.begin(), struBin.end());
            result.push_back(buildAction->coordinate.x);
            result.push_back(buildAction->coordinate.y);
            break;
        }

        case FARMGOLD:{
            FarmGoldAction* farmAction = dynamic_cast<FarmGoldAction*>(this);
            std::vector<binary> unBin = farmAction->peasant->SerializeBinary();
            result.insert(result.end(), unBin.begin(), unBin.end());
            result.push_back(farmAction->terr->coord.x);
            result.push_back(farmAction->terr->coord.y);
            break;
        }

        case RECRUIT:{
            RecruitAction* recruitAction = dynamic_cast<RecruitAction*>(this);
            int unitType = static_cast<int>(recruitAction->unitType);
            std::vector<binary> struBin = recruitAction->stru->SerializeBinary();

            result.push_back(unitType);
            result.insert(result.end(), struBin.begin(), struBin.end());
            break;
        }
    }
    return result;
}

actionT Action::DeserializeBinary(std::deque<binary>& bin){
    int actionType = std::get<int>(bin[0]);
    bin.pop_front();
    switch(actionType){
        case 0:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            Unit* newUnit = GetUnit(package);
            Vec2 dest(std::get<int>(bin[5]), std::get<int>(bin[6]));
            MoveAction move(newUnit, dest);
            bin.erase(bin.begin(), bin.begin() + 7);
            return move;
        }

        case 1:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            Unit* newUnit = GetUnit(package);
            int unitOrStruct = std::get<int>(bin[5]);
            Unit* targetUnit;
            Structure* targetStructure;
            bin.erase(bin.begin(), bin.begin() + 6);
            if (unitOrStruct == 0){
                std::vector<binary> package2(bin.begin(), bin.begin() + 5);
                targetUnit = GetUnit(package2); 
                delete targetStructure;
                bin.erase(bin.begin(), bin.begin() + 5);
            }else{
                std::vector<binary> package2(bin.begin(), bin.begin() + 4);
                targetStructure = GetStructure(package2); 
                delete targetUnit;
                bin.erase(bin.begin(), bin.begin() + 4);
            }
            AttackAction attackAction(targetUnit, targetStructure);
            return attackAction;
        }

        case 2:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            //bin.erase(bin.begin(), bin.begin() + 5);
            //std::vector<binary> package2(bin.begin(), bin.begin() + 4);
            StructureType structType = static_cast<StructureType>(std::get<int>(bin[5]));
            Vec2 buildCoord(std::get<int>(bin[6]), std::get<int>(bin[7]));
            bin.erase(bin.begin(), bin.begin() + 8);

            Unit* newUnit = GetUnit(package);
            //Structure* newStruct = GetStructure(package2);
            BuildAction buildAction(newUnit, structType, buildCoord);
            return buildAction;
        }

        case 3:{
            std::vector<binary> package(bin.begin(), bin.begin() + 5);
            Unit* newUnit = GetUnit(package);
            int x = std::get<int>(bin[5]);
            int y = std::get<int>(bin[6]);
            Vec2 dest(x, y);

            bin.erase(bin.begin(), bin.begin() + 7);
            FarmGoldAction farmAction(newUnit, dest);
            return farmAction;
        }

        case 4:{
            UnitType unitType = static_cast<UnitType>(std::get<int>(bin[0]));
            std::vector<binary> package(bin.begin() + 1, bin.begin() + 4);
            Structure* binStru = GetStructure(package);
            
            bin.erase(bin.begin(), bin.begin() + 5);
            RecruitAction recruitAction(unitType, binStru);
            return recruitAction;
        }
    }
}

Unit* GetUnit(std::vector<binary>& bin){
    UnitType type = static_cast<UnitType>(std::get<int>(bin[0]));
    float health = std::get<float>(bin[1]);
    float mana = std::get<float>(bin[2]);
    int x = std::get<int>(bin[3]);
    int y = std::get<int>(bin[4]);
    Unit* un;
    switch(type){
        case FOOTMAN:
            un = new Footman(Vec2(x, y), health, mana);
            break;
                                                                       
        case PEASANT:
            un = new Peasant(Vec2(x, y), health, mana);
            break;
    }
    return un;
}

Structure* GetStructure(std::vector<binary>& bin){
    StructureType type = static_cast<StructureType>(std::get<int>(bin[0]));
    float health = std::get<float>(bin[1]);
    int x = std::get<int>(bin[2]);
    int y = std::get<int>(bin[3]);
    Structure* str;
    switch(type){
        case HALL:
            str = new TownHall(Vec2(x, y), health);
            break;

        case BARRACK:
            str = new Barrack(Vec2(x, y), health);
            break;


        case FARM:
            str = new Farm(Vec2(x, y), health);
            break;
    }

    return str;
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
FarmGoldAction::FarmGoldAction(Unit *p, Vec2 v) : destCoord(v), peasant(p){type = FARMGOLD;}    
FarmGoldAction::FarmGoldAction(Unit *p, Vec2 v, TownHall *h) : peasant(p), destCoord(v), hall(h){type = FARMGOLD;}
RecruitAction::RecruitAction(UnitType typeOfUnit, Structure* s) : unitType(typeOfUnit), stru(s){type = RECRUIT;}
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
bool RecruitAction::operator==(const RecruitAction& a) const {
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
ActionType RecruitAction::GetType(){
    return type;
}

