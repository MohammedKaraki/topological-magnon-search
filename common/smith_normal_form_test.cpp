#include "common/smith_normal_form.hpp"

#include "Eigen/Dense"

#include "gtest/gtest.h"

namespace magnon::common {

#define RUN_TEST_CASE(m, a_expected)                                         \
    {                                                                        \
        MatrixXl s, a, t;                                                    \
        std::tie(s, a, t) = compute_smith_normal_form(m);                    \
        EXPECT_EQ(a, s *m *t);                                               \
        constexpr double TOLERANCE = 1.0e-5;                                 \
        EXPECT_NEAR(std::abs(s.cast<double>().determinant()), 1, TOLERANCE); \
        EXPECT_NEAR(std::abs(t.cast<double>().determinant()), 1, TOLERANCE); \
        EXPECT_EQ(a, a_expected);                                            \
    }

TEST(SmithNormalFormTest, TestArbitraryCases) {
    {
        const MatrixXl m = (MatrixXl(3, 3) << 5, 1, 5, 1, -1, -1, 3, 8, 2).finished();
        const MatrixXl expected_a = (MatrixXl(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 80).finished();
        RUN_TEST_CASE(m, MatrixXl(expected_a));
    }
    {
        const MatrixXl m =
            (MatrixXl(3, 4) << -20, -17, -5, -5, -16, 3, 13, 16, -4, 21, -3, 15).finished();
        const MatrixXl expected_a =
            (MatrixXl(3, 4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 16, 0).finished();
        RUN_TEST_CASE(m, MatrixXl(expected_a));
    }
}

}  // namespace magnon::common
