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

} // namespace Utility
} // namespace TopoMagnon

#endif // OSTREAM_UTILITY_HPP
