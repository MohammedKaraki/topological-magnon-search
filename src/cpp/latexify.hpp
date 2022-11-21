#ifndef LATEXIFY_HPP
#define LATEXIFY_HPP

#include <string>
#include <vector>

#include "spectrum_data.hpp"
#include "physics_and_chemistry.hpp"

namespace TopoMagnon {

std::string latexify_greeks(const std::string& label);
std::string latexify_sis(const SpectrumData& data);
std::string latexify_comp_rels(const SpectrumData& data);

std::string latexify_supercond_chemistries(
  const SpectrumData& data,
  const SupercondChemistries& supercond_chemistries
  );
std::string latexify_physics_and_chemistries_pairs(
  const std::vector<std::pair<Physics, Chemistries>>& pairs
  );

class LatexDoc {
public:
  LatexDoc& operator<<(const std::string& code_str) {
    code << code_str;
    return *this;
  }

  void dump(const std::string& filename);

private:
  std::ostringstream code;
};

} // namespace TopoMagnon

#endif // LATEXIFY_HPP
