#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <vector>
#include <algorithm>

#include <llvm/ADT/SmallVector.h>

template<typename T>
using Vector2 = llvm::SmallVector<T, 2>;

template<typename T>
using Vector4 = llvm::SmallVector<T, 4>;

template<typename T>
using Vector8 = llvm::SmallVector<T, 8>;

template<typename T>
using Vector16 = llvm::SmallVector<T, 16>;

template<typename T>
using Vector32 = llvm::SmallVector<T, 32>;


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

template<typename VeclikeOfVeclikes>
bool cartesian_permute(VeclikeOfVeclikes& vecs)
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
