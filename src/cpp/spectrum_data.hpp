#ifndef SPECTRUM_DATA_HPP
#define SPECTRUM_DATA_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <Eigen/Core>

#include "utility.hpp"

namespace TopoMagnon {


using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

struct SpectrumData {
  struct Msg {
    std::string label, number;

    std::vector<std::string> irreps;
    std::map<std::string, std::string> irrep_to_k;
    std::map<std::string, int> irrep_to_dim;

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
    int irrep_to_idx(const std::string irrep) const {
      return Utility::find_unique_index(irreps, irrep);
    }

    int k_to_idx(const std::string k) const {
      return Utility::find_unique_index(ks, k);
    }
  } super_msg, sub_msg;

  std::vector<std::string> band_super_irreps, band_sub_irreps;
  IntMatrix comp_rels_matrix;
  std::vector<int> si_orders;
  IntMatrix si_matrix;

  std::map<std::string,
    std::map<std::string, std::vector<std::string>>>
      subk_to_superirrep_to_subirreps;

  std::map<std::string, std::vector<std::string>> superirrep_to_all_subirreps;
};

}

std::istream& operator>>(std::istream& in, TopoMagnon::SpectrumData& s);

#endif // SPECTRUM_DATA_HPP
