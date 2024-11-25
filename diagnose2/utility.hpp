#pragma once

#include <Eigen/Core>
#include <algorithm>
#include <set>
#include <vector>

namespace magnon::diagnose2 {

using MatrixInt = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
std::size_t find_unique_index(const std::vector<T> &elems, const T &elem) {
    auto it = std::find(elems.cbegin(), elems.cend(), elem);
    assert(it != elems.cend());
    assert(std::count(std::next(it), elems.cend(), elem) == 0);

    return std::distance(elems.begin(), it);
}

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

}  // namespace magnon::diagnose2
