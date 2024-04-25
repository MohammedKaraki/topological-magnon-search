#pragma once

#include "diagnose2/perturbed_band_structure.pb.h"
#include "formula/formula.pb.h"
#include "groups/magnetic_space_group.pb.h"

namespace magnon::formula {

void replace_formula(groups::MagneticSpaceGroup &group, const Formulas &formulas);
bool maybe_replace_formula(groups::MagneticSpaceGroup &group, const MsgToFormulas &msg_to_formulas);
bool maybe_replace_formula(groups::MagneticSpaceGroup &group);

groups::MagneticSpaceGroup maybe_with_alternative_si_formulas(groups::MagneticSpaceGroup group);
diagnose2::PerturbedBandStructure maybe_with_alternative_si_formulas(
    diagnose2::PerturbedBandStructure perturbation);

}  // namespace magnon::formula
