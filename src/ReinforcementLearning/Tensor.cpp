#include "Tensor.h"
#include <algorithm>

TensorStruct::TensorStruct(State &state, Map &map) {
    currentMap = GetMapTensor(map);

    float pGold = static_cast<float>(state.playerGold) / MAX_GOLD;
    playerGold = torch::tensor({pGold}).view({-1, 1});
    playerFood = GetVec(state.playerFood);
    playerStructs = GetStructuresTensor(state.playerStructs);
    playerUnits = GetUnitsTensor(state.playerUnits);

    float eGold = static_cast<float>(state.enemyGold) / MAX_GOLD;
    enemyGold = torch::tensor({eGold}).view({-1, 1});
    enemyFood = GetVec(state.enemyFood);
    enemyUnits = GetUnitsTensor(state.enemyUnits);
    enemyStructs = GetStructuresTensor(state.enemyStructs);
}

torch::Tensor TensorStruct::GetMapTensor(Map &map) {
    std::vector<float> data;

    for (const auto &row : map.terrain)
    {
        for (const auto &terrain : row)
        {
            data.push_back(1.0f); //this is terrain type ground or water
            data.push_back(static_cast<float>(terrain.resourceLeft) / MAX_MINE_GOLD);
            data.push_back(static_cast<float>(terrain.coord.x) / MAP_SIZE);
            data.push_back(static_cast<float>(terrain.coord.y) / MAP_SIZE);
        }
    }
    return torch::tensor(data).view({1, -1});
}
torch::Tensor TensorStruct::GetVec(Vec2 food) {
    std::vector<float> data;
    if (food.y <= 0)
        data.push_back(0.0f);
    else
        data.push_back(static_cast<float>(food.x) / food.y);
    data.push_back(static_cast<float>(food.y) / MAX_FOOD);
    return torch::tensor(data).view({1, -1});
}

torch::Tensor TensorStruct::GetUnitsTensor(std::vector<Unit *> &units) {
    std::vector<float> data;
    for (const auto &unit : units)
    {
        if (unit->maxHealth <= 0)
            data.push_back(static_cast<float>(unit->health));
        else
            data.push_back(static_cast<float>(unit->health) / unit->maxHealth);
        data.push_back(static_cast<float>(unit->coordinate.x) / MAP_SIZE);
        data.push_back(static_cast<float>(unit->coordinate.y) / MAP_SIZE);
        data.push_back(static_cast<float>(unit->is) / NR_OF_UNITS);
        data.push_back(static_cast<float>(unit->attack) / 20.0f);
        if (unit->maxMana <= 0)
            data.push_back(0.0f);
        else
            data.push_back(static_cast<float>(unit->mana) / unit->maxMana);


        if (unit->is == UnitType::PEASANT) {
            Peasant* p = static_cast<Peasant*>(unit);
            if (p->maxGoldInventory <= 0)
                data.push_back(0.0f);
            else
                data.push_back(static_cast<float>(p->goldInventory) / p->maxGoldInventory);
        } else {
            data.push_back(0.0f);
        }
    }
    return torch::tensor(data).view({1, -1});
}

torch::Tensor TensorStruct::GetStructuresTensor(
    std::vector<Structure *> &structures){

    std::vector<float> structureData;
    for (const auto &structure : structures)
    {
        if (structure->maxHealth <= 0)
            structureData.push_back(static_cast<float>(structure->health));
        else
            structureData.push_back(static_cast<float>(structure->health) / structure->maxHealth);
        structureData.push_back(static_cast<float>(structure->coordinate.x) / MAP_SIZE);
        structureData.push_back(static_cast<float>(structure->coordinate.y) / MAP_SIZE);
        structureData.push_back(static_cast<float>(structure->is) / NR_OF_STRUCTS);    
    }
    return torch::tensor(structureData).view({1, -1});
}

torch::Tensor TensorStruct::GetTensor() {
    auto options = torch::TensorOptions().dtype(torch::kFloat32);

    torch::Tensor paddedUnits = torch::zeros({1, std::max((int64_t)0, MAX_UNITS - (playerUnits.size(1) / unitVar)) * unitVar});
    torch::Tensor paddedStructs = torch::zeros({1, std::max((int64_t)0, MAX_STRUCTS - (playerStructs.size(1) / strucVar)) * strucVar});
    torch::Tensor paddedUnitsEnemy = torch::zeros({1, std::max((int64_t)0, MAX_UNITS - (enemyUnits.size(1) / unitVar)) * unitVar});
    torch::Tensor paddedStructsEnemy = torch::zeros({1, std::max((int64_t)0, MAX_STRUCTS - (enemyStructs.size(1) / strucVar)) * strucVar});

    torch::Tensor finalPUnits = playerUnits.size(1) > (MAX_UNITS * unitVar) 
        ? playerUnits.slice(1, 0, MAX_UNITS * unitVar) 
        : torch::cat({playerUnits, paddedUnits}, 1);

    torch::Tensor finalEUnits = enemyUnits.size(1) > (MAX_UNITS * unitVar) 
        ? enemyUnits.slice(1, 0, MAX_UNITS * unitVar) 
        : torch::cat({enemyUnits, paddedUnitsEnemy}, 1);

    torch::Tensor finalPStructs = playerStructs.size(1) > (MAX_STRUCTS * strucVar) 
        ? playerStructs.slice(1, 0, MAX_STRUCTS * strucVar) 
        : torch::cat({playerStructs, paddedStructs}, 1);
    
    torch::Tensor finalEStructs = enemyStructs.size(1) > (MAX_STRUCTS * strucVar) 
        ? enemyStructs.slice(1, 0, MAX_STRUCTS * strucVar) 
        : torch::cat({enemyStructs, paddedStructsEnemy}, 1);

    std::vector<torch::Tensor> tensors = {
        currentMap, 
        playerGold,   
        playerFood,   
        finalPUnits,  
        finalPStructs,
        enemyGold,    
        enemyFood,
        finalEUnits,    
        finalEStructs   
    };

    return torch::cat(tensors, 1); // Total is now ALWAYS 926
}
