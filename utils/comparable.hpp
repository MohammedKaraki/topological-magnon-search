#pragma once

#include <tuple>

template <typename... Ts>
std::tuple<Ts...> tuple_of_vals_from_tuple_of_refs(const std::tuple<Ts &...>);

#define MAKE_COMPARABLE()                                                             \
    bool operator==(const auto &rhs) const {                                          \
        static_assert(sizeof(tuple_of_vals_from_tuple_of_refs(this->field_refs())) == \
                      sizeof(*this));                                                 \
        return this->field_refs() == rhs.field_refs();                                \
    }                                                                                 \
    bool operator<(const auto &rhs) const { return this->field_refs() < rhs.field_refs(); }
