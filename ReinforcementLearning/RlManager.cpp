#include "RlManager.h"

void RlManager::SaveModel(){
    //torch::save(target_net, "player.pt"); 
   // torch::save(policy_net, "enemy.pt");
}

void RlManager::LoadModel(){
    //torch::jit::load(target_net, "player.pt");
}

void RlManager::SaveExperienceBuffer(){
    std::ofstream outfile(fileName, std::ios::binary);
    
    if (!outfile) {
        throw std::runtime_error("Failed to open file for saving experience buffer.");
    }

    for (const auto& transition : memory) {
        outfile.write(reinterpret_cast<const char*>(&transition), sizeof(Transition));
    }
}

void RlManager::LoadExperienceBuffer(){
    std::ifstream inFile(fileName, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for loading experience buffer.");
    }

    Transition transition;
    while (inFile.read(reinterpret_cast<char*>(&transition), sizeof(Transition))) {
        memory.push_back(transition);
    }

    inFile.close();
}

double RlManager::CalculateStateReward(State state){
    double reward = 0.0;

    if (state.enemyUnits.size() <= 0 && state.enemyStructs.size() <= 0)
        reward = 1; 
    else if (state.playerUnits.size() <= 0 && state.playerStructs.size() <= 0)
        reward = -1;

    return reward;
}

void RlManager::StartPolicy(Map map, Player player, Player enemy) {
  if (calledMemOnce == false) {
    InitializeDQN(map, player, enemy);
    calledMemOnce = true;
  }

  State currentState = CreateCurrentState(map, player, enemy);

  actionT playerAction = policy_net.SelectAction(currentState);
  player.TakeAction(playerAction);
  actionT enemyAction = policy_net.SelectAction(currentState);
  enemy.TakeAction(enemyAction);
  
  State nextState = CreateCurrentState(map, player, enemy);
  nextState.reward = CalculateStateReward(nextState);

  //Transition egal = CreateTransition(currentState, playerAction, nextState);
  //memory.push_back(egal);
}

void RlManager::InitializeDQN(Map map, Player player, Player enemy){
  State st = CreateCurrentState(map, player, enemy);

  policy_net.Initialize(st);
  target_net.Initialize(st);

  torch::Device device(torch::kCPU);
  if (torch::cuda::is_available()) {
    device = torch::Device(torch::DeviceType::CUDA);
  }
  policy_net.to(device);
  target_net.to(device);
  torch::optim::AdamW optimizer(policy_net.parameters(), torch::optim::AdamWOptions(0.01).weight_decay(1e-4));
}

State RlManager::CreateCurrentState(Map map, Player player, Player enemy) {
  State st;
  st.currentMap = map;
  st.playerFood = player.food;
  st.playerGold = player.gold;

  st.enemyGold = enemy.gold;
  st.enemyFood = enemy.food;
  for (int i = 0; i < player.units.size(); i++) {
    Footman *footman = nullptr;
    Peasant *peasant = nullptr;
    switch (player.units[i]->is) {
      case FOOTMAN:
        footman = dynamic_cast<Footman *>(player.units[i].get());
        st.playerUnits.push_back(new Footman(*footman));
        break;
      case PEASANT:
        peasant = dynamic_cast<Peasant *>(player.units[i].get());
        st.playerUnits.push_back(new Peasant(*peasant));
        break;
      default:
        break;
    }
  }
  for (int i = 0; i < enemy.units.size(); i++) {
    Footman *footman = nullptr;
    Peasant *peasant = nullptr;
    switch (enemy.units[i]->is) {
      case FOOTMAN:
        footman = dynamic_cast<Footman *>(enemy.units[i].get());
        st.enemyUnits.push_back(new Footman(*footman));
        break;
      case PEASANT:
        peasant = dynamic_cast<Peasant *>(enemy.units[i].get());
        st.enemyUnits.push_back(new Peasant(*peasant));
        break;
      default:
        break;
    }
  }

  for (int i = 0; i < player.structures.size(); i++) {
    Barrack *barracks = nullptr;
    Farm *farm = nullptr;
    TownHall *townHall = nullptr;
    switch (player.structures[i]->is) {
      case BARRACK:
        barracks = dynamic_cast<Barrack *>(player.structures[i].get());
        st.playerStructs.push_back(new Barrack(*barracks));
        break;
      case FARM:
        farm = dynamic_cast<Farm *>(player.structures[i].get());
        st.playerStructs.push_back(new Farm(*farm));
        break;
      case HALL:
        townHall = dynamic_cast<TownHall *>(player.structures[i].get());
        st.playerStructs.push_back(new TownHall(*townHall));
        break;
      default:
        break;
    }
  }

  for (int i = 0; i < enemy.structures.size(); i++) {
    Barrack *barracks = nullptr;
    Farm *farm = nullptr;
    TownHall *townHall = nullptr;
    switch (enemy.structures[i]->is) {
      case BARRACK:
        barracks = dynamic_cast<Barrack *>(enemy.structures[i].get());
        st.enemyStructs.push_back(new Barrack(*barracks));
        break;
      case FARM:
        farm = dynamic_cast<Farm *>(enemy.structures[i].get());
        st.enemyStructs.push_back(new Farm(*farm));
        break;
      case HALL:
        townHall = dynamic_cast<TownHall *>(enemy.structures[i].get());
        st.enemyStructs.push_back(new TownHall(*townHall));
        break;
      default:
        break;
    }
  }
  return st;
}

