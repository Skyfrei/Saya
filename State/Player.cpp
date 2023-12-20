//
// Created by Klavio Tarka on 13.12.23.
//
#include "Player.h"
namespace Warcraft::State
{
    Player::Player()
    {
        food.y = 10;
        gold = 2000;
    }
    void Player::SetInitialCoordinates(Vec2 v) {
        for (const auto &structure: structures) {
            structure->coordinate = v;
        }
        for (const auto &unit: units) {
            unit->coordinate = structures[0]->coordinate;
        }
    }

    bool Player::HasStructure(StructureType structType) {
        for (const auto &structure: structures) {
            if (structure->is == structType) {
                return true;
            }
        }
        return false;
    }

    bool Player::HasUnit(UnitType unitType) {
        for (const auto &unit: units) {
            if (unit->is == unitType) {
                return true;
            }
        }
        return false;
    }

//    std::vector<Living>& Player::Select()
//    {
//
//    }

    Structure& Player::FindClosestStructure(Unit &unit, StructureType type) {
        double min = 100;
        int8_t index = 0;

        for (int i = 0; i < structures.size(); i++) {
            if (structures[i]->is == type) {
                Vec2 difference = unit.FindDifference(structures[i]->coordinate);
                double temp = std::sqrt(std::pow(difference.x, 2) + std::pow(difference.y, 2));
                if (temp <= min) {
                    min = temp;
                    index = i;
                }
            }
        }
        return *structures[index];
    }

    void Player::Initialize(Map &m) {
        structures.push_back(std::make_unique<TownHall>());
        for (int i = 0; i < 5; i++) {
            units.push_back(std::make_unique<Peasant>());
            units[0]->coordinate = structures[0]->coordinate;
        }
        structures.push_back(std::make_unique<Barrack>());
        map = m;
        ValidateFood();
    }

    void Player::ValidateFood() {
        food.x = 0;
        food.y = 0;

        for (const auto &unit: units) {
            food.x += unit->foodCost;
        }
        for (const auto &structure: structures) {
            if (structure->is == FARM)
                food.y += 5;
        }
    }

    void Player::RecruitSoldier(UnitType unitType) {
        if (HasStructure(BARRACK)) {
            if (unitType == ARCHMAGE) {
                bool temp = HasUnit(BLOODMAGE);
                if (temp) {
                    return;
                }
            } else if (unitType == BLOODMAGE) {
                bool temp = HasUnit(ARCHMAGE);
                if (temp) {
                    return;
                }
            }
        }

        for (const auto &structure: structures) {
            if (structure->is == BARRACK) {
                dynamic_cast<Barrack &>(*structure).CreateUnit(units, gold, unitType);
            }
        }
    }

    void Player::UpdateGold(int g) {
        gold += g;
    }

    void Player::ChooseToBuild(StructureType structType, Vec2 terrCoord) {
        //Peasant a = FindClosestLiving(terrCoord);
        for (const auto &unit: units) {
            if (unit->is == PEASANT) {
                dynamic_cast<Peasant &>(*unit).Build(structures, gold, structType, map.objects[terrCoord.x][terrCoord.y]);
            }
        }
    }
}