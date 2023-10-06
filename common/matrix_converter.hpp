#pragma once

#include "Eigen/Core"

#include "common/matrix.pb.h"

namespace magnon::common {

Eigen::MatrixXi from_proto(const MatrixXi &matrix_proto);
Eigen::MatrixXd from_proto(const MatrixXd &matrix_proto);
Eigen::MatrixXcd from_proto(const MatrixXcd &matrix_proto);
Eigen::Matrix4d from_proto(const Matrix4d &matrix_proto);
Eigen::Vector4d from_proto(const Vector4d &matrix_proto);

MatrixXi to_proto(const Eigen::MatrixXi &matrix);
MatrixXd to_proto(const Eigen::MatrixXd &matrix);
MatrixXcd to_proto(const Eigen::MatrixXcd &matrix);
Matrix4d to_proto(const Eigen::Matrix4d &matrix);
Vector4d to_proto(const Eigen::Vector4d &matrix);

}  // namespace magnon::common
