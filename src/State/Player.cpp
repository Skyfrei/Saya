//
// Created by Klavio Tarka on 13.12.23.
//
#include "Player.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <set>

#include "../Race/Structure/Barrack.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/Structure.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Hero/ArchMage.h"
#include "../Race/Unit/Hero/BloodMage.h"
#include "../Race/Unit/Peasant.h"
#include "../ReinforcementLearning/Reward.h"
#include "../State/Terrain.h"
#include "DeathManager.h"

Player::Player(Map &m, Side en) : map(m), side(en) {
    Initialize();
    food.y = 10;
    gold = 300;
}
void Player::Reset(Side en){
    for (auto &structure : structures)
        map.RemoveOwnership(structure.get(), Vec2(structure->coordinate.x, structure->coordinate.y));
    for (auto &unit : units)
        map.RemoveOwnership(unit.get(), Vec2(unit->coordinate.x, unit->coordinate.y));

    units.clear();
    structures.clear();
    Initialize();
    food.y = 10;
    gold = 300;

    if (en == PLAYER)
        SetInitialCoordinates(Vec2(2, 2));
    else if (en == ENEMY)
        SetInitialCoordinates(Vec2(MAP_SIZE - 4, MAP_SIZE - 4));
}
Player::Player(const Player &other) : map(other.map) {
    gold = other.gold;
    food = other.food;

     // Deep copy structures
     for (const auto &structure : other.structures)
     {
         structures.emplace_back(structure->Clone());
     }
     // Deep copy units
     for (const auto &unit : other.units)
     {
         units.emplace_back(unit->Clone());
     }

     side = other.side;
}
Player::~Player() {
    units.clear();
    structures.clear();
}
float Player::TakeAction(actionT &act) {
    float reward = 0.0f; 
    std::vector<Living*> deleted_living = CheckUnitActions(reward);

    auto ptr_deleted = [&deleted_living](Living* l){
        for (auto& p : deleted_living){
            if (l == p)
                return true;
        }
        return false;
    };

    if (std::holds_alternative<MoveAction>(act))
    {
        MoveAction &action = std::get<MoveAction>(act);
        if (ptr_deleted(action.unit))
            return reward;
        Move(action);
    }
    else if (std::holds_alternative<AttackAction>(act))
    {
        AttackAction &action = std::get<AttackAction>(act);
        if (ptr_deleted(action.unit))
            return reward;
        Attack(action);
    }
    else if (std::holds_alternative<BuildAction>(act))
    {
        BuildAction &action = std::get<BuildAction>(act);
        if (ptr_deleted(action.peasant))
            return reward;

        if (gold < action.stru->goldCost){
            reward -= 0.01f;
            return reward;
        }

        reward += action.stru->goldCost / 10.0f;
        Build(action);
    }
    else if (std::holds_alternative<FarmGoldAction>(act))
    {
        FarmGoldAction &action = std::get<FarmGoldAction>(act);
        if (ptr_deleted(action.peasant))
            return reward;
        FarmGold(action);
    }
    else if (std::holds_alternative<RecruitAction>(act))
    {
        RecruitAction &action = std::get<RecruitAction>(act);
        if (ptr_deleted(action.stru))
            return reward;
        Recruit(action); // recruit first then get rewaard to check if action finished
        reward += GetRewardFromAction(action);
    }
    else {
        EmptyAction act;
        reward += GetRewardFromAction(act); 
    }
    return reward;
}
std::vector<Living*> Player::CheckUnitActions(float& reward){
    std::vector<Living*> deleted_ptr;

    std::erase_if(units, [this, &reward, &deleted_ptr](const auto& un) {
        if (un->IsDead()) {
            reward -= 3.0f * (un->goldCost / 10.0);
            food.x -= un->foodCost; 
            deleted_ptr.push_back(un.get());
            DeathManager::GetSingleton().RemoveFromAttackAction(un.get(), side);
            this->map.RemoveOwnership(un.get(), un->coordinate);
            return true;
        }
        return false;
    });

    std::erase_if(structures, [this, &reward, &deleted_ptr](const auto& stru) {
        if (stru->IsDead()) {
            reward -= 3.0f * (stru->goldCost / 10.0);
            deleted_ptr.push_back(stru.get());
            DeathManager::GetSingleton().RemoveFromAttackAction(stru.get(), side);

            for (auto& un : units) {
                for (auto& variantAct : un->actionQueue) {
                    if (std::holds_alternative<BuildAction>(variantAct)) {
                        auto& bAct = std::get<BuildAction>(variantAct);
                        if (bAct.stru == stru.get()) {
                            bAct.stru = nullptr; 
                        }
                    }
                    else if (std::holds_alternative<FarmGoldAction>(variantAct)) {
                        auto& bAct = std::get<FarmGoldAction>(variantAct);
                        if (bAct.hall == stru.get()) {
                            bAct.hall = nullptr; 
                        }
                    }
                }
            }

            if (stru->is == FARM && stru->isBuilt) {
                food.y -= Farm::foodReceived;
            }
            
            this->map.RemoveOwnership(stru.get(), stru->coordinate);
            return true;
        }
        return false;
    });
    
    int i = 0;
    bool one_is_being_attacked = false;
    std::set<actionT*> finished_actions;

    auto is_finished = [&](actionT& act) {
        return !finished_actions.insert(&act).second;
    };

    for (auto& un : units){
        un->TakeAction(map);
        if (un->actionQueue.empty()) continue;

        auto& act = un->actionQueue[0];
        std::visit([&](auto& action) {
            if constexpr (requires { action.finished; }) {
                if (std::holds_alternative<MoveAction>(act))
                {
                    MoveAction &action = std::get<MoveAction>(act);
                    reward += GetRewardFromAction(action);
                }
                else if (std::holds_alternative<AttackAction>(act))
                {
                    AttackAction &action = std::get<AttackAction>(act);
                    if (action.object != nullptr && un->WithinDistance(action.object->coordinate)){
                        Structure* hall = dynamic_cast<Structure*>(action.object);
                        if (hall){
                            if (!one_is_being_attacked){
                                one_is_being_attacked = true;
                                un->health -= 20;
                            }
                        }
                    }
                    reward += GetRewardFromAction(action);
                }
                else if (std::holds_alternative<BuildAction>(act))
                {
                    BuildAction &action = std::get<BuildAction>(act);
                    if (action.stru){
                        if (action.struType == FARM && action.finished && action.stru->isBuilt){
                            if (!is_finished(act))
                                food.y += Farm::foodReceived;
                        }
                    }
                    reward += GetRewardFromAction(action);
                }
                else if (std::holds_alternative<FarmGoldAction>(act))
                {
                    FarmGoldAction &action = std::get<FarmGoldAction>(act);
                    action.terr = &map.GetTerrainAtCoordinate(action.destCoord);
                    gold += action.gold;
                    if (gold <= 600){
                        reward += GetRewardFromAction(action, gold);
                    }
                }
                if (action.finished)
                    un->actionQueue.erase(un->actionQueue.begin());
            }
        }, act);
    }
    return deleted_ptr;
}
void Player::Move(MoveAction &action) {
    action.unit->InsertAction(action);
}
void Player::Attack(AttackAction &action) {
    action.unit->InsertAction(action);
}
void Player::Build(BuildAction &action) {
    Terrain &ter = map.GetTerrainAtCoordinate(action.coordinate);
    if (ter.structureOnTerrain == nullptr && ter.resourceLeft == 0)
    {
        std::unique_ptr<Structure> s = ChooseToBuild(action.struType, action.coordinate);
        if (gold - s->goldCost >= 0)
        {
            gold -= s->goldCost;
            s->health = 1;
            s->coordinate = action.coordinate;
            map.AddOwnership(s.get());
            action.stru = s.get();
            structures.emplace_back(std::move(s));
            action.peasant->InsertAction(action);
        }
    }
}

