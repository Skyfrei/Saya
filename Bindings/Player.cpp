
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 
#include "../State/Player.h"

namespace py = pybind11;
using Warcraft::State::Player;

PYBIND11_MODULE(pyl, m) {
    py::class_<Player>(m, "Player")
        .def(py::init<>)()
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

        .def_readonly("gold", &Player::gold)
        .def_readonly("food", &Player::food)
        .def_readonly("units", &Player::units)
        .def_readonly("structures", &Player::structures)
        .def_readonly("map", &Player::map)
}