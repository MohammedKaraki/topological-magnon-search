#pragma once

#include <vector>

#include "spectrum_data.hpp"

namespace TopoMagnon {

std::vector<int> k_idxs_path(const SpectrumData::Msg &msg, bool all_edges);

void complement_subk_idxs(std::vector<int> &subk_idxs, const SpectrumData::Msg &msg);

}  // namespace TopoMagnon
