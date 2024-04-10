//
// Created by Klavio Tarka on 14.12.23.
//
#include "Living.h"

bool Living::HasEnoughGold(int playerGold, int cost) {
  return playerGold >= cost;
}

bool Living::Die() { return health <= 0; }
