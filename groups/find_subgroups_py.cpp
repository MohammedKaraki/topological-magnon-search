#include "groups/find_subgroups.hpp"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11_protobuf/native_proto_caster.h"

namespace py = pybind11;

PYBIND11_MODULE(find_subgroups_py, m) {
    pybind11_protobuf::ImportNativeProtoCasters();

    m.def("find_subgroups", &magnon::groups::find_subgroups);
}
