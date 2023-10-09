#pragma once

#include <string>
#include <vector>

#include "groups/group_subgroup_relation.pb.h"

namespace magnon::groups {

// Computes the MSG subgroups induced by external field and/or strain perturbations.
std::vector<magnon::groups::GroupSubgroupRelation> find_subgroups(
    const std::string &msg_number, const MagneticSpaceGroups &magnetic_space_groups);

}  // namespace magnon::groups
