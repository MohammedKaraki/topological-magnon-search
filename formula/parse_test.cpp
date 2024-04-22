#include "formula/parse.hpp"

#include "gtest/gtest.h"

namespace magnon::formula {

TEST(ParseTest, InvalidFormula1) {
    constexpr char INVALID_FORMULA[] = "";
    const auto maybe_terms = parse_formula(INVALID_FORMULA);
    EXPECT_EQ(maybe_terms.has_value(), false);
}

TEST(ParseTest, ValidFormula1) {
    constexpr char VALID_FORMULA[] = "n(GM^{+})";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{1, "GM^{+}"}};
    EXPECT_EQ(terms, expected_terms);
}

TEST(ParseTest, ValidFormula2) {
    constexpr char VALID_FORMULA[] = "-n(GM^{+})";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{-1, "GM^{+}"}};
    EXPECT_EQ(terms, expected_terms);
}

TEST(ParseTest, ValidFormula3) {
    constexpr char VALID_FORMULA[] = "n(A)+n(B)";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{1, "A"}, Term{1, "B"}};
    EXPECT_EQ(terms, expected_terms);
}

TEST(ParseTest, ValidFormula4) {
    constexpr char VALID_FORMULA[] = "-n(A)-1n(B)";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{-1, "A"}, Term{-1, "B"}};
    EXPECT_EQ(terms, expected_terms);
}

TEST(ParseTest, ValidFormula5) {
    constexpr char VALID_FORMULA[] = "n(A)+1n(B)";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{1, "A"}, Term{1, "B"}};
    EXPECT_EQ(terms, expected_terms);
}

TEST(ParseTest, ValidFormula6) {
    constexpr char VALID_FORMULA[] = "-9n(A)+11n(B)";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{-9, "A"}, Term{11, "B"}};
    EXPECT_EQ(terms, expected_terms);
}

TEST(ParseTest, ValidFormula7) {
    constexpr char VALID_FORMULA[] = "-9n(A)-0n(B)-n(C)";
    const auto maybe_terms = parse_formula(VALID_FORMULA);
    ASSERT_EQ(maybe_terms.has_value(), true);
    const auto &terms = maybe_terms.value();

    const Terms expected_terms{Term{-9, "A"}, Term{0, "B"}, Term{-1, "C"}};
    EXPECT_EQ(terms, expected_terms);
}

}  // namespace magnon::formula
