#pragma once
#include "../Unit/Unit.h"
#include "Spell.h"

//
// Level	Effect	Hero Level Req
// 1	Adds 0.75 Mana per sec.	1
// 2	Adds 1.25 Mana per sec.	3
// 3	Adds 2 Mana per sec.	5

// Calls down waves of freezing ice shards that damage units in a target area.

class BrillanceAura : public Spell {
 public:
  BrillanceAura() {
    name = "BrillanceAura";
    type = PASSIVE;
  }

 public:
  void GetDescription() override {}
  void ProcEffect() override {}
  void ManaRegen(Unit &unit) { unit.mana += manaRegen; }

 private:
  float manaRegen = 0.75;
};
