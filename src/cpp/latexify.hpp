#ifndef LATEXIFY_HPP
#define LATEXIFY_HPP

#include <string>
#include <vector>

#include "spectrum_data.hpp"
#include "physics_and_chemistry.hpp"

namespace TopoMagnon {

std::string latexify_greeks(const std::string& label);
std::string latexify_row(const IntMatrix& ints,
                         const std::vector<std::string>& strs);
std::string latexify_matrix(const IntMatrix& matrix,
                            const std::vector<std::string>& strs,
                            bool sis);

std::string latexify_supercond_chemistries(
  const SpectrumData& data,
  const SupercondChemistries& supercond_chemistries
  );
std::string latexify_physics_and_chemistries_pairs(
  const std::vector<std::pair<Physics, Chemistries>>& pairs
  );


} // namespace TopoMagnon

#endif // LATEXIFY_HPP
