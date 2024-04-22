#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"
#include "pybind11_protobuf/native_proto_caster.h"
#include "utils/matrix_converter.hpp"

namespace py = pybind11;

PYBIND11_MODULE(matrix_converter_py, m) {
    pybind11_protobuf::ImportNativeProtoCasters();

    m.def("matrixxi_to_proto",
          py::overload_cast<const Eigen::MatrixXi &>(&magnon::utils::to_proto));
    m.def("matrixxi_from_proto",
          py::overload_cast<const magnon::utils::MatrixXi &>(&magnon::utils::from_proto));

    m.def("matrixxd_to_proto",
          py::overload_cast<const Eigen::MatrixXd &>(&magnon::utils::to_proto));
    m.def("matrixxd_from_proto",
          py::overload_cast<const magnon::utils::MatrixXd &>(&magnon::utils::from_proto));

    m.def("matrixxcd_to_proto",
          py::overload_cast<const Eigen::MatrixXcd &>(&magnon::utils::to_proto));
    m.def("matrixxcd_from_proto",
          py::overload_cast<const magnon::utils::MatrixXcd &>(&magnon::utils::from_proto));

    m.def("matrix4d_to_proto",
          py::overload_cast<const Eigen::Matrix4d &>(&magnon::utils::to_proto));
    m.def("matrix4d_from_proto",
          py::overload_cast<const magnon::utils::Matrix4d &>(&magnon::utils::from_proto));
}
