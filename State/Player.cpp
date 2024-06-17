//
// Created by Klavio Tarka on 13.12.23.
//
#include "Player.h"

#include <algorithm>
#include <memory>
#include <cmath>

#include "../Map/Terrain.h"
#include "../Race/Structure/Barrack.h"
#include "../Race/Structure/Farm.h"
#include "../Race/Structure/TownHall.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Hero/Archmage.h"
#include "../Race/Unit/Hero/BloodMage.h"
#include "../Race/Unit/Peasant.h"

#include "../Race/Structure/Structure.h"
Player::Player(Map &m) : map(m) {
  Initialize();
  food.y = 10;
}
Player::Player(const Player& other) : map(other.map){
    gold = other.gold;
    food = other.food;
    
    // Deep copy structures
    for (const auto &structure : other.structures) {
        structures.push_back(structure->Clone());
    }
    // Deep copy units
    for (const auto &unit : other.units) {
        units.push_back(unit->Clone());
    }
}
void Player::TakeAction(actionT action){
  // if (std::holds_alternative<MoveAction>(action)) {
  //   MoveAction &a = std::get<MoveAction>(action);
  //   Move(a.unit, a.v);
  // } else if (std::holds_alternative<AttackAction>(action)) {
  //   AttackAction &a = std::get<AttackAction>(action);
  //   Attack(a.unit, a.l);
  // } else if (std::holds_alternative<BuildAction>(action)) {
  //   BuildAction &a = std::get<BuildAction>(action);
  //   Build(a.p, a.type, a.v);
  // } else if (std::holds_alternative<FarmGoldAction>(action)) {
  //   FarmGoldAction &a = std::get<FarmGoldAction>(action);
  //   FarmGold(a.p, a.v, a.hall);
  // }
}
void Player::Move(Unit *u, Vec2 v) {
  actionT t = MoveAction(v);
  u->InsertAction(t);
}
void Player::Attack(Unit *u, Living *l) {
  AttackAction b(l);
  actionT t = b;
  u->InsertAction(t);
}
void Player::Build(Peasant *p, StructureType type, Vec2 v) {
  Terrain &ter = map.GetTerrainAtCoordinate(v);
  if (ter.structureOnTerrain == nullptr && ter.resourceLeft == 0) {
    std::unique_ptr<Structure> &s =
        structures.emplace_back(ChooseToBuild(type));
    if (gold - s->goldCost >= 0) {
      s->health = 1;
      s->coordinate = v;
      BuildAction b(s.get());
      p->InsertAction(b);
      map.AddOwnership(b.stru);
    }
  }
}
void Player::FarmGold(Peasant *p, Vec2 v, TownHall *hall) {
  Terrain &ter = map.GetTerrainAtCoordinate(v);

  if (ter.resourceLeft <= 0) {
    MoveAction action(ter.coord);
    p->InsertAction(action);
    return;
  }
  FarmGoldAction action(v, &ter, hall);
  p->InsertAction(action);
}
void Player::RecruitSoldier(UnitType unitType, Structure *stru) {
  if (auto s = dynamic_cast<Barrack *>(stru)) {
    Terrain &ter = map.GetTerrainAtCoordinate(stru->coordinate);
    if (unitType == ARCHMAGE) {
      if (HasUnit(BLOODMAGE)) return;
    } else if (unitType == BLOODMAGE) {
      if (HasUnit(ARCHMAGE)) return;
    }
    std::unique_ptr<Unit> &un = units.emplace_back(ChooseToRecruit(unitType));
    un->coordinate = stru->coordinate;
    map.AddOwnership(un.get());
  }
}
void Player::Initialize() {
  structures.push_back(std::make_unique<TownHall>());
  for (int i = 0; i < 5; i++) {
    units.push_back(std::make_unique<Peasant>());
    units[i]->coordinate = structures[0]->coordinate;
  }
}
void Player::SetInitialCoordinates(Vec2 v) {
  for (auto &structure : structures) {
    structure->coordinate = v;
    map.AddOwnership(structure.get());
  }
  for (auto &unit : units) {
    unit->coordinate = v;
    map.AddOwnership(unit.get());
  }
}
std::vector<std::unique_ptr<Unit>> Player::SelectUnits() {
  std::vector<std::unique_ptr<Unit>> result;
  return result;
}

bool Player::HasStructure(StructureType structType) {
  for (const auto &structure : structures) {
    if (structure->is == structType) {
      return true;
    }
  }
  return false;
}

bool Player::HasUnit(UnitType unitType) {
  for (const auto &unit : units) {
    if (unit->is == unitType) {
      return true;
    }
  }
  return false;
}

// std::vector<Living> &Player::Select() {}

Structure &Player::FindClosestStructure(Unit &unit, StructureType type) {
  double min = 100;
  int8_t index = 0;

  for (int i = 0; i < structures.size(); i++) {
    if (structures[i]->is == type) {
      Vec2 difference = unit.FindDifference(structures[i]->coordinate);
      double temp =
          std::sqrt(std::pow(difference.x, 2) + std::pow(difference.y, 2));
      if (temp <= min) {
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

  for (const auto &unit : units) {
    food.x += unit->foodCost;
  }
  for (const auto &structure : structures) {
    if (structure->is == FARM) food.y += 5;
  }
}

void Player::UpdateGold(int g) { gold += g; }

std::unique_ptr<Structure> Player::ChooseToBuild(StructureType structType) {
  std::unique_ptr<Structure> str;
  switch (structType) {
    case HALL:
      str = std::make_unique<TownHall>();
      break;

    case BARRACK:
      str = std::make_unique<Barrack>();
      break;

    case FARM:
      str = std::make_unique<Farm>();
      break;

    default:
      break;
  }
  return str;
}

std::unique_ptr<Unit> Player::ChooseToRecruit(UnitType unitType) {
  std::unique_ptr<Unit> unt;
  switch (unitType) {
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
