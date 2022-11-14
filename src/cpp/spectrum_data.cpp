#include <iostream>
#include <algorithm>
#include <sstream>
#include <nlohmann/json.hpp>

#include "spectrum_data.hpp"
#include "entities.hpp"
#include "utility.hpp"
#include "ostream_utility.hpp"

using json = nlohmann::json;

namespace TopoMagnon {

int SpectrumData::Msg::irrep_to_idx(const std::string irrep) const
{
  return Utility::find_unique_index(irreps, irrep);
}

int SpectrumData::Msg::k_to_idx(const std::string k) const
{
  return Utility::find_unique_index(ks, k);
}

std::string SpectrumData::make_br_label() const
{
  std::ostringstream result;

  const auto& [site_irrep1, site_irrep2] = magnon_site_irreps;

  if (site_irrep1 == site_irrep2) {
    result << "2{(" + site_irrep1 + ")}_{" + wp + "}";
  }
  else {
    result << "{(" + site_irrep1 + "+" + site_irrep2 + ")}_{" + wp + "}";
  }

  return result.str();
}

int SpectrumData::subk_idx_to_superk_idx(int subk_idx) const
{
  const auto subk = sub_msg.ks[subk_idx];
  const auto first_superirrep =
    subk_to_superirrep_to_subirreps.at(subk).begin()->first;
  const auto superk = super_msg.irrep_to_k.at(first_superirrep);
  return super_msg.k_to_idx(superk);
}

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



IntMatrix SpectrumData::Msg::make_br_vec(
  const std::vector<std::string>& br_irreps) {
  IntMatrix result(irreps.size(), 1);

  unsigned assert_count = 0;

  for (std::size_t i = 0; i < irreps.size(); ++i) {
    result(i) = std::count(br_irreps.begin(),
                           br_irreps.end(),
                           irreps[i]);
    assert_count += result(i);
  }

  assert(assert_count == br_irreps.size());

  return result;
}

int SpectrumData::Msg::sum_dim(const std::vector<std::string>& irreps)
{
  return std::transform_reduce(irreps.cbegin(),
                               irreps.cend(),
                               0,
                               std::plus<>(),
                               [this](const auto& irrep) {
                                 return irrep_to_dim.at(irrep);
                               }
                              );
}

static IntMatrix construct_matrix(const std::vector<std::vector<int>>& rows)
{
  int num_rows = rows.size();
  int num_cols = rows[0].size();

  for (const auto& row : rows) {
    assert(static_cast<int>(row.size()) == num_cols);
  }

  IntMatrix result(num_rows, num_cols);

  for (int i = 0; i < num_rows; ++i) {
    for (int j = 0; j < num_cols; ++j) {
      result(i, j) = rows[i][j];
    }
  }

  return result;
}

static void from_json(const json& j, SpectrumData& data)
{
  j["wp"].get_to(data.wp);
  j["magnon_site_irreps"].get_to(data.magnon_site_irreps);

  j["super_msg_label"].get_to(data.super_msg.label);
  j["super_msg_number"].get_to(data.super_msg.number);
  j["super_msg_irreps"].get_to(data.super_msg.irreps);
  j["superirrep_to_dim"].get_to(data.super_msg.irrep_to_dim);
  j["superirrep_to_k"].get_to(data.super_msg.irrep_to_k);
  data.super_msg.populate_irrep_dims();

  j["sub_msg_label"].get_to(data.sub_msg.label);
  j["sub_msg_number"].get_to(data.sub_msg.number);
  j["sub_msg_irreps"].get_to(data.sub_msg.irreps);
  j["subirrep_to_dim"].get_to(data.sub_msg.irrep_to_dim);
  j["subirrep_to_k"].get_to(data.sub_msg.irrep_to_k);
  data.sub_msg.populate_irrep_dims();

  j["super_msg_ks"].get_to(data.super_msg.ks);
  j["sub_msg_ks"].get_to(data.sub_msg.ks);

  j["subk_to_g_and_superk"].get_to(data.subk_to_g_and_superk);

  {
    std::vector<std::vector<int>> tmp;
    j["comp_rels_matrix"].get_to(tmp);
    data.comp_rels_matrix = construct_matrix(tmp);
  }

  j["si_orders"].get_to(data.si_orders);

  if (!data.si_orders.empty()) {
    std::vector<std::vector<int>> tmp;
    j["si_matrix"].get_to(tmp);
    data.si_matrix = construct_matrix(tmp);
  }
  assert(static_cast<int>(data.si_orders.size()) == data.si_matrix.rows());

  j["superirrep_to_all_subirreps"].get_to(
    data.superirrep_to_all_subirreps);
  j["subk_to_superirrep_to_subirreps"].get_to(
    data.subk_to_superirrep_to_subirreps);

  j["super_k1_to_k2_to_irrep_to_lineirreps"].get_to(
    data.super_msg.k1_to_k2_to_irrep_to_lineirreps);
  j["sub_k1_to_k2_to_irrep_to_lineirreps"].get_to(
    data.sub_msg.k1_to_k2_to_irrep_to_lineirreps);

  // j["band_super_irreps"].get_to(data.band_super_irreps);
  // for (const auto& superirrep : data.band_super_irreps) {
  //   const auto& subirreps = data.superirrep_to_all_subirreps.at(superirrep);
  //   data.band_sub_irreps.insert(data.band_sub_irreps.end(),
  //                            subirreps.cbegin(),
  //                            subirreps.cend());
  // }
  j["super_irrep12wp_decomps_of_sxsy"].get_to(
    data.super_irrep12wp_decomps_of_sxsy);

  j["super_irrep1wp_to_irreps"].get_to( data.super_irrep1wp_to_irreps);

  j["super_to_sub"].get_to(data.super_to_sub);

  for (const auto& [subk, superirrep_to_subirreps]
       : data.subk_to_superirrep_to_subirreps)
  {
    for (const auto& [superirrep, subirreps] : superirrep_to_subirreps) {
      assert(data.super_msg.irrep_to_dim.at(superirrep)
             == data.sub_msg.sum_dim(subirreps));
    }
  }

  for (const auto& [superirrep, subirreps] : data.superirrep_to_all_subirreps) {
    data.unique_bags.emplace_back(superirrep, data);
  }
  std::sort(data.unique_bags.begin(),
            data.unique_bags.end());
  data.unique_bags.erase(std::unique(data.unique_bags.begin(),
                                     data.unique_bags.end()
                                    ),
                         data.unique_bags.end()
                        );

}

} // namespace TopoMagnon

std::istream& operator>>(std::istream& in, TopoMagnon::SpectrumData& data)
{
  json j;
  in >> j;
  data = j;
  return in;
}
