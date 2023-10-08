#pragma once

#include <string>
#include <vector>

#include "groups/induced_magnetic_space_group.pb.h"

namespace magnon::groups {

// Computes the MSG subgroups induced by external field and/or strain perturbations.
std::vector<magnon::groups::InducedMagneticSpaceGroup> find_subgroups(
    const std::string &msg_number);

}  // namespace magnon::groups
