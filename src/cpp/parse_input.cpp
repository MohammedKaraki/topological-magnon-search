#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <map>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <Eigen/Core>

#include "spectrum_data.hpp"

using json = nlohmann::json;

namespace TopoMagnon {

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
  j["super_msg_label"].get_to(data.super_msg.label);
  j["super_msg_number"].get_to(data.super_msg.number);
  j["super_msg_irreps"].get_to(data.super_msg.irreps);
  j["superirrep_to_dim"].get_to(data.super_msg.irrep_to_dim);

  j["sub_msg_label"].get_to(data.sub_msg.label);
  j["sub_msg_number"].get_to(data.sub_msg.number);
  j["sub_msg_irreps"].get_to(data.sub_msg.irreps);
  j["subirrep_to_dim"].get_to(data.sub_msg.irrep_to_dim);

  j["super_msg_ks"].get_to(data.super_msg.ks);
  j["sub_msg_ks"].get_to(data.sub_msg.ks);

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

  j["band_super_irreps"].get_to(data.band_super_irreps);

  for (const auto& superirrep : data.band_super_irreps) {
    const auto& subirreps = data.superirrep_to_all_subirreps.at(superirrep);
    data.band_sub_irreps.insert(data.band_sub_irreps.end(),
                             subirreps.cbegin(),
                             subirreps.cend());
  }

  for (const auto& [subk, superirrep_to_subirreps]
       : data.subk_to_superirrep_to_subirreps)
  {
    for (const auto& [superirrep, subirreps] : superirrep_to_subirreps) {
      assert(data.super_msg.irrep_to_dim.at(superirrep)
             == data.sub_msg.sum_dim(subirreps));
    }
  }

}

} // namespace

std::istream& operator>>(std::istream& in, TopoMagnon::SpectrumData& data)
{
  json j;
  in >> j;
  data = j;
  return in;
}
