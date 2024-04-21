#include "Eigen/Core"

namespace magnon::utils {

using MatrixXl = Eigen::Matrix<long, Eigen::Dynamic, Eigen::Dynamic>;

// This function computes the Smith decomposition of a general integer-valued matrix.
// It returns a tuple of three matrices: P, A, T. Here, P and T are two square, invertible,
// integer-valued matrices, and A is a diagnoal matrices with the same dimensions as the input
// matrix. These matrices satisfy the equation P*A*T = `matrix`.
std::tuple<MatrixXl, MatrixXl, MatrixXl> compute_smith_normal_form(const MatrixXl &MatrixXl);

}  // namespace magnon::utils
