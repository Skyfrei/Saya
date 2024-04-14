//
// Created by Klavio Tarka on 13.12.23.
//
#include "Player.h"

#include <algorithm>
#include <memory>

#include "Map/Terrain.h"
#include "Race/Structure/Barrack.h"
#include "Race/Structure/Farm.h"
#include "Race/Structure/Structure.h"
#include "Race/Structure/TownHall.h"
#include "Race/Unit/Unit.h"
Player::Player(Map &m) : map(m) {
  Initialize();
  food.y = 10;
  gold = 2000;
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
    std::unique_ptr<Structure> s = ChooseToBuild(type);
    structures.emplace_back(std::move(s));
    if (gold - s->goldCost >= 0) {
      s->health = 1;
      s->coordinate = v;
      BuildAction b(s.get(), v);
      actionT t = b;
      p->InsertAction(t);
      map.AddOwnership(b.stru);
    }
  }
}
void Player::Initialize() {
  structures.push_back(std::make_unique<TownHall>());
  for (int i = 0; i < 5; i++) {
    units.push_back(std::make_unique<Peasant>());
    units[i]->coordinate = structures[0]->coordinate;
  }

  ValidateFood();
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

  for (const auto &structure : structures) {
    if (structure->is == BARRACK) {
      dynamic_cast<Barrack &>(*structure).CreateUnit(units, gold, unitType);
    }
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
  // Peasant a = FindClosestLiving(terrCoord);
}
