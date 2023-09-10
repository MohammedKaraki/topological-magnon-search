#include "config/read_global_config.hpp"
#include "pybind11/pybind11.h"
#include "pybind11_protobuf/native_proto_caster.h"

PYBIND11_MODULE(read_global_config_python, m) {
    pybind11_protobuf::ImportNativeProtoCasters();
    m.def("read_global_config", &magnon::read_global_config);
}
