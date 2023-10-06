#include "common/complex_converter.hpp"

#include "gtest/gtest.h"

namespace magnon::common {

TEST(MatrixXiAsProtoTest, RoundTripTest) {
    const std::complex<double> original{1.5, 2.5};
    const ComplexNumber proto = to_proto(original);
    const std::complex<double> converted = from_proto(proto);
    EXPECT_EQ(original, converted);
}

}  // namespace magnon::common
