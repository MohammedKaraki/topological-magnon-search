#include <vector>
#include <set>
#include <string>
#include <utility>
#include <cassert>
#include <fmt/format.h>

#include "spectrum_data.hpp"
#include "entities.hpp"
#include "ostream_utility.hpp"



namespace TopoMagnon {

Supermode::Supermode(const std::string& superirrep,
                     const SpectrumData& data)
{
  superirrep_idx = data.super_msg.irrep_to_idx(superirrep);
  bag_idx = data.bag_to_idx(
    Bag(data.superirrep_to_all_subirreps.at(superirrep))
    );
}

const Bag& Supermode::get_bag(const SpectrumData& data) const
{
  return data.unique_bags[bag_idx];
}

Superband::Superband(const std::vector<std::string>& superirreps,
                     const SpectrumData& data)
{
  fixedk_supermodes.resize(data.super_msg.ks.size());

  for (const auto& superirrep : superirreps) {
    const auto k = data.super_msg.irrep_to_k.at(superirrep);
    const auto k_idx = data.super_msg.k_to_idx(k);
    fixedk_supermodes[k_idx].energy_idx_to_supermode.push_back(
      Supermode(superirrep, data)
      );
    std::sort(fixedk_supermodes[k_idx].energy_idx_to_supermode.begin(),
              fixedk_supermodes[k_idx].energy_idx_to_supermode.end()
             );
  }
}

std::ostream& operator<<(std::ostream& out, const Bag& b)
{
  return out << b.as_sorted_vector();
}

std::ostream& operator<<(std::ostream& out, const FixedkSupermodes& f)
{
  return out << f.energy_idx_to_supermode;
}

} // namespace TopoMagnon
