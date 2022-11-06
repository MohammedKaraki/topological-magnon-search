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


// Decomposition of an irrep of supergroup into irreps of subgroup at all
// subgroup-distinct momenta belonging to the supergroup momentum star
class Bag {
public:

  Bag(const std::string& superirrep,
      const SpectrumData& data);

  auto operator<=>(const Bag& rhs) const = default;

  friend std::ostream& operator<<(std::ostream& out, const Bag& b);

public:
  std::vector<std::pair<int, int>> subk_idx_and_subirrep_idx_pairs;
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
  int bag_idx; // Can be inferred from superirrep_idx, but used here
               // to speed up operator<() which lives in a tight loop
};


class Superband {
public:

  Superband(const std::vector<std::string>& superirreps,
            const SpectrumData& data);

  friend std::ostream& operator<<(std::ostream& out, const Superband& b)
  {
    return out << b.k_idx_to_e_idx_to_supermode;
  }

  bool cartesian_permute()
  {
    return Utility::cartesian_permute(k_idx_to_e_idx_to_supermode);
  }

public:
  std::vector<std::vector<Supermode>> k_idx_to_e_idx_to_supermode;
};

} // namespace TopoMagnon

#endif // ENTITIES_HPP
