//
// Created by Klavio Tarka on 13.12.23.
//
#include "Peasant.h"

#include <memory>

#include "Race/Unit/Unit.h"
std::unique_ptr<Structure> Peasant::Build(StructureType type, Terrain &terr) {
  std::unique_ptr<Structure> struc;
  if (WithinDistance(terr.coord)) {
    if (terr.type == GROUND) {
      switch (type) {
        case BARRACK:
          struc = std::make_unique<Barrack>();
          return struc;
          break;

        case FARM:
          struc = std::make_unique<Farm>();
          return struc;
          break;

        case HALL:
          struc = std::make_unique<TownHall>();
          return struc;
          break;

        default:
          break;
      }
    }
  } else {
    Move(terr.coord);
  }
  return struc;
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
