#include "physics_and_chemistry.hpp"

#include <string>
#include <utility>
#include <vector>

namespace magnon {

std::vector<std::pair<Physics, Chemistries>> find_physics_and_chemistries_pairs(
    const SpectrumData &data, SupercondChemistries &supercond_chemistries) {
    std::vector<std::pair<Physics, Chemistries>> physics_and_chemistries_pairs;

    std::map<Physics, Chemistries> physics_to_chemistries;

    for (const auto &irrep12wp_decomp_str : data.super_irrep12wp_decomps_of_sxsy) {
        SupercondChemistry supercond_chem = Irrep12wpDecomp(irrep12wp_decomp_str);

        supercond_chemistries.insert(supercond_chem);
        for (const auto &irrep1wp_decomp : supercond_chem.find_all_magnon_irrep1wp_decomps()) {
            std::vector<std::string> kirreps;
            for (const auto &irrep1wp : irrep1wp_decomp.get_comps()) {
                for (const auto &kirrep : data.super_irrep1wp_to_irreps.at(irrep1wp.to_str())) {
                    kirreps.push_back(kirrep);
                }
            }
            physics_to_chemistries[LittleirrepDecomp(kirreps)].insert(irrep1wp_decomp);
        }
    }

    for (const auto &[physics, chemistries] : physics_to_chemistries) {
        physics_and_chemistries_pairs.emplace_back(physics, chemistries);
    }
    std::sort(physics_and_chemistries_pairs.begin(),
              physics_and_chemistries_pairs.end(),
              [](const auto &l, const auto &r) { return *l.second.begin() < *r.second.begin(); });

    return physics_and_chemistries_pairs;
}

}  // namespace magnon
