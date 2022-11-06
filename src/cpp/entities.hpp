#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <vector>
#include <set>
#include <string>
#include <utility>
#include <cassert>
#include <fmt/format.h>
#include <span>

#include "ostream_utility.hpp"
#include "utility.hpp"

namespace TopoMagnon {

struct SpectrumData;

// An irrep of subgroup
class Cluster {
public:
  Cluster(std::string label) : label{std::move(label)}
  { }

  auto operator<=>(const Cluster& rhs) const = default;

  friend std::ostream& operator<<(std::ostream& out, const Cluster& c)
  {
    return out << c.label;
  }

private:
  std::string label;
};

// Decomposition of an irrep of supergroup into irreps of subgroup at all
// subgroup-distinct momenta belonging to the supergroup momentum star
class Bag {
public:
  Bag(const std::vector<std::string>& subirreps)
  {
    for (const auto& subirrep : subirreps) {
      clusters.insert(subirrep);
    }
  }

  auto operator<=>(const Bag& rhs) const = default;

  std::vector<Cluster> as_sorted_vector() const
  {
    auto result = std::vector(clusters.begin(), clusters.end());
    assert(std::is_sorted(result.begin(), result.end()));
    return result;
  }

  friend std::ostream& operator<<(std::ostream& out, const Bag& b);

private:
  std::multiset<Cluster> clusters;
};

// Use integer *references* to superirrep and bag, rather than the objects
// themselves.
// This design is crucial for optimization purposes, as the structure
// and its comparison operator live in a very tight loop in the main algorithm.
class Supermode {
public:

  bool operator<(const Supermode& rhs) {
    return this->bag_idx < rhs.bag_idx;
  }

  Supermode(const std::string& superirrep,
            const SpectrumData& data);

  const Bag& get_bag(const SpectrumData& data) const;


  friend std::ostream& operator<<(std::ostream& out, const Supermode& supermode)
  {
    return out << fmt::format("({}, {})",
                              supermode.superirrep_idx,
                              supermode.bag_idx
                             );
  }

private:
  int superirrep_idx;
  int bag_idx;
};

// class FixedkSupermodes {
// public:
//
//   friend std::ostream& operator<<(std::ostream& out, const FixedkSupermodes& f);
//
//   bool permute()
//   {
//     return std::next_permutation(energy_idx_to_supermode.begin(),
//                                  energy_idx_to_supermode.end());
//   }
//
// public:
//   std::vector<Supermode> energy_idx_to_supermode;
// };



class Superband {

public:

  Superband(const std::vector<std::string>& superirreps,
            const SpectrumData& data);

  friend std::ostream& operator<<(std::ostream& out, const Superband& b)
  {
    return out << b.k_idx_to_e_idx_to_supermodes;
  }

  bool cartesian_permute()
  {
    return Utility::cartesian_permute(k_idx_to_e_idx_to_supermodes);
  }

public:
  std::vector<std::vector<Supermode>> k_idx_to_e_idx_to_supermodes;
};

} // namespace TopoMagnon

#endif // ENTITIES_HPP
