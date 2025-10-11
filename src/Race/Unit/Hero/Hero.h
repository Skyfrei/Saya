#ifndef HERO_H
#define HERO_H
#include <vector>

#include "../../Spells/Spell.h"
#include "../Unit.h"


enum Attribute
{
    STRENGTH,
    AGILITY,
    INTELLIGENCE
};

class Hero : public Unit
{
  public:
    Hero();

  public:
    void CastSpell(int s);
    void Attack(Living &un); 
    void RegenMana();
    void CheckExperience(float bonus);
    void LevelUp();

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
#endif
