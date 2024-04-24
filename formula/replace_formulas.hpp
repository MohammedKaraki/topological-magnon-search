#pragma once

#include "formula/formula.pb.h"
#include "groups/magnetic_space_group.pb.h"

namespace magnon::formula {

void replace_formula(groups::MagneticSpaceGroup &group, const Formulas &formulas);
bool maybe_replace_formula(groups::MagneticSpaceGroup &group, const MsgToFormulas &msg_to_formulas);
bool maybe_replace_formula(groups::MagneticSpaceGroup &group);

}  // namespace magnon::formula
