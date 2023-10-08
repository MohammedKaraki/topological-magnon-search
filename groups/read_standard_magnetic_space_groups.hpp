#pragma once

#include <string>

#include "groups/magnetic_space_group.pb.h"

namespace magnon::groups {

MagneticSpaceGroups read_standard_magnetic_space_groups_from_disk();

}
