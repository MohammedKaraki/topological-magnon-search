#include "common/matrix_converter.hpp"

#include <cstddef>

namespace magnon::common {

Eigen::MatrixXi from_proto(const MatrixXi &matrix_proto) {
    const std::size_t num_rows = matrix_proto.num_rows();
    const std::size_t num_columns = matrix_proto.num_columns();
    assert(num_rows > 0 && num_columns > 0);
    const std::size_t num_entries = matrix_proto.entry_size();
    assert(num_rows * num_columns == num_entries);

    Eigen::MatrixXi result(num_rows, num_columns);
    for (auto row = 0U; row < num_rows; ++row) {
        for (auto column = 0U; column < num_columns; ++column) {
            const auto entry_index = num_columns * row + column;
            result(row, column) = matrix_proto.entry(entry_index);
        }
    }
    return result;
}

MatrixXi to_proto(const Eigen::MatrixXi &matrix) {
    const std::size_t num_rows = matrix.rows();
    const std::size_t num_columns = matrix.cols();

    MatrixXi result;
    result.set_num_rows(num_rows);
    result.set_num_columns(num_columns);
    for (auto row = 0U; row < num_rows; ++row) {
        for (auto column = 0U; column < num_columns; ++column) {
            result.add_entry(matrix(row, column));
        }
    }
    return result;
}

}  // namespace magnon::common
