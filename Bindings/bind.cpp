// pyl.cpp
#include <pybind11/pybind11.h>
#include "../Tools/Vec2.h"
#include "../Race/Unit/Footman.h"
#include "../Race/Unit/Unit.h"
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

    py::class_<Unit, Living>(m, "Unit")
        .def("Move", &Unit::Move)
        .def("WithinDistance", &Unit::WithinDistance)
        .def("FindDifference", &Unit::FindDifference)
        .def("Attack", &Unit::Attack)
        .def("GetAttackTime", &Unit::GetAttackTime)
        .def("RegenHealth", &Unit::RegenHealth)
        .def("CheckTask", &Unit::CheckTask)
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
        .def_readwrite("time1", &Unit::time1)
        .def_readwrite("hpTime", &Unit::hpTime)
        .def_readwrite("time", &Unit::time)
        .def_readwrite("is", &Unit::is);

    py::class_<Footman, Unit>(m, "Footman")
        .def(py::init<>())
        .def("GetDescription", &Footman::GetDescription);
}