Transition RlManager::CreateTransition(State s, actionT a, State nextS) {
  Transition t(s, a, nextS);
  return t;
}

void RlManager::AddMemory(const Transition& experience){
    if (memory.size() >= maxSize) 
        memory.pop_front();  // Remove oldest experience if full
    memory.push_back(experience);
}

std::vector<Transition> RlManager::Sample(size_t batch_size) {
    std::vector<Transition> batch;
    std::sample(memory.begin(), memory.end(), std::back_inserter(batch), batch_size, std::mt19937{std::random_device{}()});
    return batch;
}

void RlManager::OptimizeModel(std::deque<Transition> memory) {
  if (memory.size() < batchSize) {
      return;
  }
  std::vector<Transition> samples = Sample(batchSize);

  std::vector<torch::Tensor> state_batch, action_batch, reward_batch, next_state_batch;
  for (const auto& t : samples) {
    TensorStruct s(t.state);
    TensorStruct ns(t.nextState);
    state_batch.push_back(s.GetTensor());
    //action_batch.push_back(torch::tensor(t.stateAction.action, torch::kFloat32)); // fix this shit
    reward_batch.push_back(torch::tensor(t.nextState.reward, torch::kFloat32));
    next_state_batch.push_back(ns.GetTensor());
  }

  torch::Tensor state_tensor = torch::stack(state_batch);
  torch::Tensor action_tensor = torch::stack(action_batch);
  torch::Tensor reward_tensor = torch::stack(reward_batch);
  torch::Tensor next_state_tensor = torch::stack(next_state_batch);

  auto state_action_values = policy_net.Forward(state_tensor).gather(1, action_tensor);
  // torch::Tensor next_state_values = torch::zeros({batchSize}, torch::kFloat32);
  // {
  //     torch::NoGradGuard no_grad; // Disable gradient computation
  //     torch::Tensor non_final_mask = torch::ones({batchSize}, torch::kBool);
  //     for (int i = 0; i < batchSize; ++i) {
  //         if (!next_state_tensor[i].defined()) {
  //             non_final_mask[i] = false;
  //         }
  //     }
  //     auto next_state_values_tensor = target_net.Forward(next_state_tensor);
  //     next_state_values.index_put_({non_final_mask}, next_state_values_tensor.max(1).values.index({non_final_mask}));
  //     next_state_values = next_state_values.detach();
  // }
  // torch::Tensor expected_state_action_values = (next_state_values * gamma) + reward_tensor;
  // torch::nn::SmoothL1Loss criterion;
  // torch::Tensor loss = criterion->forward(state_action_values, expected_state_action_values.unsqueeze(1));
  // optimizer.zero_grad();
  // loss.backward();
  // optimizer.step();
}


