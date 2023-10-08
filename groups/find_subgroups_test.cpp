#include "groups/find_subgroups.hpp"

#include "google/protobuf/text_format.h"
#include "groups/read_standard_magnetic_space_groups.hpp"
#include "gtest/gtest.h"

namespace magnon::groups {

const auto standard_msgs = read_standard_magnetic_space_groups_from_disk();

TEST(FindSubgroupsTest, FindTrivialSubGroup) {
    const char group_number[] = "2.4";
    const auto subgroups = find_subgroups(group_number, standard_msgs);

    EXPECT_TRUE(subgroups.empty());
}

TEST(FindSubgroupsTest, FindNontrivialSubGroups) {
    const char group_number[] = "205.33";
    const auto subgroups = find_subgroups(group_number, standard_msgs);

    ASSERT_EQ(subgroups.size(), 10U);

    for (const auto &subgroup : subgroups) {
        std::string output;
        ::google::protobuf::TextFormat::PrintToString(subgroup, &output);
        std::cout << output << '\n';
        EXPECT_EQ(subgroup.supergroup_number(), group_number);
    }

    ASSERT_EQ(subgroups[3].unbroken_standard_general_positions().general_position_size(), 3U);
    EXPECT_EQ(
        subgroups[3].unbroken_standard_general_positions().general_position(0).coordinates_form(),
        "x,y,z,+1");
    EXPECT_EQ(
        subgroups[3].unbroken_standard_general_positions().general_position(1).coordinates_form(),
        "z,x,y,+1");
    EXPECT_EQ(
        subgroups[3].unbroken_standard_general_positions().general_position(2).coordinates_form(),
        "y,z,x,+1");
    ASSERT_EQ(subgroups[3].perturbation_prescription_size(), 1U);
    EXPECT_EQ(subgroups[3].perturbation_prescription(0), "E parallel [111]");
}

}  // namespace magnon::groups
