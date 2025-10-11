#include "Hero.h"
#include "../../../Tools/Macro.h"

Hero::Hero(){}
void Hero::CastSpell(int s) {
    switch (s)
    {
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
void Hero::Attack(Living &un) {
    if (WithinDistance(un.coordinate))
    {
        if (CanAttack())
            un.health -= attack;
    }
    else
        Move(un.coordinate);
}

void Hero::RegenMana() {
    if (mana + manaRegen >= maxMana)
        return;
    mana += manaRegen;
}

void Hero::CheckExperience(float bonus) {
    if (experience + bonus >= maxExperience)
        LevelUp();
}

    
void Hero::LevelUp() {
    if (level >= 10)
        return;
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
    //attackCooldown = duration<float>(attackCooldown.count() - 0.02f);
    experience = 0.0f;
    maxExperience += 150;
}