//========================================= Serialization and Deserialization for file saving ===================================================











void RlManager::Serialize(const State& state) {
    std::ofstream outFile(fileName, std::ios::binary); 
    
    outFile.write(reinterpret_cast<const char*>(&state.playerGold), sizeof(int));
    outFile.write(reinterpret_cast<const char*>(&state.enemyGold), sizeof(int));
    outFile.write(reinterpret_cast<const char*>(&state.reward), sizeof(double));

    // Serialize player units
    size_t unitsSize = state.playerUnits.size();
    outFile.write(reinterpret_cast<const char*>(&unitsSize), sizeof(size_t));
    for (const auto& unit : state.playerUnits) {
        SerializeUnit(outFile, *unit);  // Using external serializeUnit function
    }

    // Serialize player structures
    size_t structsSize = state.playerStructs.size();
    outFile.write(reinterpret_cast<const char*>(&structsSize), sizeof(size_t));
    for (const auto& structure : state.playerStructs) {
        SerializeStructure(outFile, *structure);  // Using external serializeStructure function
    }

    // Serialize enemy units
    unitsSize = state.enemyUnits.size();
    outFile.write(reinterpret_cast<const char*>(&unitsSize), sizeof(size_t));
    for (const auto& unit : state.enemyUnits) {
        SerializeUnit(outFile, *unit);
    }

    // Serialize enemy structures
    structsSize = state.enemyStructs.size();
    outFile.write(reinterpret_cast<const char*>(&structsSize), sizeof(size_t));
    for (const auto& structure : state.enemyStructs) {
        SerializeStructure(outFile, *structure);
    }

    // Serialize map and other components
    SerializeMap(outFile, state.currentMap);
    SerializeVec2(outFile, state.enemyFood);
    SerializeVec2(outFile, state.playerFood);
    SerializeAction(outFile, state.action);
}

void RlManager::Deserialize(State& state) {
    // DeSerialize primitive fields
    std::ifstream inFile(fileName, std::ios::binary);
    inFile.read(reinterpret_cast<char*>(&state.playerGold), sizeof(int));
    inFile.read(reinterpret_cast<char*>(&state.enemyGold), sizeof(int));
    inFile.read(reinterpret_cast<char*>(&state.reward), sizeof(double));

    // DeSerialize player units
    size_t unitsSize;
    inFile.read(reinterpret_cast<char*>(&unitsSize), sizeof(size_t));
    state.playerUnits.resize(unitsSize);
    for (auto& unit : state.playerUnits) {
        unit = new Unit();  // Ensure we allocate memory for the unit pointer
        DeserializeUnit(inFile, *unit);  // Using external deserializeUnit function
    }

    // DeSerialize player structures
    size_t structsSize;
    inFile.read(reinterpret_cast<char*>(&structsSize), sizeof(size_t));
    state.playerStructs.resize(structsSize);
    for (auto& structure : state.playerStructs) {
        structure = new Structure();
        DeserializeStructure(inFile, *structure);  // Using external deserializeStructure function
    }

    // DeSerialize enemy units
    inFile.read(reinterpret_cast<char*>(&unitsSize), sizeof(size_t));
    state.enemyUnits.resize(unitsSize);
    for (auto& unit : state.enemyUnits) {
        unit = new Unit();
        DeserializeUnit(inFile, *unit);
    }

    // DeSerialize enemy structures
    inFile.read(reinterpret_cast<char*>(&structsSize), sizeof(size_t));
    state.enemyStructs.resize(structsSize);
    for (auto& structure : state.enemyStructs) {
        structure = new Structure();
        DeserializeStructure(inFile, *structure);
    }

    // DeSerialize map and other components
    DeserializeMap(inFile, state.currentMap);
    DeserializeVec2(inFile, state.enemyFood);
    DeserializeVec2(inFile, state.playerFood);
    DeserializeAction(inFile, state.action);
}




void SerializeVec2(std::ostream& out, const Vec2& vec) {
    out.write(reinterpret_cast<const char*>(&vec.x), sizeof(int));
    out.write(reinterpret_cast<const char*>(&vec.y), sizeof(int));
}

