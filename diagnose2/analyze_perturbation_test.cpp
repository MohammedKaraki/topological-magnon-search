#include "diagnose2/analyze_perturbation.hpp"

#include <cassert>

#include "google/protobuf/util/message_differencer.h"
#include "gtest/gtest.h"
#include "utils/proto_text_format.hpp"

namespace magnon::utils {

constexpr const char *RESULT_PATH =
    "diagnose2/test_data/analyze_perturbation_result_205_33_4a_2_4.txtpb";
constexpr const char *PROCESSED_TABLES_PATH =
    "diagnose2/test_data/processed_tables_205_33_4a_2_4.txtpb";

TEST(AnalyzePerturbationTest, RegressionTestCase) {
    const auto structure = []() {
        magnon::diagnose2::PerturbedBandStructure structure{};
        assert(magnon::utils::proto::read_from_text_file(PROCESSED_TABLES_PATH, structure));
        return structure;
    }();
    const auto expected_result = []() {
        magnon::diagnose2::SearchResult result{};
        assert(magnon::utils::proto::read_from_text_file(RESULT_PATH, result));
        return result;
    }();

    const auto result = [&structure]() {
        auto result = magnon::diagnose2::analyze_perturbation(structure);
        result.clear_metadata();
        return result;
    }();

    assert(result.supergroup_number() == "205.33");
    assert(result.subgroup_number() == "2.4");
    assert(expected_result.supergroup_number() == "205.33");
    assert(expected_result.subgroup_number() == "2.4");
    EXPECT_TRUE(::google::protobuf::util::MessageDifferencer::Equals(result, expected_result));
}

}  // namespace magnon::utils
