#pragma once

#include <vector>

#include "diagnose2/spectrum_data.hpp"

namespace magnon {

std::vector<int> make_kpath_indices(const diagnose2::SpectrumData::Msg &msg, bool all_edges);

void complement_kpath_indices(std::vector<int> &subk_idxs, const diagnose2::SpectrumData::Msg &msg);

}  // namespace magnon
