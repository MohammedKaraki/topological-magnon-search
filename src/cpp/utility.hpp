#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <algorithm>

namespace TopoMagnon {
namespace Utility {

template<typename T>
std::size_t find_unique_index(const std::vector<T>& elems,
                              const T& elem)
{
  auto it = std::find(elems.cbegin(),
                      elems.cend(),
                      elem);
  assert(std::count(std::next(it),
                    elems.cend(),
                    elem)
         == 0
         );

  return std::distance(elems.begin(), it);
}

template<typename Vecs>
bool cartesian_permute(Vecs& vecs)
{
  for (auto& vec : vecs)
  {
    if (std::next_permutation(vec.begin(), vec.end())) {
      return true;
    }
  }

  return false;
}

} // namespace Utility
} // namespace TopoMagnon

#endif // UTILITY_HPP
