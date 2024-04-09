//
// Created by Klavio Tarka on 13.12.23.
//
#include "Peasant.h"

#include "Race/Unit/Unit.h"
void Peasant::Build(std::vector<std::unique_ptr<Structure>> &structures,
                    int &playerGold, StructureType type, Terrain &terr) {
  std::unique_ptr<Structure> struc;
  if (WithinDistance(terr.coord)) {
    if (terr.type == GROUND) {
      switch (type) {
        case BARRACK:
          struc = std::make_unique<Barrack>();
          break;

        case FARM:
          struc = std::make_unique<Farm>();
          break;

        case HALL:
          struc = std::make_unique<TownHall>();
          break;

        default:
          break;
      }
      if (HasEnoughGold(playerGold, struc->goldCost)) {
        playerGold -= struc->goldCost;
        // buildTask.emplace_front(*struc);
        structures.emplace_back(std::move(struc));
        terr.onTerrainLiving.push_back(struc.get());
        // terr.object = stru;
      }
    }
  } else {
    Move(terr.coord);
  }
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
