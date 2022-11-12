#ifndef SPECTRUM_DATA_HPP
#define SPECTRUM_DATA_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Eigen/Core>

#include "utility.hpp"
#include "ostream_utility.hpp"

namespace TopoMagnon {

class Bag;

using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

struct SpectrumData {
  struct Msg {
    std::string label, number;

    std::vector<std::string> irreps;
    std::map<std::string, std::string> irrep_to_k;
    std::map<std::string, int> irrep_to_dim;
    std::vector<int> dims;

    std::vector<std::string> ks;
    std::map<std::string, std::string> k_to_coords;

    std::map<std::string,
      std::map<std::string,
        std::map<std::string,
          std::vector<std::string>>>>
            k1_to_k2_to_irrep_to_lineirreps;

    IntMatrix make_br_vec(const std::vector<std::string>& br_irreps);
    int sum_dim(const std::vector<std::string>& irreps);

  public:
    int irrep_to_idx(const std::string irrep) const
    {
      return Utility::find_unique_index(irreps, irrep);
    }

    int k_to_idx(const std::string k) const
    {
      return Utility::find_unique_index(ks, k);
    }

    void populate_irrep_dims();
  } super_msg, sub_msg;

  std::vector<std::string> band_super_irreps, band_sub_irreps;
  IntMatrix comp_rels_matrix;
  std::vector<int> si_orders;
  IntMatrix si_matrix;
  std::map<std::string,
    std::map<std::string, std::vector<std::string>>>
      subk_to_superirrep_to_subirreps;

  std::map<std::string, std::vector<std::string>> superirrep_to_all_subirreps;

  std::map<std::string, std::pair<std::string, std::string>>
    subk_to_g_and_superk;

  std::vector<Bag> unique_bags;

public:
    int subk_idx_to_superk_idx(int subk_idx) const
    {
      const auto subk = sub_msg.ks[subk_idx];
      const auto first_superirrep =
        subk_to_superirrep_to_subirreps.at(subk).begin()->first;
      const auto superk = super_msg.irrep_to_k.at(first_superirrep);
      return super_msg.k_to_idx(superk);
    }


  int bag_to_idx(const Bag& bag) const;

};

} // namespace TopoMagnon

std::istream& operator>>(std::istream& in, TopoMagnon::SpectrumData& s);

#endif // SPECTRUM_DATA_HPP
