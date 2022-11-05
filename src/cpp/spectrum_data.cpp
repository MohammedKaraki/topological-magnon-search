#include "spectrum_data.hpp"
#include "entities.hpp"

namespace TopoMagnon {


int SpectrumData::bag_to_idx(const Bag& bag) const
{
  return Utility::find_unique_index(unique_bags, bag);
}


} // namespace TopoMagnon
