#include <algorithm>

#include "spectrum_data.hpp"
#include "entities.hpp"

namespace TopoMagnon {


int SpectrumData::bag_to_idx(const Bag& bag) const
{
  return Utility::find_unique_index(unique_bags, bag);
}

void SpectrumData::Msg::populate_irrep_dims()
{
  std::transform(irreps.begin(),
                 irreps.end(),
                 std::back_inserter(dims),
                 [this](const auto& irrep) {
                   return irrep_to_dim.at(irrep);
                 }
                );

  assert(dims.size() == irreps.size());
  assert(std::all_of(dims.begin(),
                     dims.end(),
                     [](const auto dim) {
                       return dim >= 1;
                     }
                    )
        );
  for (int i = 0; i < static_cast<int>(irreps.size()); ++i) {
    assert(irrep_to_dim.at(irreps[i]) == dims[i]);
  }
}

} // namespace TopoMagnon
