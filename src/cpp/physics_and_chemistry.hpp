#ifndef PHYSICS_AND_CHEMISTRY_HPP
#define PHYSICS_AND_CHEMISTRY_HPP

#include "spectrum_data.hpp"
#include "latexify.hpp"

namespace TopoMagnon {

using Physics = LittleirrepDecomp;
using Chemistry = Irrep1wpDecomp;
using Chemistries = std::set<Chemistry>;
using SupercondChemistry = Irrep12wpDecomp;
using SupercondChemistries = std::set<SupercondChemistry>;


std::vector<std::pair<Physics, Chemistries>>
find_physics_and_chemistries_pairs(const SpectrumData& data,
                                   SupercondChemistries& supercond_chemistries);

} // namespace TopoMagnon

#endif // PHYSICS_AND_CHEMISTRY_HPP
