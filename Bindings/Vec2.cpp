
#include <pybind11/pybind11.h>
#include <pybind11/stl.h> 
#include "../Tools/Vec2.h"

namespace py = pybind11;

void init_Vec2(py::module &m)
{
    py::class_<Vec2>(m, "Vec2")
        .def(py::init<int, int>())
        .def_readwrite("x", &Vec2::x)
        .def_readwrite("y", &Vec2::y)
        .def("__eq__", &Vec2::operator==)
        .def("__ne__", &Vec2::operator!=)
        .def("__repr__", [](const Vec2 &v) {
            return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        });
}

PYBIND11_MODULE(pyl, m)
{
    init_Vec2(m);
    // Add other bindings here if needed
}