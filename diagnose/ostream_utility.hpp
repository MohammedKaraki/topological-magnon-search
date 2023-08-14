#pragma once

#include <llvm/ADT/SmallVector.h>

#include <iostream>
#include <map>
#include <set>
#include <span>
#include <string>
#include <vector>

namespace TopoMagnon {

std::ostream &print_indent(std::ostream &out, int N);

template <typename A, typename B>
std::ostream &operator<<(std::ostream &out, const std::pair<A, B> &pair) {
    return out << '(' << pair.first << ", " << pair.second << ')';
}

namespace TypeTraits {

template <typename T>
struct is_vector_like {
    static constexpr auto value = false;
};

template <typename T>
struct is_set_like {
    static constexpr auto value = false;
};

template <typename T>
struct is_vector_like<std::vector<T>> {
    static constexpr auto value = true;
};

template <typename T>
struct is_vector_like<std::span<T>> {
    static constexpr auto value = true;
};

template <typename T>
struct is_set_like<std::set<T>> {
    static constexpr auto value = true;
};

template <typename T, unsigned N>
struct is_vector_like<std::array<T, N>> {
    static constexpr auto value = true;
};

template <typename T, unsigned N>
struct is_vector_like<llvm::SmallVector<T, N>> {
    static constexpr auto value = true;
};

}  // namespace TypeTraits

template <typename VectorLike>
requires TypeTraits::is_vector_like<VectorLike>::value std::ostream &operator<<(
    std::ostream &out, const VectorLike &v) {
    out << '[';
    if (v.empty()) {
        return out << ']';
    }

    out << v.front();

    for (auto it = std::next(v.begin()); it != v.end(); ++it) {
        out << " ";
        out << *it;
    }

    return out << ']';
}

template <typename SetLike>
requires TypeTraits::is_set_like<SetLike>::value std::ostream &operator<<(std::ostream &out,
                                                                          const SetLike &v) {
    out << "Set: [";
    for (auto it = v.begin(); it != v.end(); ++it) {
        out << " ";
        out << *it;
    }

    return out << ']';
}

template <typename T>
std::ostream &print(std::ostream &out, const T &t, int total_indent, int consumed_indent) {
    return print_indent(out, total_indent - consumed_indent) << t;
}

template <typename T>
concept StringLike = requires(T t) {
    t.substr(1);
};

template <typename Key, typename Val>
std::ostream &print(std::ostream &out,
                    const std::map<Key, Val> &map,
                    const int total_indent,
                    int consumed_indent) requires StringLike<Key> {
    auto key_size = [](const auto &keyval_pair) { return keyval_pair.first.size(); };

    const int delta_indent =
        5 + key_size(*std::max_element(
                map.cbegin(), map.cend(), [key_size](const auto &lhs, const auto &rhs) {
                    return key_size(lhs) < key_size(rhs);
                }));

    for (const auto &[key, val] : map) {
        print_indent(out, total_indent - consumed_indent);
        out << key << ":";

        print(out, val, total_indent + delta_indent, total_indent + 1 + key.size()) << '\n';
        consumed_indent = 0;
    }

    return out;
}

template <typename Key, typename Val>
std::ostream &operator<<(std::ostream &out, const std::map<Key, Val> &map) {
    return print(out, map, 0, 0);
}

template <typename Val>
std::ostream &operator<<(std::ostream &out, const std::map<int, Val> &map) {
    for (const auto &[key, val] : map) {
        out << key << ":\t" << val << '\n';
    }

    return out;
}

}  // namespace TopoMagnon
