#pragma once

#include <map>
#include <span>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "Eigen/Core"

#include "utility.hpp"
#include "utility/comparable.hpp"

namespace magnon {

class Bag;

struct SpectrumData {
    struct Msg {
        std::string label, number;

        std::vector<std::string> irreps;
        std::map<std::string, std::string> irrep_to_k;
        std::map<std::string, int> irrep_to_dim;
        std::vector<int> dims;
        std::vector<int> irrepidx_to_kidx;

        std::vector<std::string> ks;
        std::vector<std::string> kcoords;

        std::vector<std::tuple<int, int, std::map<int, int>>>
            k1idx_k2idx_irrep1idxtoirrep2idx_tuples;

        std::map<std::string,
                 std::map<std::string, std::map<std::string, std::vector<std::string>>>>
            k1_to_k2_to_irrep_to_lineirreps;

        IntMatrix make_br_vec(const std::vector<std::string> &br_irreps);
        int sum_dim(const std::vector<std::string> &irreps);
        int irrep_to_idx(const std::string irrep) const;
        int k_to_idx(const std::string k) const;
        void populate_irrep_dims();
    } super_msg, sub_msg;

    std::string wp;
    std::pair<std::string, std::string> magnon_site_irreps;
    std::pair<std::string, std::string> pos_neg_siteirreps;
    std::pair<std::vector<std::string>, std::vector<std::string>> pos_neg_magnonirreps;

    std::vector<std::vector<std::string>> super_irrep12wp_decomps_of_sxsy;
    std::map<std::string, std::vector<std::string>> super_irrep1wp_to_irreps;
    std::vector<std::vector<std::string>> super_to_sub;
    std::vector<std::string> presc;

    IntMatrix comp_rels_matrix;
    std::vector<int> si_orders;
    IntMatrix si_matrix;
    std::map<std::string, std::map<std::string, std::vector<std::string>>>
        subk_to_superirrep_to_subirreps;

    std::map<std::string, std::vector<std::string>> superirrep_to_all_subirreps;

    std::map<std::string, std::pair<std::string, std::string>> subk_to_g_and_superk;

    std::vector<Bag> unique_bags;

    std::string firstsiteirrep_and_wp_as_strkey() const;
    std::string site_irreps_as_str() const;
    std::string make_br_label() const;
    int subk_idx_to_superk_idx(int subk_idx) const;
    int bag_to_idx(const Bag &bag) const;
    std::string si_orders_to_latex() const;
};

}  // namespace magnon

std::istream &operator>>(std::istream &in, magnon::SpectrumData &s);

namespace magnon {

struct SpectrumData;

// Decomposition of an irrep of supergroup into irreps of subgroup at all
// subgroup-distinct momenta belonging to the supergroup momentum star
class Bag {
 public:
    Bag(const std::string &superirrep, const SpectrumData &data);

    friend std::ostream &operator<<(std::ostream &out, const Bag &b);

    enum { invalid_idx = -1 };
    auto field_refs() const { return std::tie(subk_idx_and_subirrep_idx_pairs); }
    MAKE_COMPARABLE();

 public:
    std::vector<std::pair<int, int>> subk_idx_and_subirrep_idx_pairs;
};

// Use integer *references* to superirrep and bag, rather than the objects
// themselves.
// This design is crucial for optimization purposes, as the structure
// and its comparison operator live in a very tight loop in the main algorithm.
class Supermode {
 public:
    Supermode(const std::string &superirrep, const SpectrumData &data);

    const Bag &get_bag(const SpectrumData &data) const;

    bool operator<(const Supermode &rhs) const { return this->bag_idx < rhs.bag_idx; }

    friend std::ostream &operator<<(std::ostream &out, const Supermode &supermode);

 public:
    int superirrep_idx;
    int bag_idx;  // Can be inferred from superirrep_idx, but used here
                  // to speed up operator<() which lives in a tight loop
};

class Submode {
 public:
    Submode(int subirrep_idx) : subirrep_idx{subirrep_idx} {}

    auto field_refs() const { return std::tie(subirrep_idx); }
    MAKE_COMPARABLE();

    friend std::ostream &operator<<(std::ostream &out, const Submode &submode);

 public:
    int subirrep_idx;
};

using Span = std::span<Submode>;
using Spans = Vector<std::span<Submode>>;

class Subband {
 public:
    Subband(const SpectrumData &data) : data{data} {}

 public:
    Vector4<Vector8<int>> dimvalid_e_idxs();

    Vector32<short> make_br(const Vector8<int> &e_idxs_beg,
                            const Vector8<int> &e_idxs_end,
                            const SpectrumData &data);

    std::map<int, std::pair<bool, IntMatrix>> calc_gap_sis() const;

    bool next_energetics();
    bool satisfies_antiunit_rels() const;
    void fix_antiunit_rels();

    int get_num_bands() const {
        assert(num_bands >= 1);
        return num_bands;
    };

 public:
    Vector8<Vector16<Submode>> subk_idx_to_e_idx_to_submode;

    Vector<std::tuple<Vector<int>, Vector<Span>, bool>> gaps_allspanstopermute_done_tuples;

    Vector<int> all_possible_gaps;

 private:
    const SpectrumData &data;
    int num_bands = -1;
    friend class Superband;
};

class Superband {
 public:
    Superband(const std::vector<std::string> &superirreps, const SpectrumData &data);

    friend std::ostream &operator<<(std::ostream &out, const Superband &b);
    bool cartesian_permute();
    bool satisfies_antiunit_rels() const;
    void fix_antiunit_rels();

    Subband make_subband() const;

 public:
    std::vector<std::vector<Supermode>> k_idx_to_e_idx_to_supermode;
    const SpectrumData &data;
};

}  // namespace magnon