std::string GetActionName(ActionType type) {
    switch (type) {
        case MOVE:     return "MOVE";
        case ATTACK:   return "ATTACK";
        case BUILD:    return "BUILD";
        case FARMGOLD: return "FARMGOLD";
        case RECRUIT:  return "RECRUIT";
        case EMPTY:    return "EMPTY";
        default:       return "UNKNOWN";
    }
}

void Player::FarmGold(FarmGoldAction &action) {
    action.peasant->InsertAction(action);
}
void Player::Recruit(RecruitAction &action) {
    if (!action.stru)
        return;

    bool isHallRecruitingPeasant = (action.stru->is == HALL && action.unitType == PEASANT);
    bool isBarrackRecruitingMilitary = (action.stru->is == BARRACK && action.unitType != PEASANT);

    if (isHallRecruitingPeasant || isBarrackRecruitingMilitary)
    {
        std::unique_ptr<Unit> un = ChooseToRecruit(action.unitType);
        if (gold >= un->goldCost && food.x + un->foodCost <= food.y)
        {
            gold -= un->goldCost;
            food.x += un->foodCost;
            un->coordinate = action.stru->coordinate;
            map.AddOwnership(un.get());
            units.emplace_back(std::move(un));
            action.finished = true; 
        }
    }
}
void Player::Initialize() {
    structures.push_back(std::make_unique<TownHall>(Vec2(0, 0)));
    for (int i = 0; i < 5; i++)
    {
        std::unique_ptr<Unit> un = std::make_unique<Peasant>();
        un->coordinate = structures[0]->coordinate;
        map.AddOwnership(un.get());
        units.emplace_back(std::move(un));
    }
    ValidateFood();
}
void Player::SetInitialCoordinates(Vec2 v) {
    for (auto &structure : structures)
    {
        map.RemoveOwnership(structure.get(), Vec2(structure->coordinate.x, structure->coordinate.y));
        structure->coordinate = v;
        map.AddOwnership(structure.get());
    }
    for (auto &unit : units)
    {
        map.RemoveOwnership(unit.get(), Vec2(unit->coordinate.x, unit->coordinate.y));
        unit->coordinate = v;
        map.AddOwnership(unit.get());
    }
}
std::vector<std::unique_ptr<Unit>> Player::SelectUnits() {
    std::vector<std::unique_ptr<Unit>> result;
    return result;
}

