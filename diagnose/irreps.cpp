#include "irreps.hpp"

#include <cassert>
#include <regex>
#include <sstream>

#include "fmt/core.h"

namespace magnon {

Littleirrep::Littleirrep(std::string source_str) : src{std::move(source_str)} {
    const std::regex regex{R"((([A-Z]+).*)\(([0-9]+)\))"};
    std::smatch match;
    assert(std::regex_match(this->src, match, regex));

    irreplabel = match[1];
    ksymbol = match[2];
    dim = match[3];
}

Irrep1wp::Irrep1wp(std::string src) {
    const std::regex regex{R"(\(([^,]+),([^,]+)\))"};
    std::smatch match;
    assert(std::regex_match(src, match, regex));

    irrep1 = match[1];
    wp = match[2];
}

const std::string Irrep1wp::to_str() const { return "(" + irrep1 + "," + wp + ")"; }

Irrep12wp::Irrep12wp(std::string source_str) : src{std::move(source_str)} {
    const std::regex regex{R"(\(([^,]+),([^,]+),([^,]+)\))"};
    std::smatch match;
    assert(std::regex_match(src, match, regex));

    irrep1 = match[1];
    irrep2 = match[2];
    wp = match[3];
}

Irrep12wpDecomp::Irrep12wpDecomp(const std::vector<std::string> &srcs) {
    for (const auto &src : srcs) {
        irrep12wp_comps.insert(Irrep12wp(src));
    }
}

static std::set<std::multiset<Irrep1wp>> particle_irrep1wp_decomps_helper(
    const std::multiset<Irrep1wp> &cur_decomp, std::multiset<Irrep12wp> remaining_irrep12wps) {
    if (remaining_irrep12wps.empty()) {
        return {cur_decomp};
    }

    Irrep12wp next_irrep12wp = *remaining_irrep12wps.begin();
    remaining_irrep12wps.erase(remaining_irrep12wps.begin());

    std::set<std::multiset<Irrep1wp>> result;

    {
        std::multiset<Irrep1wp> new_decomp = cur_decomp;
        new_decomp.insert(next_irrep12wp.get_irrep1wp());
        for (const auto &decomp :
             particle_irrep1wp_decomps_helper(new_decomp, remaining_irrep12wps)) {
            result.insert(decomp);
        }
    }

    if (next_irrep12wp.get_irrep1wp() != next_irrep12wp.get_irrep2wp()) {
        std::multiset<Irrep1wp> new_decomp = cur_decomp;
        new_decomp.insert(next_irrep12wp.get_irrep2wp());
        for (const auto &decomp :
             particle_irrep1wp_decomps_helper(new_decomp, remaining_irrep12wps)) {
            result.insert(decomp);
        }
    }

    return result;
}

std::set<Irrep1wpDecomp> Irrep12wpDecomp::find_all_magnon_irrep1wp_decomps() const {
    std::set<Irrep1wpDecomp> result;

    for (auto &comps : particle_irrep1wp_decomps_helper({}, irrep12wp_comps)) {
        result.insert(Irrep1wpDecomp(comps));
    }

    return result;
}

LittleirrepDecomp::LittleirrepDecomp(const std::vector<std::string> &srcs) {
    for (const auto &src : srcs) {
        littleirrep_comps.insert(Littleirrep(src));
    }
}

std::vector<std::string> LittleirrepDecomp::get_irreps_as_strs() const {
    std::vector<std::string> result;

    for (const auto &littleirrep : littleirrep_comps) {
        result.push_back(littleirrep.get_irreplabel());
    }

    return result;
}

}  // namespace magnon
