//
// Created by Klavio Tarka on 13.12.23.
//
#include "Player.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "../Race/Structure/Barrack.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Hero/Archmage.h"
#include "../Race/Unit/Hero/BloodMage.h"
#include "../Race/Unit/Peasant.h"
#include "../State/Terrain.h"
#include "../Race/Structure/Structure.h"
#include "../ReinforcementLearning/Reward.h"

Player::Player(Map &m, Side en) : map(m), side(en) {
    Initialize();
    food.y = 10;
}
Player::Player(const Player &other) : map(other.map) {
    gold = other.gold;
    food = other.food;

   // // Deep copy structures
   // for (const auto &structure : other.structures)
   // {
   //     structures.push_back(structure->Clone());
   // }
   // // Deep copy units
   // for (const auto &unit : other.units)
   // {
   //     units.push_back(unit->Clone());
   // }

   // side = other.side;
}
Player::~Player(){

}
float Player::TakeAction(actionT& act) {
    float reward = GetRewardFromAction(act);
    if (std::holds_alternative<MoveAction>(act)){
        MoveAction& action = std::get<MoveAction>(act);
        Move(action);
    }
    else if (std::holds_alternative<AttackAction>(act)){
        AttackAction& action = std::get<AttackAction>(act);
        Attack(action);
    }
    else if (std::holds_alternative<BuildAction>(act)){
        BuildAction& action = std::get<BuildAction>(act);
        Build(action);
    }
    else if (std::holds_alternative<FarmGoldAction>(act)){
        FarmGoldAction& action = std::get<FarmGoldAction>(act);
        FarmGold(action);
    }
    else if (std::holds_alternative<RecruitAction>(act)){
        RecruitAction& action = std::get<RecruitAction>(act);
        Recruit(action);
    }
    return reward;
}
void Player::Move(MoveAction& action) {
    action.unit->InsertAction(action);
}
void Player::Attack(AttackAction& action) {
    action.unit->InsertAction(action);
}
void Player::Build(BuildAction& action){
    Terrain& ter = map.GetTerrainAtCoordinate(action.coordinate);
    if (ter.structureOnTerrain == nullptr && ter.resourceLeft == 0){
        std::unique_ptr<Structure> s = ChooseToBuild(action.struType);
        if (gold - s->goldCost >= 0){
            s->health = 1;
            s->coordinate = action.coordinate;
            structures.emplace_back(std::move(s));
            action.peasant->InsertAction(action);
            map.AddOwnership(action.stru);
        }
    }
}
void Player::FarmGold(FarmGoldAction& action) {
    action.peasant->InsertAction(action);
}
void Player::Recruit(RecruitAction& action) {
    if (action.stru != nullptr && action.stru->is == BARRACK){
        std::unique_ptr<Unit> un = ChooseToRecruit(action.unitType);
        un->coordinate = action.stru->coordinate;
        if (gold >= un->goldCost)
            units.emplace_back(std::move(un));
        map.AddOwnership(un.get());
    }
}
void Player::Initialize() {
    structures.push_back(std::make_unique<TownHall>(Vec2(0, 0)));
    for (int i = 0; i < 5; i++)
    {
        units.push_back(std::make_unique<Peasant>());
        units[i]->coordinate = structures[0]->coordinate;
    }
}
void Player::SetInitialCoordinates(Vec2 v) {
    for (auto &structure : structures)
    {
        structure->coordinate = v;
        map.AddOwnership(structure.get());
    }
    for (auto &unit : units)
    {
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
            double temp = std::sqrt(std::pow(difference.x, 2) + std::pow(difference.y, 2));
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
    food.y = 0;

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

std::unique_ptr<Structure> Player::ChooseToBuild(StructureType structType) {
    std::unique_ptr<Structure> str;
    switch (structType)
    {
    case HALL:
        str = std::make_unique<TownHall>(Vec2(0, 0));
        break;

    case BARRACK:
        str = std::make_unique<Barrack>(Vec2(0, 0));
        break;

    case FARM:
        str = std::make_unique<Farm>(Vec2(0, 0));
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
        unt = std::make_unique<Archmage>();
        break;

    default:
        break;
    }
    return unt;
}
