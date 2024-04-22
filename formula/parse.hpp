#pragma once

#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace magnon::formula {

struct Term {
    bool operator==(const Term &rhs) const {
        return std::tie(factor, basis) == std::tie(rhs.factor, rhs.basis);
    }

    int factor{};
    std::string basis{};
};

using Terms = std::vector<Term>;

std::optional<Terms> parse_formula(const std::string &formula);

}  // namespace magnon::formula
