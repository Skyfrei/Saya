#ifndef UNIT_H
#define UNIT_H

#include <chrono>
#include <cstddef>
#include <string>
#include <variant>
#include <vector>

#include "../../Living.h"
#include "../../State/Terrain.h"
#include "../../Tools/Binary.h"
#include "../Spells/Spell.h"
#include "../Structure/Structure.h"
#include "Action.h"

using namespace std::chrono;

class TownHall;
class Farm;
class Barrack;
class Unit;

// struct Path {
//   Path(int d, Vec2 l) : distance(d), comesFrom(l) {}
//   Path() {}
//   int distance;
//   Vec2 comesFrom;
// };

class Unit : public Living
{
  public:
    Unit();
    virtual ~Unit() = default;
    virtual Unit *Clone() const = 0;

    void Move(Vec2 terr);
    void Attack(Living &un);
    void RegenHealth();
    bool WithinDistance(Vec2 terr);
    bool CanAttack();
    Vec2 FindDifference(Vec2 terr);
    actionT TakeAction();
    void InsertAction(actionT);
    void ResetActions();
    bool HasCommand();
    int GetActionQueueSize();
    std::string Serialize();
    // Unit* Deserialize(std::string& info);
    std::vector<binary> SerializeBinary();
    // Unit* DeserializeBinary(std::vector<binary>& bin);
    bool operator==(const Unit &other) const;

  private:
    bool IsMovable();
    void MoveCoordinates(Vec2 terr);
    void ChangeCoordinate(MoveType dir);

  public:
    bool isInvisible = false;
    float attack{};
    float mana = 100;
    int maxMana = 100;
    float attackRange{};
    int movementSpeed = 1;
    duration<float, std::milli> attackCooldown = std::chrono::milliseconds(200);
    duration<float, std::milli> hpCooldown = std::chrono::milliseconds(1000);
    duration<float, std::milli> moveCooldown = std::chrono::milliseconds(100);
    float manaRegen = 0.25f;
    float hpRegen = 0.25f;
    UnitType is;
    std::vector<actionT> actionQueue;

  private:
    high_resolution_clock::time_point hpTime = high_resolution_clock::now();
    high_resolution_clock::time_point attackTime = high_resolution_clock::now();
    high_resolution_clock::time_point moveTime = high_resolution_clock::now();
};

#endif
