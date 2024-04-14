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

void Peasant::FarmGold(Terrain &terr, TownHall &hall) {
  if (WithinDistance(terr.coord)) {
    if (terr.type == GOLD) {
      if (goldInventory >= 20) TransferGold(hall);
      if (CanAttack()) goldInventory++;
    }
  } else {
    Move(terr.coord);
  }
}

void Peasant::TransferGold(TownHall &hall) {
  Move(hall.coordinate);

  if (WithinDistance(hall.coordinate)) goldInventory = 0;
}
