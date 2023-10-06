#include "common/matrix_converter.hpp"
#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"
#include "pybind11_protobuf/native_proto_caster.h"

PYBIND11_MODULE(matrix_converter_py, m) {
    pybind11_protobuf::ImportNativeProtoCasters();
    m.def("matrix_to_proto", &magnon::common::to_proto);
    m.def("matrix_from_proto", &magnon::common::from_proto);
}
