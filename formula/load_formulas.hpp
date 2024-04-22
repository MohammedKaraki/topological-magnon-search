#pragma once

#include "formula/formula.pb.h"
#include "groups/magnetic_space_group.pb.h"

namespace magnon::formula {

void replace_formula(groups::MagneticSpaceGroup &group, const formula::Formulas &formulas);
void replace_formula(groups::MagneticSpaceGroup &group,
                     const formula::MsgToFormulas &msg_to_formulas);

}  // namespace magnon::formula
