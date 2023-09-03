#pragma once

#include <set>
#include <vector>

#include "irreps.hpp"
#include "spectrum_data.hpp"

namespace magnon {

using Physics = LittleirrepDecomp;
using Chemistry = Irrep1wpDecomp;
using Chemistries = std::set<Chemistry>;
using SupercondChemistry = Irrep12wpDecomp;
using SupercondChemistries = std::set<SupercondChemistry>;

std::vector<std::pair<Physics, Chemistries>> find_physics_and_chemistries_pairs(
    const SpectrumData &data, SupercondChemistries &supercond_chemistries);

}  // namespace magnon
