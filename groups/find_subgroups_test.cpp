#include "groups/find_subgroups.hpp"
#include "gtest/gtest.h"

namespace magnon::groups {

TEST(FindSubgroupsTest, FindTrivialSubGroup) {
    find_subgroups("1.1");
}


}  // namespace magnon::groups