void DeserializeVec2(std::istream& in, Vec2& vec) {
    in.read(reinterpret_cast<char*>(&vec.x), sizeof(int));
    in.read(reinterpret_cast<char*>(&vec.y), sizeof(int));
}

// Serialization for Terrain
void SerializeTerrain(std::ostream& out, const Terrain& terrain) {
    SerializeVec2(out, terrain.coord);
    out.write(reinterpret_cast<const char*>(&terrain.resourceLeft), sizeof(int));
    out.write(reinterpret_cast<const char*>(&terrain.type), sizeof(TerrainType));

    // Serialize living objects on terrain
    size_t livingCount = terrain.onTerrainLiving.size();
    out.write(reinterpret_cast<const char*>(&livingCount), sizeof(size_t));
    for (const auto* living : terrain.onTerrainLiving) {
        int livingId = (living != nullptr) ? living->GetID() : -1;  // Assuming Living has GetID()
        out.write(reinterpret_cast<const char*>(&livingId), sizeof(int));
    }

    // Serialize Structure on Terrain
    int structureId = (terrain.structureOnTerrain != nullptr) ? terrain.structureOnTerrain->GetID() : -1;
    out.write(reinterpret_cast<const char*>(&structureId), sizeof(int));
}

void DeserializeTerrain(std::istream& in, Terrain& terrain) {
    DeserializeVec2(in, terrain.coord);
    in.read(reinterpret_cast<char*>(&terrain.resourceLeft), sizeof(int));
    in.read(reinterpret_cast<char*>(&terrain.type), sizeof(TerrainType));

    // DeSerialize living objects on terrain
    size_t livingCount;
    in.read(reinterpret_cast<char*>(&livingCount), sizeof(size_t));
    terrain.onTerrainLiving.resize(livingCount);
    for (auto& living : terrain.onTerrainLiving) {
        int livingId;
        in.read(reinterpret_cast<char*>(&livingId), sizeof(int));
        living = (livingId != -1) ? findLivingById(livingId) : nullptr;  // Define findLivingById in RLManager
    }

    // DeSerialize Structure on Terrain
    int structureId;
    in.read(reinterpret_cast<char*>(&structureId), sizeof(int));
    terrain.structureOnTerrain = (structureId != -1) ? findStructureById(structureId) : nullptr;  // Define findStructureById in RLManager
}

// Serialization for Map
void SerializeMap(std::ostream& out, const Map& map) {
    size_t rows = map.terrain.size();
    size_t cols = rows > 0 ? map.terrain[0].size() : 0;
    out.write(reinterpret_cast<const char*>(&rows), sizeof(size_t));
    out.write(reinterpret_cast<const char*>(&cols), sizeof(size_t));

    for (const auto& row : map.terrain) {
        for (const auto& cell : row) {
            SerializeTerrain(out, cell);
        }
    }
}

void DeserializeMap(std::istream& in, Map& map) {
    size_t rows, cols;
    in.read(reinterpret_cast<char*>(&rows), sizeof(size_t));
    in.read(reinterpret_cast<char*>(&cols), sizeof(size_t));

    map.terrain.resize(rows, std::vector<Terrain>(cols, Terrain(0, 0)));
    for (auto& row : map.terrain) {
        for (auto& cell : row) {
            DeserializeTerrain(in, cell);
        }
    }
}

// Serialization for Structure
void SerializeStructure(std::ostream& out, const Structure& structure) {
    out.write(reinterpret_cast<const char*>(&structure.is), sizeof(StructureType));
    out.write(reinterpret_cast<const char*>(&structure.isBeingBuilt), sizeof(bool));
    SerializeVec2(out, structure.coordinate);
}

void DeserializeStructure(std::istream& in, Structure& structure) {
    in.read(reinterpret_cast<char*>(&structure.is), sizeof(StructureType));
    in.read(reinterpret_cast<char*>(&structure.isBeingBuilt), sizeof(bool));
    DeserializeVec2(in, structure.coordinate);
}

