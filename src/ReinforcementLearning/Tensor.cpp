#include "Tensor.h"

TensorStruct::TensorStruct(State &state, Map &map) {
    currentMap = GetMapTensor(map);
    playerGold = torch::tensor(state.playerGold).view({-1, 1});
    playerFood = GetVec(state.playerFood);
    playerStructs = GetStructuresTensor(state.playerStructs);
    playerUnits = GetUnitsTensor(state.playerUnits);

    enemyGold = torch::tensor(state.enemyGold).view({-1, 1});
    enemyFood = GetVec(state.enemyFood);
    enemyUnits = GetUnitsTensor(state.enemyUnits);
    enemyStructs = GetStructuresTensor(state.enemyStructs);
}

torch::Tensor TensorStruct::GetMapTensor(Map &map) {
    std::vector<int> data;

    for (const auto &row : map.terrain)
    {
        for (const auto &terrain : row)
        {
            data.push_back((terrain.type));
            data.push_back((terrain.resourceLeft));
            data.push_back(terrain.coord.x);
            data.push_back(terrain.coord.y);
        }
    }
    return torch::tensor(data).view({1, -1});
}
torch::Tensor TensorStruct::GetVec(Vec2 food) {
    std::vector<int> data;
    data.push_back(food.x);
    data.push_back(food.y);
    return torch::tensor(data).view({1, -1});
}

torch::Tensor TensorStruct::GetUnitsTensor(std::vector<Unit *> &units) {
    std::vector<int> data;
    for (const auto &unit : units)
    {
        // Extract health and coordinates from each structure
        data.push_back(static_cast<int>(unit->health));
        data.push_back(unit->coordinate.x);
        data.push_back(unit->coordinate.y);
        data.push_back(unit->is);
        data.push_back(static_cast<int>(unit->attack));
        data.push_back(static_cast<int>(unit->maxHealth));
        data.push_back(static_cast<int>(unit->mana));
        data.push_back(static_cast<int>(unit->maxMana));
    }
    return torch::tensor(data).view({1, -1});
}
torch::Tensor TensorStruct::GetStructuresTensor(
    std::vector<Structure *> &structures) {
    std::vector<int> structureData;
    for (const auto &structure : structures)
    {
        structureData.push_back(static_cast<int>(structure->health));
        structureData.push_back(structure->coordinate.x);
        structureData.push_back(structure->coordinate.y);
        structureData.push_back(structure->is);
    }
    return torch::tensor(structureData).view({1, -1});
}
torch::Tensor TensorStruct::GetTensor() {
    torch::Tensor paddedUnits =
        torch::zeros({1, (MAX_UNITS - (playerUnits.size(1) / unitVar)) * unitVar});
    torch::Tensor paddedStructs = torch::zeros(
        {1, (MAX_STRUCTS - (playerStructs.size(1) / strucVar)) * strucVar});
    torch::Tensor paddedUnitsEnemy =
        torch::zeros({1, (MAX_UNITS - (enemyUnits.size(1) / unitVar)) * unitVar});
    torch::Tensor paddedStructsEnemy = torch::zeros(
        {1, (MAX_STRUCTS - (enemyStructs.size(1) / strucVar)) * strucVar});

    std::vector<torch::Tensor> tensors = {
        playerGold,    playerFood,       playerUnits,  paddedUnits,
        playerStructs, paddedStructs,    enemyGold,    enemyFood,
        enemyUnits,    paddedUnitsEnemy, enemyStructs, paddedStructsEnemy};

    torch::Tensor concatenatedTensor = torch::cat(tensors, 1);

    return concatenatedTensor;
}
