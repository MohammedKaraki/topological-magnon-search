// #ifndef ENTITIES_HPP
// #define ENTITIES_HPP
//
// #include <vector>
// #include <string>
// #include <utility>
// #include <tuple>
// #include <span>
// #include <map>
// #include <Eigen/Core>
//
// #include "utility.hpp"
//
// namespace TopoMagnon {
//
// struct SpectrumData;
//
//
// // Decomposition of an irrep of supergroup into irreps of subgroup at all
// // subgroup-distinct momenta belonging to the supergroup momentum star
// class Bag {
// public:
//
//   Bag(const std::string& superirrep,
//       const SpectrumData& data);
//
//   int operator<=>(const Bag& rhs) const{
//       assert(false);
//   }
//
//   friend std::ostream& operator<<(std::ostream& out, const Bag& b);
//
//   enum {invalid_idx = -1};
// public:
//   std::vector<std::pair<int, int>> subk_idx_and_subirrep_idx_pairs;
// };
//
// // Use integer *references* to superirrep and bag, rather than the objects
// // themselves.
// // This design is crucial for optimization purposes, as the structure
// // and its comparison operator live in a very tight loop in the main algorithm.
// class Supermode {
// public:
//
//   Supermode(const std::string& superirrep,
//             const SpectrumData& data);
//
//   const Bag& get_bag(const SpectrumData& data) const;
//
//   bool operator<(const Supermode& rhs) const {
//     return this->bag_idx < rhs.bag_idx;
//   }
//
//   friend std::ostream& operator<<(std::ostream& out,
//                                   const Supermode& supermode);
//
// public:
//   int superirrep_idx;
//   int bag_idx; // Can be inferred from superirrep_idx, but used here
//                // to speed up operator<() which lives in a tight loop
// };
//
// class Submode {
// public:
//   Submode(int subirrep_idx) : subirrep_idx{subirrep_idx}
//   { }
//
//   int operator<=>(const Submode& rhs) const{
//       assert(false);
//   }
//
//   bool operator!=(const Submode& rhs) const{
//       assert(false);
//   }
//
//   friend std::ostream& operator<<(std::ostream& out, const Submode& submode);
//
// public:
//   int subirrep_idx;
// };
//
//
// using Span = std::span<Submode>;
// using Spans = Vector<std::span<Submode>>;
//
//
// class Subband {
// public:
//   Subband(const SpectrumData& data) : data{data}
//   {
//   }
//
// public:
//   Vector4<Vector8<int>> dimvalid_e_idxs();
//
//   Vector32<short> make_br(
//     const Vector8<int>& e_idxs_beg,
//     const Vector8<int>& e_idxs_end,
//     const SpectrumData& data
//     );
//
//   std::map<int, std::pair<bool, IntMatrix>>
//     calc_gap_sis() const;
//
//   bool next_energetics();
//   bool satisfies_antiunit_rels() const;
//   void fix_antiunit_rels();
//
//   int get_num_bands() const { assert(num_bands >= 1); return num_bands; };
//
// public:
//   Vector8<Vector16<Submode>> subk_idx_to_e_idx_to_submode;
//
//   Vector<std::tuple<Vector<int>, Vector<Span>, bool>>
//     gaps_allspanstopermute_done_tuples;
//
//   Vector<int> all_possible_gaps;
// private:
//   const SpectrumData& data;
//   int num_bands = -1;
//   friend class Superband;
// };
//
//
// class Superband {
// public:
//
//   Superband(const std::vector<std::string>& superirreps,
//             const SpectrumData& data);
//
//   friend std::ostream& operator<<(std::ostream& out, const Superband& b);
//   bool cartesian_permute();
//   bool satisfies_antiunit_rels() const;
//   void fix_antiunit_rels();
//
//   Subband make_subband() const;
//
// public:
//   std::vector<std::vector<Supermode>> k_idx_to_e_idx_to_supermode;
//   const SpectrumData& data;
// };
//
// } // namespace TopoMagnon
//
// #endif // ENTITIES_HPP
