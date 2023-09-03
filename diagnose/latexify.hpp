#pragma once

#include <string>
#include <vector>

#include "physics_and_chemistry.hpp"
#include "spectrum_data.hpp"

namespace magnon {

std::string latexify_greeks(const std::string &label);
std::string latexify_sis(const SpectrumData &data);
std::string latexify_comp_rels(const SpectrumData &data);
std::string latexify_super_to_sub_axis(const SpectrumData &data, int axis_idx);
std::string latexify_super_to_sub(const SpectrumData &data);
std::string latexify_super_to_sub_v2(const SpectrumData &data);
std::string latexify_korirrep(std::string label);
std::string latexify_gkcoords(std::string g, std::string k, std::string coords);
std::string latexify_irrepsum(const std::vector<std::string> &irreps);

std::string latexify_supercond_chemistries(const SpectrumData &data,
                                           const SupercondChemistries &supercond_chemistries);
std::string latexify_physics_and_chemistries_pairs(
    const std::vector<std::pair<Physics, Chemistries>> &pairs);

class LatexDoc {
 public:
    LatexDoc &operator<<(const std::string &code_str) {
        code << code_str;
        return *this;
    }

    void describe_si_formulas(const SpectrumData &data);
    void describe_sc_chems(const SpectrumData &data);
    void dump(const std::string &filename, bool standalone = true);

 private:
    std::ostringstream code;
};

}  // namespace magnon
