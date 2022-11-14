#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include <vector>
#include <string>
#include <utility>
#include <span>

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

  Supermode(const std::string& superirrep,
            const SpectrumData& data);

  const Bag& get_bag(const SpectrumData& data) const;

  bool operator<(const Supermode& rhs) {
    return this->bag_idx < rhs.bag_idx;
  }

  friend std::ostream& operator<<(std::ostream& out,
                                  const Supermode& supermode);

public:
  int superirrep_idx;
  int bag_idx; // Can be inferred from superirrep_idx, but used here
               // to speed up operator<() which lives in a tight loop
};

class Submode {
public:
  Submode(int subirrep_idx) : subirrep_idx{subirrep_idx}
  { }

  auto operator<=>(const Submode& rhs) const = default;

  friend std::ostream& operator<<(std::ostream& out, const Submode& submode);

public:
  int subirrep_idx;
};


class Subband {
public:

  Vector4<Vector8<int>> dimvalid_e_idxs(const SpectrumData& data);

  Vector32<short> make_br(
    const Vector8<int>& e_idxs_beg,
    const Vector8<int>& e_idxs_end,
    const SpectrumData& data
    );

public:
  Vector8<Vector16<Submode>> subk_idx_to_e_idx_to_submode;
  Vector16<std::span<Submode>> all_submode_spans;
};

class Superband {
public:

  Superband(const std::vector<std::string>& superirreps,
            const SpectrumData& data);

  friend std::ostream& operator<<(std::ostream& out, const Superband& b);
  bool cartesian_permute();

  void populate_subband(Subband& subband, const SpectrumData& data);

public:
  std::vector<std::vector<Supermode>> k_idx_to_e_idx_to_supermode;
};

} // namespace TopoMagnon

#endif // ENTITIES_HPP
