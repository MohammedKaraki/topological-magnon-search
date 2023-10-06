#include "common/matrix_converter.hpp"
#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"
#include "pybind11_protobuf/native_proto_caster.h"

namespace py = pybind11;

PYBIND11_MODULE(matrix_converter_py, m) {
    pybind11_protobuf::ImportNativeProtoCasters();

    m.def("matrixxi_to_proto",
          py::overload_cast<const Eigen::MatrixXi &>(&magnon::common::to_proto));
    m.def("matrixxi_from_proto",
          py::overload_cast<const magnon::common::MatrixXi &>(&magnon::common::from_proto));

    m.def("matrixxd_to_proto",
          py::overload_cast<const Eigen::MatrixXd &>(&magnon::common::to_proto));
    m.def("matrixxd_from_proto",
          py::overload_cast<const magnon::common::MatrixXd &>(&magnon::common::from_proto));

    m.def("matrixxcd_to_proto",
          py::overload_cast<const Eigen::MatrixXcd &>(&magnon::common::to_proto));
    m.def("matrixxcd_from_proto",
          py::overload_cast<const magnon::common::MatrixXcd &>(&magnon::common::from_proto));

    m.def("matrix4d_to_proto",
          py::overload_cast<const Eigen::Matrix4d &>(&magnon::common::to_proto));
    m.def("matrix4d_from_proto",
          py::overload_cast<const magnon::common::Matrix4d &>(&magnon::common::from_proto));
}
