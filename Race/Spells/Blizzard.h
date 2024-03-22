#pragma once
#include "Spell.h"

// Level	Duration	Effect	Hero Level Req
// 1	6 sec.	6 waves at 30 damage each, 150 max damage	1
// 2	8 sec.	8 waves at 40 damage each, 200 max damage	3
// 3	10 sec.	10 waves at 50 damage each, 250 max damage	5

// Calls down waves of freezing ice shards that damage units in a target area.

class Blizzard : public Spell {
 public:
  Blizzard() {
    cooldown = 6.0f;
    manaCost = 75;
    range = 80;
    areaOfEffect = 20;
    duration = 6.0f;
    name = "Blizzard";
    type = MANUAL;
  }

 public:
  void GetDescription() override {}
  void ProcEffect() override {}
};
