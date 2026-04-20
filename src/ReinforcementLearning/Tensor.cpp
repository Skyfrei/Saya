#include "Tensor.h"
#include <algorithm>

TensorStruct::TensorStruct(State &state, Map &map) {
    int mapSize = MAP_SIZE * MAP_SIZE * 4;
    int totalSize = mapSize + 1 + 2 + (MAX_UNITS * unitVar) + (MAX_STRUCTS * strucVar) + 
                    1 + 2 + (MAX_UNITS * unitVar) + (MAX_STRUCTS * strucVar);

    std::vector<float> data(totalSize, 0.0f);
    int offset = 0;

    for (const auto &row : map.terrain) {
        for (const auto &terrain : row) {
            data[offset++] = 1.0f;
            data[offset++] = static_cast<float>(terrain.resourceLeft) / MAX_MINE_GOLD;
            data[offset++] = static_cast<float>(terrain.coord.x) / MAP_SIZE;
            data[offset++] = static_cast<float>(terrain.coord.y) / MAP_SIZE;
        }
    }

    data[offset++] = static_cast<float>(state.playerGold) / MAX_GOLD;

    if (state.playerFood.y <= 0) data[offset++] = 0.0f;
    else data[offset++] = static_cast<float>(state.playerFood.x) / state.playerFood.y;
    data[offset++] = static_cast<float>(state.playerFood.y) / MAX_FOOD;

    int pUnitCount = std::min(static_cast<int>(state.playerUnits.size()), MAX_UNITS);
    for (int i = 0; i < pUnitCount; i++) {
        auto unit = state.playerUnits[i];
        data[offset++] = (unit->maxHealth <= 0) ? static_cast<float>(unit->health) : static_cast<float>(unit->health) / unit->maxHealth;
        data[offset++] = static_cast<float>(unit->coordinate.x) / MAP_SIZE;
        data[offset++] = static_cast<float>(unit->coordinate.y) / MAP_SIZE;
        data[offset++] = static_cast<float>(unit->is) / NR_OF_UNITS;
        data[offset++] = static_cast<float>(unit->attack) / 20.0f;
        data[offset++] = static_cast<float>(unit->GetActionQueueSize()) / 2.0f;
        data[offset++] = (unit->maxMana <= 0) ? 0.0f : static_cast<float>(unit->mana) / unit->maxMana;

        if (unit->is == UnitType::PEASANT) {
            Peasant* p = static_cast<Peasant*>(unit);
            data[offset++] = (p->maxGoldInventory <= 0) ? 0.0f : static_cast<float>(p->goldInventory) / p->maxGoldInventory;
        } else {
            data[offset++] = 0.0f;
        }
    }
    offset += (MAX_UNITS - pUnitCount) * unitVar;

    int pStrucCount = std::min(static_cast<int>(state.playerStructs.size()), MAX_STRUCTS);
    for (int i = 0; i < pStrucCount; i++) {
        auto structure = state.playerStructs[i];
        data[offset++] = (structure->maxHealth <= 0) ? static_cast<float>(structure->health) : static_cast<float>(structure->health) / structure->maxHealth;
        data[offset++] = static_cast<float>(structure->coordinate.x) / MAP_SIZE;
        data[offset++] = static_cast<float>(structure->coordinate.y) / MAP_SIZE;
        data[offset++] = static_cast<float>(structure->is) / NR_OF_STRUCTS;
    }
    offset += (MAX_STRUCTS - pStrucCount) * strucVar;

    data[offset++] = static_cast<float>(state.enemyGold) / MAX_GOLD;

    if (state.enemyFood.y <= 0) data[offset++] = 0.0f;
    else data[offset++] = static_cast<float>(state.enemyFood.x) / state.enemyFood.y;
    data[offset++] = static_cast<float>(state.enemyFood.y) / MAX_FOOD;

    int eUnitCount = std::min(static_cast<int>(state.enemyUnits.size()), MAX_UNITS);
    for (int i = 0; i < eUnitCount; i++) {
        auto unit = state.enemyUnits[i];
        data[offset++] = (unit->maxHealth <= 0) ? static_cast<float>(unit->health) : static_cast<float>(unit->health) / unit->maxHealth;
        data[offset++] = static_cast<float>(unit->coordinate.x) / MAP_SIZE;
        data[offset++] = static_cast<float>(unit->coordinate.y) / MAP_SIZE;
        data[offset++] = static_cast<float>(unit->is) / NR_OF_UNITS;
        data[offset++] = static_cast<float>(unit->attack) / 20.0f;
        data[offset++] = static_cast<float>(unit->GetActionQueueSize()) / 2.0f;
        data[offset++] = (unit->maxMana <= 0) ? 0.0f : static_cast<float>(unit->mana) / unit->maxMana;

        if (unit->is == UnitType::PEASANT) {
            Peasant* p = static_cast<Peasant*>(unit);
            data[offset++] = (p->maxGoldInventory <= 0) ? 0.0f : static_cast<float>(p->goldInventory) / p->maxGoldInventory;
        } else {
            data[offset++] = 0.0f;
        }
    }
    offset += (MAX_UNITS - eUnitCount) * unitVar;

    int eStrucCount = std::min(static_cast<int>(state.enemyStructs.size()), MAX_STRUCTS);
    for (int i = 0; i < eStrucCount; i++) {
        auto structure = state.enemyStructs[i];
        data[offset++] = (structure->maxHealth <= 0) ? static_cast<float>(structure->health) : static_cast<float>(structure->health) / structure->maxHealth;
        data[offset++] = static_cast<float>(structure->coordinate.x) / MAP_SIZE;
        data[offset++] = static_cast<float>(structure->coordinate.y) / MAP_SIZE;
        data[offset++] = static_cast<float>(structure->is) / NR_OF_STRUCTS;
    }

    finalTensor = torch::from_blob(data.data(), {1, totalSize}, torch::kFloat32).clone();
}

torch::Tensor TensorStruct::GetTensor() {
    return finalTensor;
}

