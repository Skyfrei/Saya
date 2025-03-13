#pragma once

#include <chrono>
#include <vector>

#include "../../Spells/Spell.h"
#include "../Unit.h"

enum Attribute { STRENGTH, AGILITY, INTELLIGENCE };

class Hero : public Unit {
 public:
  Hero() {}

 public:
  void CastSpell(int s) {
    switch (s) {
      case 0:
        spells[0]->ProcEffect();
        break;

      case 1:
        spells[1]->ProcEffect();
        break;

      case 2:
        spells[2]->ProcEffect();
        break;
    }
  }
  void Attack(Living &un) {
    if (WithinDistance(un.coordinate)) {
      if (CanAttack()) un.health -= attack;
    } else
      Move(un.coordinate);
  }

  void RegenMana() {
    if (mana + manaRegen >= maxMana) return;
    mana += manaRegen;
  }

  void CheckExperience(float bonus) {
    if (experience + bonus >= maxExperience) LevelUp();
  }

  void LevelUp() {
    if (level >= 10) return;
    strength += 2;
    health += 50;
    maxHealth += 50;
    hpRegen += 0.1;
    agility += 1;
    intelligence += 3;
    mana += 45;
    maxMana += 45;
    manaRegen += 0.15;
    attack += 3;
    attackCooldown = duration<float>(attackCooldown.count() - 0.02f);
    experience = 0.0f;
    maxExperience += 150;
  }

 public:
  bool hiredOnce = false;
  float strength;
  float agility;
  float intelligence;
  int8_t level = 1;
  float experience = 0.0f;
  float maxExperience = 100.0f;

  std::vector<Spell *> spells;
  Attribute primaryAttribute;
};