// Serialization for Transition
void SerializeTransition(std::ostream& out, const Transition& transition) {
    SerializeState(out, transition.state);
    SerializeState(out, transition.nextState);
    out.write(reinterpret_cast<const char*>(&transition.done), sizeof(bool));
}

void DeserializeTransition(std::istream& in, Transition& transition) {
    DeserializeState(in, transition.state);
    DeserializeState(in, transition.nextState);
    in.read(reinterpret_cast<char*>(&transition.done), sizeof(bool));
}

void SerializeState(std::ostream& out, const State& state) {
    SerializeMap(out, state.currentMap);
    out.write(reinterpret_cast<const char*>(&state.playerGold), sizeof(int));
    SerializeVec2(out, state.playerFood);

    // Serialize each player's units and structures
    size_t unitsSize = state.playerUnits.size();
    out.write(reinterpret_cast<const char*>(&unitsSize), sizeof(size_t));
    for (const auto* unit : state.playerUnits) {
        int unitId = (unit != nullptr) ? unit->GetID() : -1;
        out.write(reinterpret_cast<const char*>(&unitId), sizeof(int));
    }

    size_t structsSize = state.playerStructs.size();
    out.write(reinterpret_cast<const char*>(&structsSize), sizeof(size_t));
    for (const auto* structure : state.playerStructs) {
        SerializeStructure(out, *structure);  // Assuming Structure has serialize function
    }

    out.write(reinterpret_cast<const char*>(&state.enemyGold), sizeof(int));
    SerializeVec2(out, state.enemyFood);

    // Enemy units and structures
    unitsSize = state.enemyUnits.size();
    out.write(reinterpret_cast<const char*>(&unitsSize), sizeof(size_t));
    for (const auto* unit : state.enemyUnits) {
        int unitId = (unit != nullptr) ? unit->GetID() : -1;
        out.write(reinterpret_cast<const char*>(&unitId), sizeof(int));
    }

    structsSize = state.enemyStructs.size();
    out.write(reinterpret_cast<const char*>(&structsSize), sizeof(size_t));
    for (const auto* structure : state.enemyStructs) {
        SerializeStructure(out, *structure);
    }

    SerializeAction(out, state.action);
    out.write(reinterpret_cast<const char*>(&state.reward), sizeof(double));
}

void DeserializeState(std::istream& in, State& state) {
    DeserializeMap(in, state.currentMap);
    in.read(reinterpret_cast<char*>(&state.playerGold), sizeof(int));
    DeserializeVec2(in, state.playerFood);

    // DeSerialize player's units and structures
    size_t unitsSize;
    in.read(reinterpret_cast<char*>(&unitsSize), sizeof(size_t));
    state.playerUnits.resize(unitsSize);
    for (auto& unit : state.playerUnits) {
        int unitId;
        in.read(reinterpret_cast<char*>(&unitId), sizeof(int));
        unit = (unitId != -1) ? findUnitById(unitId) : nullptr;
    }

    size_t structsSize;
    in.read(reinterpret_cast<char*>(&structsSize), sizeof(size_t));
    state.playerStructs.resize(structsSize);
    for (auto& structure : state.playerStructs) {
        structure = new Structure();
        DeserializeStructure(in, *structure);
    }

    in.read(reinterpret_cast<char*>(&state.enemyGold), sizeof(int));
    DeserializeVec2(in, state.enemyFood);

    // Enemy units and structures
    in.read(reinterpret_cast<char*>(&unitsSize), sizeof(size_t));
    state.enemyUnits.resize(unitsSize);
    for (auto& unit : state.enemyUnits) {
        int unitId;
        in.read(reinterpret_cast<char*>(&unitId), sizeof(int));
        unit = (unitId != -1) ? findUnitById(unitId) : nullptr;
    }

    in.read(reinterpret_cast<char*>(&structsSize), sizeof(size_t));
    state.enemyStructs.resize(structsSize);
    for (auto& structure : state.enemyStructs) {
        structure = new Structure();
        DeserializeStructure(in, *structure);
    }

    DeserializeAction(in, state.action);
    in.read(reinterpret_cast<char*>(&state.reward), sizeof(double));
}




