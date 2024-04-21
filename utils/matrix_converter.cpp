#include "utils/matrix_converter.hpp"

#include <cstddef>

#include "utils/complex_converter.hpp"

namespace magnon::utils {

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

Eigen::MatrixXd from_proto(const MatrixXd &matrix_proto) {
    const std::size_t num_rows = matrix_proto.num_rows();
    const std::size_t num_columns = matrix_proto.num_columns();
    assert(num_rows > 0 && num_columns > 0);
    const std::size_t num_entries = matrix_proto.entry_size();
    assert(num_rows * num_columns == num_entries);

    Eigen::MatrixXd result(num_rows, num_columns);
    for (auto row = 0U; row < num_rows; ++row) {
        for (auto column = 0U; column < num_columns; ++column) {
            const auto entry_index = num_columns * row + column;
            result(row, column) = matrix_proto.entry(entry_index);
        }
    }
    return result;
}

Eigen::MatrixXcd from_proto(const MatrixXcd &matrix_proto) {
    const std::size_t num_rows = matrix_proto.num_rows();
    const std::size_t num_columns = matrix_proto.num_columns();
    assert(num_rows > 0 && num_columns > 0);
    const std::size_t num_entries = matrix_proto.entry_size();
    assert(num_rows * num_columns == num_entries);

    Eigen::MatrixXcd result(num_rows, num_columns);
    for (auto row = 0U; row < num_rows; ++row) {
        for (auto column = 0U; column < num_columns; ++column) {
            const auto entry_index = num_columns * row + column;
            result(row, column) = from_proto(matrix_proto.entry(entry_index));
        }
    }
    return result;
}

Eigen::Vector4d from_proto(const Vector4d &vector_proto) {
    assert(vector_proto.has_entry_0());
    assert(vector_proto.has_entry_1());
    assert(vector_proto.has_entry_2());
    assert(vector_proto.has_entry_3());

    return (Eigen::Vector4d() << vector_proto.entry_0(),
            vector_proto.entry_1(),
            vector_proto.entry_2(),
            vector_proto.entry_3())
        .finished();
}

Eigen::Matrix4d from_proto(const Matrix4d &matrix_proto) {
    Eigen::Matrix4d result;
    result.row(0) = from_proto(matrix_proto.row_0());
    result.row(1) = from_proto(matrix_proto.row_1());
    result.row(2) = from_proto(matrix_proto.row_2());
    result.row(3) = from_proto(matrix_proto.row_3());
    return result;
}

Vector4d to_proto(const Eigen::Vector4d &vector) {
    Vector4d result;
    result.set_entry_0(vector(0));
    result.set_entry_1(vector(1));
    result.set_entry_2(vector(2));
    result.set_entry_3(vector(3));
    return result;
}

Matrix4d to_proto(const Eigen::Matrix4d &matrix) {
    Matrix4d result;
    *result.mutable_row_0() = to_proto(Eigen::Vector4d(matrix.row(0)));
    *result.mutable_row_1() = to_proto(Eigen::Vector4d(matrix.row(1)));
    *result.mutable_row_2() = to_proto(Eigen::Vector4d(matrix.row(2)));
    *result.mutable_row_3() = to_proto(Eigen::Vector4d(matrix.row(3)));
    return result;
}

MatrixXd to_proto(const Eigen::MatrixXd &matrix) {
    const std::size_t num_rows = matrix.rows();
    const std::size_t num_columns = matrix.cols();

    MatrixXd result;
    result.set_num_rows(num_rows);
    result.set_num_columns(num_columns);
    for (auto row = 0U; row < num_rows; ++row) {
        for (auto column = 0U; column < num_columns; ++column) {
            result.add_entry(matrix(row, column));
        }
    }
    return result;
}

MatrixXcd to_proto(const Eigen::MatrixXcd &matrix) {
    const std::size_t num_rows = matrix.rows();
    const std::size_t num_columns = matrix.cols();

    MatrixXcd result;
    result.set_num_rows(num_rows);
    result.set_num_columns(num_columns);
    for (auto row = 0U; row < num_rows; ++row) {
        for (auto column = 0U; column < num_columns; ++column) {
            *result.add_entry() = to_proto(matrix(row, column));
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

}  // namespace magnon::utils
