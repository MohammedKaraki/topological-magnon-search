#include "groups/read_standard_magnetic_space_groups.hpp"
#include "pybind11/pybind11.h"
#include "pybind11_protobuf/native_proto_caster.h"

namespace py = pybind11;

PYBIND11_MODULE(read_standard_magnetic_space_groups_py, m) {
    pybind11_protobuf::ImportNativeProtoCasters();

    m.def("read_standard_msgs_from_disk",
          &magnon::groups::read_standard_magnetic_space_groups_from_disk);
}
