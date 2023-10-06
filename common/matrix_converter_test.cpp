#include "common/matrix_converter.hpp"

#include "fmt/format.h"
#include "fmt/ostream.h"

#include "gtest/gtest.h"

namespace magnon::common {

TEST(MatrixXiAsProtoTest, RoundTripTest) {
    const Eigen::MatrixXi matrix_original =
        (Eigen::MatrixXi(2, 3) << 11, 12, 13, 21, 22, 23).finished();
    const MatrixXi matrix_proto = to_proto(matrix_original);
    const Eigen::MatrixXi matrix_converted = from_proto(matrix_proto);
    EXPECT_EQ(matrix_original, matrix_converted);
}

TEST(MatrixXdAsProtoTest, RoundTripTest) {
    const Eigen::MatrixXd matrix_original =
        (Eigen::MatrixXd(2, 3) << 1.1, 1.2, 1.3, 2.1, 2.2, 2.3).finished();
    const MatrixXd matrix_proto = to_proto(matrix_original);
    const Eigen::MatrixXd matrix_converted = from_proto(matrix_proto);
    EXPECT_EQ(matrix_original, matrix_converted);
}

TEST(Matrix4dAsProtoTest, RoundTripTest) {
    const Eigen::Matrix4d matrix_original = (Eigen::Matrix4d() << 1.1,
                                             1.2,
                                             1.3,
                                             1.4,
                                             2.1,
                                             2.2,
                                             2.3,
                                             2.4,
                                             3.1,
                                             3.2,
                                             3.3,
                                             3.4,
                                             4.1,
                                             4.2,
                                             4.3,
                                             4.4)
                                                .finished();
    const Matrix4d matrix_proto = to_proto(matrix_original);
    const Eigen::Matrix4d matrix_converted = from_proto(matrix_proto);
    EXPECT_EQ(matrix_original, matrix_converted);
}

TEST(Vector4dAsProtoTest, RoundTripTest) {
    const Eigen::Vector4d vector_original = (Eigen::Vector4d() << 1.1, 1.2, 1.3, 1.4).finished();
    const Vector4d vector_proto = to_proto(vector_original);
    const Eigen::Vector4d vector_converted = from_proto(vector_proto);
    EXPECT_EQ(vector_original, vector_converted);
}

}  // namespace magnon::common