bool Player::HasStructure(StructureType structType) {
    for (const auto &structure : structures)
    {
        if (structure->is == structType)
        {
            return true;
        }
    }
    return false;
}

bool Player::HasUnit(UnitType unitType) {
    for (const auto &unit : units)
    {
        if (unit->is == unitType)
        {
            return true;
        }
    }
    return false;
}

// std::vect<Living> &Player::Select() {}

Structure &Player::FindClosestStructure(Unit &unit, StructureType type) {
    double min = 100;
    int8_t index = 0;

    for (int i = 0; i < structures.size(); i++)
    {
        if (structures[i]->is == type)
        {
            Vec2 difference = unit.FindDifference(structures[i]->coordinate);
            double temp =
                std::sqrt(std::pow(difference.x, 2) + std::pow(difference.y, 2));
            if (temp <= min)
            {
                min = temp;
                index = i;
            }
        }
    }
    return *structures[index];
}

void Player::ValidateFood() {
    food.x = 0;
    food.y = 10;

    for (const auto &unit : units)
    {
        food.x += unit->foodCost;
    }
    for (const auto &structure : structures)
    {
        if (structure->is == FARM)
            food.y += 5;
    }
}

void Player::UpdateGold(int g) {
    gold += g;
}

std::unique_ptr<Structure> Player::ChooseToBuild(StructureType structType,
                                                 Vec2 &coord) {
    std::unique_ptr<Structure> str;
    switch (structType)
    {
    case HALL:
        str = std::make_unique<TownHall>(coord);
        break;

    case BARRACK:
        str = std::make_unique<Barrack>(coord);
        break;

    case FARM:
        str = std::make_unique<Farm>(coord);
        break;

    default:
        break;
    }
    return str;
}

std::unique_ptr<Unit> Player::ChooseToRecruit(UnitType unitType) {
    std::unique_ptr<Unit> unt;
    switch (unitType)
    {
    case PEASANT:
        unt = std::make_unique<Peasant>();
        break;
    case FOOTMAN:
        unt = std::make_unique<Footman>();
        break;

    case BLOODMAGE:
        unt = std::make_unique<BloodMage>();
        break;

    case ARCHMAGE:
        unt = std::make_unique<ArchMage>();
        break;

    default:
        break;
    }
    return unt;
}
