#include <vector>
#include <set>
#include <string>
#include <utility>
#include <cassert>

#include "spectrum_data.hpp"
#include "entities.hpp"
#include "ostream_utility.hpp"



namespace TopoMagnon {


Bag::Bag(const std::string& superirrep, const SpectrumData& data)
{
  const auto& subirreps = data.superirrep_to_all_subirreps.at(superirrep);

  for (const auto& subirrep : subirreps) {

    const auto subk = data.sub_msg.irrep_to_k.at(subirrep);
    const auto subk_idx = data.sub_msg.k_to_idx(subk);
    const auto subirrep_idx = data.sub_msg.irrep_to_idx(subirrep);

    subk_idx_and_subirrep_idx_pairs.emplace_back(
      std::pair{subk_idx, subirrep_idx}
      );
  }

  std::sort(subk_idx_and_subirrep_idx_pairs.begin(),
            subk_idx_and_subirrep_idx_pairs.end()
           );
}

Supermode::Supermode(const std::string& superirrep,
                     const SpectrumData& data)
{
  superirrep_idx = data.super_msg.irrep_to_idx(superirrep);
  bag_idx = data.bag_to_idx(Bag(superirrep, data));
}

const Bag& Supermode::get_bag(const SpectrumData& data) const
{
  return data.unique_bags[bag_idx];
}

Superband::Superband(const std::vector<std::string>& superirreps,
                     const SpectrumData& data)
{
  k_idx_to_e_idx_to_supermode.resize(data.super_msg.ks.size());

  for (const auto& superirrep : superirreps) {
    const auto k = data.super_msg.irrep_to_k.at(superirrep);
    const auto k_idx = data.super_msg.k_to_idx(k);

    k_idx_to_e_idx_to_supermode[k_idx].push_back(Supermode(superirrep, data));

    std::sort(k_idx_to_e_idx_to_supermode[k_idx].begin(),
              k_idx_to_e_idx_to_supermode[k_idx].end()
             );
  }
}

std::ostream& operator<<(std::ostream& out, const Bag& b)
{
  return out << b.subk_idx_and_subirrep_idx_pairs;
}

} // namespace TopoMagnon
