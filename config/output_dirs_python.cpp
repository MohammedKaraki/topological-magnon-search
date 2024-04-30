#include "config/output_dirs.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

PYBIND11_MODULE(output_dirs_python, m) { m.def("get_output_dirs", &magnon::get_output_dirs); }
