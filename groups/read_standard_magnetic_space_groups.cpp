#include "read_standard_magnetic_space_groups.hpp"

#include "utils/proto_text_format.hpp"

namespace magnon::groups {

MagneticSpaceGroups read_standard_magnetic_space_groups_from_disk() {
    MagneticSpaceGroups groups{};
    magnon::utils::proto::read_from_text_file("data/standard_magnetic_space_groups.txtpb", groups);
    return groups;
}

}  // namespace magnon::groups
