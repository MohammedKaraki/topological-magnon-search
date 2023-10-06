#include "common/matrix_converter.hpp"

#include "fmt/format.h"
#include "fmt/ostream.h"

#include "gtest/gtest.h"

namespace magnon::common {

TEST(MatrixAsProtoTest, RoundTripTest) {
    const Eigen::MatrixXi matrix_original =
        (Eigen::MatrixXi(2, 3) << 11, 12, 13, 21, 21, 22).finished();
    const MatrixXi matrix_proto = to_proto(matrix_original);
    const Eigen::MatrixXi matrix_converted = from_proto(matrix_proto);
    EXPECT_EQ(matrix_original, matrix_converted);
}

}  // namespace magnon::common
