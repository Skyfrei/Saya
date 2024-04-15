//
// Created by Klavio Tarka on 13.12.23.
//
#include "Peasant.h"

#include <memory>

#include "Race/Unit/Unit.h"
void Peasant::Build(Structure *str) {
  if (WithinDistance(str->coordinate)) {
    if (CanAttack()) {
      if (str->health + attack <= maxHealth) {
        str->health += attack;
      }
    }
  } else
    Move(str->coordinate);
}

void Peasant::FarmGold(Terrain &terr, TownHall &hall, int &g) {
  if (goldInventory > 0 && WithinDistance(hall.coordinate)) {
    goldInventory = 0;
  }
  if (goldInventory >= maxGoldInventory) {
    Move(hall.coordinate);
    return;
  }
  if (WithinDistance(terr.coord)) {
    if (terr.type == GOLD) {
      if (CanAttack()) {
        goldInventory++;
        terr.resourceLeft--;
        g++;
      }
    }
  } else {
    Move(terr.coord);
  }
}
