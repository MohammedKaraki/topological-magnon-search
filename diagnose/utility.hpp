#pragma once

#include <llvm/ADT/SmallVector.h>

#include <Eigen/Core>
#include <algorithm>
#include <set>
#include <vector>
using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

template <typename T>
using Vector2 = llvm::SmallVector<T, 2>;

template <typename T>
using Vector4 = llvm::SmallVector<T, 4>;

template <typename T>
using Vector8 = llvm::SmallVector<T, 8>;

template <typename T>
using Vector16 = llvm::SmallVector<T, 16>;

template <typename T>
using Vector32 = llvm::SmallVector<T, 32>;

template <typename T>
using Vector = Vector8<T>;

namespace TopoMagnon {
namespace Utility {

template <typename T>
std::size_t find_unique_index(const std::vector<T> &elems, const T &elem) {
    auto it = std::find(elems.cbegin(), elems.cend(), elem);
    assert(std::count(std::next(it), elems.cend(), elem) == 0);

    return std::distance(elems.begin(), it);
}

template <typename VeclikeOfVeclikes>
bool cartesian_permute(VeclikeOfVeclikes &vecs);

template <typename T>
bool cartesian_permute(T &vecs) {
    constexpr bool has_begin_begin = requires(T t) { t.begin()->begin(); };
    for (auto &vec : vecs) {
        if constexpr (has_begin_begin) {
            if (std::next_permutation(vec.begin(), vec.end())) {
                return true;
            }
        } else {
            if (vec.permute()) {
                return true;
            }
        }
    }

    return false;
}

template <typename T>
bool is_cartesian_sorted(T &vecs) {
    for (auto &vec : vecs) {
        if (!std::is_sorted(vec.begin(), vec.end())) {
            return false;
        }
    }

    return true;
}

template <typename Vec>
bool all_equal(const Vec &vec) {

    assert(!vec.empty());  // Remove if you like, it is just a sanity check:
                           // my use case doesn't pass empty v.

    if (vec.empty()) {
        return true;
    }

    for (const auto &x : vec) {
        if (x != vec.front()) {
            return false;
        }
    }

    return true;
}

template <typename T>
Vector<T> intersect(const Vector<std::set<T>> &sets) {
    Vector<T> result;

    if (sets.empty()) {
        return {};
    }

    std::copy(sets.begin()->begin(), sets.begin()->end(), std::back_inserter(result));

    for (auto it = std::next(sets.begin()); it != sets.end(); ++it) {
        Vector<T> tmp;
        std::set_intersection(
            result.begin(), result.end(), it->begin(), it->end(), std::back_inserter(tmp));
        result = std::move(tmp);
    }

    return result;
}
}  // namespace Utility
}  // namespace TopoMagnon
