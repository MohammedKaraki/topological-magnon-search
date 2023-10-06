#pragma once

#include "Eigen/Core"

#include "common/matrix.pb.h"

namespace magnon::common {

Eigen::MatrixXi from_proto(const MatrixXi &matrix_proto);
MatrixXi to_proto(const Eigen::MatrixXi &matrix);

}  // namespace magnon::common
