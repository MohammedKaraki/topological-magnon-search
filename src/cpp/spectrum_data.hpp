#ifndef SPECTRUM_DATA_HPP
#define SPECTRUM_DATA_HPP

#include <string>
#include <vector>
#include <map>
#include <Eigen/Core>


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

    std::vector<std::tuple<int, int, std::map<int, int>>>
      k1idx_k2idx_irrep1idxtoirrep2idx_tuples;

    std::map<std::string,
      std::map<std::string,
        std::map<std::string,
          std::vector<std::string>>>>
            k1_to_k2_to_irrep_to_lineirreps;


    IntMatrix make_br_vec(const std::vector<std::string>& br_irreps);
    int sum_dim(const std::vector<std::string>& irreps);
    int irrep_to_idx(const std::string irrep) const;
    int k_to_idx(const std::string k) const;
    void populate_irrep_dims();
  } super_msg, sub_msg;

  std::string wp;
  std::pair<std::string, std::string> magnon_site_irreps;

  std::vector<std::vector<std::string>> super_irrep12wp_decomps_of_sxsy;
  std::map<std::string, std::vector<std::string>> super_irrep1wp_to_irreps;
  std::vector<std::vector<std::string>> super_to_sub;

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

  std::string site_irreps_as_str() const;
  std::string make_br_label() const;
  int subk_idx_to_superk_idx(int subk_idx) const;
  int bag_to_idx(const Bag& bag) const;
};

} // namespace TopoMagnon

std::istream& operator>>(std::istream& in, TopoMagnon::SpectrumData& s);

#endif // SPECTRUM_DATA_HPP
