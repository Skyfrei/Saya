#pragma once
#include <string>

enum SpellType { AURA, MANUAL, PASSIVE };

class Spell {
 public:
  Spell() {}

 public:
  virtual void GetDescription() = 0;

  virtual void ProcEffect() = 0;

 public:
  float cooldown;
  float manaCost;
  int range;
  float areaOfEffect;
  float duration;
  SpellType type;
  std::string name;
};
