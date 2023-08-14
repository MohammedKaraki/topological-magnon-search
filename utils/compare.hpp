#pragma once
#include <tuple>

template <typename... Ts>
std::tuple<Ts...> tuple_of_vals_from_tuple_of_refs(const std::tuple<Ts &...>);

#define MAKE_COMPARABLE()                                                                \
    bool operator==(const auto &rhs) const {                                             \
        static_assert(sizeof(tuple_of_vals_from_tuple_of_refs(this->view_as_tuple())) == \
                      sizeof(*this));                                                    \
        return this->view_as_tuple() == rhs.view_as_tuple();                             \
    }                                                                                    \
    bool operator<(const auto &rhs) const { return this->view_as_tuple() < rhs.view_as_tuple(); }
