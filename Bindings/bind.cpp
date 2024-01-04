// pyl.cpp
#include <pybind11/pybind11.h>
#include "../Tools/Vec2.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Unit.h"
#include "../Race/Unit/Peasant.h"
#include "../State/Player.h"
#include "../Map/Terrain.h"
#include "../Map/Map.h"
#include "../State/Manager.h"
namespace py = pybind11;

using namespace Warcraft::Units;

PYBIND11_MODULE(Warcraft, m) {
    // Vec2 submodule
    py::class_<Vec2>(m, "Vec2")
        .def(py::init<int, int>())
        .def_readwrite("x", &Vec2::x)
        .def_readwrite("y", &Vec2::y)
        .def("__eq__", &Vec2::operator==)
        .def("__ne__", &Vec2::operator!=)
        .def("__repr__", [](const Vec2 &v) {
            return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });

    py::enum_<UnitType>(m, "UnitType")
        .value("FOOTMAN", UnitType::FOOTMAN)
        .value("PEASANT", UnitType::PEASANT)
        .value("ARCHMAGE", UnitType::ARCHMAGE)
        .value("BLOODMAGE", UnitType::BLOODMAGE)
        .value("OTHER", UnitType::OTHER);

    // Define the Enum MoveType
    py::enum_<MoveType>(m, "MoveType")
        .value("W", MoveType::W)
        .value("NW", MoveType::NW)
        .value("N", MoveType::N)
        .value("NE", MoveType::NE)
        .value("E", MoveType::E)
        .value("SE", MoveType::SE)
        .value("S", MoveType::S)
        .value("SW", MoveType::SW)
        .value("STAY", MoveType::STAY);

    py::enum_<TerrainType>(m, "TerrainType")
        .value("GROUND", TerrainType::GROUND)
        .value("GOLD", TerrainType::GOLD);

    py::class_<Living>(m, "Living");

    // Unit module
    py::class_<Unit, Living>(m, "Unit")
        .def("Move", &Unit::Move)
        .def("WithinDistance", &Unit::WithinDistance)
        .def("FindDifference", &Unit::FindDifference)
        .def("Attack", &Unit::Attack)
        .def("GetAttackTime", &Unit::GetAttackTime)
        .def("RegenHealth", &Unit::RegenHealth)
        //.def("CheckTask", &Unit::CheckTask)
        .def("ChangeCoordinate", &Unit::ChangeCoordinate)
        .def_readwrite("isInvisible", &Unit::isInvisible)
        .def_readwrite("attack", &Unit::attack)
        .def_readwrite("attackCooldown", &Unit::attackCooldown)
        .def_readwrite("mana", &Unit::mana)
        .def_readwrite("manaRegen", &Unit::manaRegen)
        .def_readwrite("maxMana", &Unit::maxMana)
        .def_readwrite("attackRange", &Unit::attackRange)
        .def_readwrite("movementSpeed", &Unit::movementSpeed)
        .def_readwrite("hpRegen", &Unit::hpRegen)
        .def_readwrite("hpTime", &Unit::hpTime)
        .def_readwrite("is", &Unit::is);

    // Peasant module
    py::class_<Peasant, Unit>(m, "Peasant")
        .def(py::init<>())
        .def("GetDescription", &Peasant::GetDescription)
        .def("Build", &Peasant::Build)
        .def("WorkOnTask", &Peasant::WorkOnTask)
        .def("ChangeBuilding", &Peasant::ChangeBuilding)
        .def("FarmGold", &Peasant::FarmGold)
        .def("TransferGold", &Peasant::TransferGold)
        .def_readwrite("goldInventory", &Peasant::goldInventory);

    // Footman module
    py::class_<Footman, Unit>(m, "Footman")
        .def(py::init<>())
        .def("GetDescription", &Footman::GetDescription);

    // Structure module
    py::class_<Structure, Living>(m, "Structure")
        .def("FinishBuilding", &Structure::FinishBuilding)
        .def_readwrite("is", &Structure::is)
        //.def_readwrite("tasks", &Structure::tasks)
        .def_readwrite("isBeingBuilt", &Structure::isBeingBuilt);
    
    // TownHall module
    py::class_<TownHall, Structure>(m, "TownHall")
        .def(py::init<>())
        .def("UpgradeEquipment", &TownHall::UpgradeEquipment)
        .def("FinishBuilding", &TownHall::FinishBuilding)
        .def("GetDescription", &TownHall::GetDescription)
        .def_readwrite("currentUpgrade", &TownHall::currentUpgrade);

    // Barrack module
    py::class_<Barrack, Structure>(m, "Barrack")
        .def(py::init<>())
        .def("FinishBuilding", &Barrack::FinishBuilding)
        .def("GetDescription", &Barrack::GetDescription)
        .def("CreateUnit", &Barrack::CreateUnit);
    // Farm module
    py::class_<Farm, Structure>(m, "Farm")
        .def(py::init<>())
        .def("GetDescription", &Farm::GetDescription)
        .def("FinishBuilding", &Farm::FinishBuilding)
        .def("GetFood", &Farm::GetFood);
    
    // Player module
    py::class_<Player>(m, "Player")
        .def(py::init<>())
        .def("Initialize", &Player::Initialize)
        .def("SetInitialCoordinates", &Player::SetInitialCoordinates)
        .def("HasStructure", &Player::HasStructure)
        .def("HasUnit", &Player::HasUnit)
        .def("FindClosestStructure", &Player::FindClosestStructure)
        .def("Select", &Player::Select)
        .def("ValidateFood", &Player::ValidateFood)
        .def("UpdateGold", &Player::UpdateGold)
        .def("ChooseToBuild", &Player::ChooseToBuild)
        .def("RecruitSoldier", &Player::RecruitSoldier)
        .def_readwrite("gold", &Player::gold)
        .def_readwrite("food", &Player::food)
        .def_readwrite("map", &Player::map);

    // Manager module
    py::class_<Manager>(m, "Manager")
        .def(py::init<>())
        .def("MainLoop", &Manager::MainLoop)
        .def("ManageLiving", &Manager::ManageLiving)
        .def("GetTime", &Manager::GetTime)
        .def("CheckForMovement", &Manager::CheckForMovement)
        .def_readwrite("player", &Manager::player)
        .def_readwrite("enemy", &Manager::enemy)
        .def_readwrite("map", &Manager::map);

    // Terrain module
    py::class_<Terrain>(m, "Terrain")
        .def(py::init<int, int>())
        .def("GetResourceLeft", &Terrain::GetResourceLeft)
        .def_readonly("coord", &Terrain::coord)
        .def_readonly("resourceLeft", &Terrain::resourceLeft)
        .def_readonly("type", &Terrain::type);

    // Map module
    py::class_<Map>(m, "Map")
        .def(py::init<>())
        .def_readwrite("objects", &Map::objects);
}
