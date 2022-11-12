#include <vector>
#include <set>
#include <string>
#include <utility>
#include <tuple>
#include <numeric>
#include <cassert>

#include "utility.hpp"
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

void Superband::populate_subband(Subband& subband, const SpectrumData& data)
{
  subband.subk_idx_to_e_idx_to_submode.resize(data.sub_msg.ks.size());

  for (auto& e_idx_to_submode : subband.subk_idx_to_e_idx_to_submode) {
    e_idx_to_submode.clear();
  }

  Vector8<Vector16<int>> subk_idx_to_span_sizes( data.sub_msg.ks.size());

  for (const auto& e_idx_to_supermode : this->k_idx_to_e_idx_to_supermode) {
    for (const auto& supermode : e_idx_to_supermode) {
      const auto bag_idx = supermode.bag_idx;
      const auto& bag = data.unique_bags[bag_idx];

      for (auto& span_sizes : subk_idx_to_span_sizes)
      {
        span_sizes.push_back(0);
      }

      for (const auto& [subk_idx, subirrep_idx]
           : bag.subk_idx_and_subirrep_idx_pairs)
      {
        subband.subk_idx_to_e_idx_to_submode[subk_idx].emplace_back(
          subirrep_idx);

        ++subk_idx_to_span_sizes[subk_idx].back();
      }

      for (auto& span_sizes : subk_idx_to_span_sizes)
      {
        if (span_sizes.back() == 0) {
          span_sizes.pop_back();
        }
      }

    }
  }

  subband.all_submode_spans.clear();

  for (int subk_idx = 0;
       subk_idx < static_cast<int>(subk_idx_to_span_sizes.size());
       ++subk_idx)
  {
    auto next_begin_it = subband.subk_idx_to_e_idx_to_submode[subk_idx].begin();
    for (const int span_size : subk_idx_to_span_sizes[subk_idx]) {
      assert(span_size >= 1);

      auto next_end_it = next_begin_it + span_size;
      if (span_size >= 2) {
        subband.all_submode_spans.push_back(std::span(next_begin_it,
                                                      next_end_it
                                                     )
                                           );
      }
      next_begin_it = next_end_it;
    }
  }
}


Vector32<short> Subband::make_br(
  const Vector8<int>& e_idxs_beg,
  const Vector8<int>& e_idxs_end,
  const SpectrumData& data
  )
{
  Vector32<short> result(data.sub_msg.irreps.size(), 0);

  for (unsigned i = 0; i < e_idxs_beg.size(); ++i) {
    for (int j = e_idxs_beg[i]; j < e_idxs_end[i]; ++j) {
      ++result[subk_idx_to_e_idx_to_submode[i][j].subirrep_idx];
    }
  }

  return result;
}


Vector4<Vector8<int>>
Subband::dimvalid_e_idxs(const SpectrumData& data)
{
  Vector4<Vector8<int>> result;

  const auto num_ks = subk_idx_to_e_idx_to_submode.size();

  Vector8<int> k_idx_to_accum(num_ks, 0);
  Vector8<int> k_idx_to_cur_e_idx(num_ks, 0);

  result.push_back(k_idx_to_cur_e_idx);

  auto min_accum_it = k_idx_to_accum.begin();
  auto max_accum_it = min_accum_it;
  int min_accum_k_idx = 0;

  while (k_idx_to_cur_e_idx[min_accum_k_idx] < static_cast<int>(
      subk_idx_to_e_idx_to_submode[min_accum_k_idx].size())
        )
  {
    *min_accum_it += data.sub_msg.dims[
        subk_idx_to_e_idx_to_submode[
          min_accum_k_idx][
            k_idx_to_cur_e_idx[min_accum_k_idx]++
          ].subirrep_idx
    ];

    std::tie(min_accum_it, max_accum_it) = std::minmax_element(
      k_idx_to_accum.begin(),
      k_idx_to_accum.end()
      );
    min_accum_k_idx = std::distance(k_idx_to_accum.begin(), min_accum_it);

    if (*min_accum_it == *max_accum_it) {
      result.push_back(k_idx_to_cur_e_idx);
    }
  }

  return result;
}


std::ostream& operator<<(std::ostream& out, const Bag& b)
{
  return out << b.subk_idx_and_subirrep_idx_pairs;
}


} // namespace TopoMagnon
