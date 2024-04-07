//
// Created by Klavio Tarka on 14.12.23.
//
#include "Unit.h"

#include <__algorithm/remove.h>
#include <__fwd/get.h>

#include "State/Manager.h"
#include "Tools/Vec2.h"

Unit::Unit() {}
void Unit::InsertAction(actionT v) {
  if (std::find(actionQueue.begin(), actionQueue.end(), v) ==
      actionQueue.end()) {
    actionQueue.push_back(v);
  }
}
bool Unit::HasCommand() {
  if (actionQueue.size() > 0) return true;
  return false;
}

void Unit::TakeAction() {
  if (!HasCommand()) return;

  if (std::holds_alternative<AttackAction>(actionQueue[0])) {
    AttackAction action = std::get<AttackAction>(actionQueue[0]);
    Attack(*action.object);
  }

  else if (std::holds_alternative<Vec2>(actionQueue[0])) {
    Vec2 targetDest = std::get<Vec2>(actionQueue[0]);
    if (coordinate.x == targetDest.x && coordinate.y == targetDest.y) {
      actionQueue.erase(actionQueue.begin());
      return;
    }
    Move(targetDest);

  } else if (std::holds_alternative<BuildAction>(actionQueue[0])) {
    std::cout << "Implement build here";
  }
}
int Unit::GetActionQueueSize() { return actionQueue.size(); }

void Unit::MoveDij(Vec2 dest, Map &map) {
  std::vector<Node *> nextDestination =
      map.GetClosestDestNode(coordinate, dest);
}
void Unit::Move(Vec2 terr) {
  if (!IsMovable()) {
    return;
  }

  Vec2 difference;
  difference.x = coordinate.x - terr.x;
  difference.y = coordinate.y - terr.y;
  if (difference.x > 0 && difference.y > 0)
    ChangeCoordinate(NW);
  else if (difference.x > 0 && difference.y < 0)
    ChangeCoordinate(NE);
  else if (difference.x < 0 && difference.y > 0)
    ChangeCoordinate(SW);
  else if (difference.x < 0 && difference.y < 0)
    ChangeCoordinate(SE);
  else if (difference.x == 0 && difference.y < 0)
    ChangeCoordinate(E);
  else if (difference.x == 0 && difference.y > 0)
    ChangeCoordinate(W);
  else if (difference.x > 0 && difference.y == 0)
    ChangeCoordinate(N);
  else if (difference.x < 0 && difference.y == 0)
    ChangeCoordinate(S);
}

bool Unit::WithinDistance(Vec2 terr) {
  Vec2 difference = FindDifference(terr);

  if (std::abs(difference.x) <= 1 && std::abs(difference.y) <= 1) return true;

  return false;
}

Vec2 Unit::FindDifference(Vec2 terr) {
  Vec2 difference;
  difference.x = coordinate.x - terr.x;
  difference.y = coordinate.y - terr.y;

  return difference;
}

void Unit::Attack(Living &un) {
  if (WithinDistance(un.coordinate)) {
    if (CanAttack()) {
      un.health -= attack;
      std::cout << "AAAA";
    }
  }
  Move(un.coordinate);
}

bool Unit::CanAttack() {
  auto currentCd =
      duration_cast<seconds>(high_resolution_clock::now() - attackTime).count();
  if (currentCd >= attackCooldown) {
    attackTime = high_resolution_clock::now();
    std::cout << "A";
    return true;
  };

  return false;
}

bool Unit::IsMovable() {
  auto currentCd =
      duration_cast<seconds>(high_resolution_clock::now() - moveTime).count();
  if (currentCd >= moveCooldown) {
    moveTime = high_resolution_clock::now();
    return true;
  }
  return false;
}

void Unit::RegenHealth() {
  auto currentCd =
      duration_cast<seconds>(high_resolution_clock::now() - hpTime).count();

  if (currentCd >= hpCooldown) {
    if (health + hpRegen >= maxHealth) return;
    health += hpRegen;
    hpTime = high_resolution_clock::now();
  }
}

void Unit::ChangeCoordinate(MoveType dir) {
  switch (dir) {
    case W:
      coordinate.y -= 1;
      break;
    case NW:
      coordinate.x -= 1;
      coordinate.y -= 1;
      break;
    case N:
      coordinate.x -= 1;
      break;
    case NE:
      coordinate.x -= 1;
      coordinate.y += 1;
      break;
    case E:
      coordinate.y += 1;
      break;
    case SE:
      coordinate.x += 1;
      coordinate.y += 1;
      break;
    case S:
      coordinate.x += 1;
      break;
    case SW:
      coordinate.x += 1;
      coordinate.y -= 1;
      break;
    default:
      break;
  }
}
