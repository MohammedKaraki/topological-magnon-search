#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <utility>

#include "Eigen/Core"
#include "Eigen/Dense"
#include "fmt/format.h"

#include "range/v3/view.hpp"

using IntMatrix = Eigen::Matrix<int, 3, 3>;
using IntColumn = Eigen::Matrix<int, 3, 1>;
using IntRow = Eigen::Matrix<int, 1, 3>;

class Unitarity {
 public:
    Unitarity(int value) { set(value); }

    Unitarity(const Unitarity &u) { set(u.value); }
    Unitarity &operator=(const Unitarity &u) {
        set(u.value);
        return *this;
    }

    Unitarity operator*(const Unitarity &rhs) const { return Unitarity(value * rhs.value); }

    int get() const { return value; }

 private:
    int value;

    void set(int new_value) {
        assert(new_value == 1 || new_value == -1);
        value = new_value;
    }
};

class PointGroupElement {
 public:
    PointGroupElement(std::string_view gstr) : data{parse_gstr(gstr)} {}

    PointGroupElement(const PointGroupElement &) = default;

    PointGroupElement(const IntMatrix &m, const Unitarity u) : data{Data(m, u)} {}

    const IntMatrix &get_matrix() const { return data.first; }

    const Unitarity get_u() const { return data.second; }

    bool is_identity() const {
        return data.first == IntMatrix::Identity() && data.second.get() == 1;
    }

 private:
    using Data = std::pair<IntMatrix, Unitarity>;
    static Data parse_gstr(std::string_view gstr);

 private:
    Data data;
};

class EField {
 public:
    EField(int x, int y, int z) {
        dir << x, y, z;
        assert(std::tuple(dir(0), dir(1), dir(2)) == std::tuple(x, y, z));
    }

    bool operator==(const EField &) const = default;

    friend EField operator*(const PointGroupElement &g, const EField &e);

 private:
    IntColumn dir;
};

class BField {
 public:
    BField(int x, int y, int z) {
        dir << x, y, z;
        assert(std::tuple(dir(0), dir(1), dir(2)) == std::tuple(x, y, z));
    }

    bool operator==(const BField &) const = default;

    friend BField operator*(const PointGroupElement &g, const BField &b);

 private:
    IntColumn dir;
};

class UniStrain {
 public:
    UniStrain(int x, int y, int z) {
        IntColumn dir;
        dir << x, y, z;
        assert(std::tuple(dir(0), dir(1), dir(2)) == std::tuple(x, y, z));
        matrix = dir * dir.transpose();
    }

    bool operator==(const UniStrain &) const = default;

    friend UniStrain operator*(const PointGroupElement &g, const UniStrain &s);

 private:
    UniStrain(const IntMatrix &matrix) : matrix(matrix) {}

 private:
    IntMatrix matrix;
};

EField operator*(const PointGroupElement &g, const EField &e) {
    IntColumn vec = g.get_matrix() * e.dir;

    return EField(vec(0), vec(1), vec(2));
}

BField operator*(const PointGroupElement &g, const BField &b) {
    int factors = g.get_matrix().determinant() * g.get_u().get();
    IntColumn vec = factors * g.get_matrix() * b.dir;

    return BField(vec(0), vec(1), vec(2));
}

UniStrain operator*(const PointGroupElement &g, const UniStrain &s) {
    return UniStrain(g.get_matrix() * s.matrix * g.get_matrix().transpose());
}

PointGroupElement::Data PointGroupElement::parse_gstr(std::string_view gstr) {
    assert(std::count(gstr.begin(), gstr.end(), ' ') == 0);
    assert(std::count(gstr.begin(), gstr.end(), ',') == 3);
    assert(std::count(gstr.begin(), gstr.end(), 'x') >= 1);
    assert(std::count(gstr.begin(), gstr.end(), 'y') >= 1);
    assert(std::count(gstr.begin(), gstr.end(), 'z') >= 1);
    assert(std::count(gstr.begin(), gstr.end(), '1') >= 1);

    using namespace ranges;
    auto view = gstr | views::split(',') | views::transform([](auto &&x) {
                    return std::string_view(&*x.begin(), distance(x));
                });

    auto parse_coord = [](std::string_view str) {
        IntRow result = IntRow::Zero();

        auto contains = [&str](const std::string &sub) {
            return str.find(sub) != std::string::npos;
        };

        const std::array<std::string, 3> vars = {"x", "y", "z"};
        for (auto i = 0u; i < vars.size(); ++i) {
            auto var = vars[i];

            if (contains('-' + var)) {
                assert(result(i) == 0);
                result(i) = -1;
            } else if (contains('+' + var)) {
                assert(result(i) == 0);
                result(i) = +1;
            } else if (contains(var)) {
                assert(result(i) == 0);
                result(i) = +1;
            }
        }
        assert(result != IntRow::Zero());

        return result;
    };

    IntMatrix m = IntMatrix::Zero();
    int row = 0;
    for (const auto coordstr : view | views::take(3)) {
        m.row(row++) = parse_coord(coordstr);
    }

    {
        auto det = m.determinant();
        assert(det * det == 1);
    }

    Unitarity u(+1);
    auto ustr = *drop_view(view, 3).begin();
    if (ustr == "-1") {
        u = -1;
    } else {
        assert(ustr == "+1");
    }

    return Data(m, u);
}

std::string preserved_gstrs(const std::vector<std::string> &gstrs, const std::vector<bool> &symm) {
    assert(gstrs.size() == symm.size());

    std::string result;

    for (auto i = 0u; i < gstrs.size(); ++i) {
        if (symm[i]) {
            result += gstrs[i] + ';';
        }
    }

    result.erase(result.size() - 1);

    return result;
}

using Dirs = std::vector<std::pair<std::array<const int, 3>, const std::string>>;

const std::string in_generic_direction = "in generic direction";
const Dirs tri_dirs = {
    {{0, 0, 0}, ""},
    {{1, 9, 88}, in_generic_direction},
};

const Dirs mono_dirs = {
    {{0, 0, 0}, ""},
    {{0, 1, 0}, "$\\parallel [010]$"},
    {{1, 0, 5}, "$\\perp [010]$"},
    {{1, 9, 88}, in_generic_direction},
};

const Dirs ortho_dirs = {
    {{0, 0, 0}, ""},
    {{1, 0, 0}, "$\\parallel [100]$"},
    {{0, 1, 0}, "$\\parallel [010]$"},
    {{0, 0, 1}, "$\\parallel [001]$"},
    {{0, 1, 9}, "$\\perp [100]$"},
    {{1, 0, 8}, "$\\perp [010]$"},
    {{121, 3, 0}, "$\\perp [001]$"},
    {{1, 9, 88}, in_generic_direction},
};

const Dirs tetra_dirs = {
    {{0, 0, 0}, ""},
    {{0, 0, 1}, "$\\parallel [001]$"},
    {{1, 0, 0}, "$\\parallel [100]$"},
    {{1, 1, 0}, "$\\parallel [110]$"},
    {{99, 1, 0}, "$\\perp [001]$"},
    {{0, 8, 112}, "$\\perp [100]$"},
    {{1, -1, 9}, "$\\perp [110]$"},
    {{1, 9, 88}, in_generic_direction},
};

const Dirs hex_dirs = {
    {{0, 0, 0}, ""},
    {{0, 0, 1}, "$\\parallel [001]$"},
    {{1, 0, 0}, "$\\parallel [100]$"},
    {{1, 1, 0}, "$\\parallel [110]$"},
    {{99, 1, 0}, "$\\perp [001]$"},
    {{1, 2, 112}, "$\\perp [100]$"},
    {{-1, 1, 9}, "$\\perp [110]$"},
    {{1, 9, 88}, in_generic_direction},
};

const Dirs cubic_dirs = {
    {{0, 0, 0}, ""},
    {{1, 0, 0}, "$\\parallel [100]$"},
    {{1, 1, 0}, "$\\parallel [110]$"},
    {{1, 1, 1}, "$\\parallel [111]$"},
    {{0, 1, 77}, "$\\perp [100]$"},
    {{1, -1, 77}, "$\\perp [110]$"},
    {{97, 3, -100}, "$\\perp [111]$"},
    {{1, 9, 288}, in_generic_direction},
};

// Given "122.59" return `122`.
int extract_parent_sg_number(const std::string &msg_number) {
    std::regex regex(R"(^(\d+)\.(\d+)$)");

    std::smatch match;
    std::regex_match(msg_number, match, regex);
    assert(match.size() == 3);
    assert(match.str(0) == msg_number);

    int num1 = std::stoi(match.str(1));
    int num2 = std::stoi(match.str(2));
    assert(msg_number == fmt::format("{}.{}", num1, num2));

    return num1;
}

const Dirs &get_dirs(const std::string &msg_number) {
    const auto parent_number = extract_parent_sg_number(msg_number);
    assert(parent_number >= 1 && parent_number <= 230);

    if (parent_number <= 2) {
        return tri_dirs;
    } else if (parent_number <= 15) {
        return mono_dirs;
    } else if (parent_number <= 74) {
        return ortho_dirs;
    } else if (parent_number <= 142) {
        return tetra_dirs;
    } else if (parent_number <= 194) {
        return hex_dirs;
    } else if (parent_number <= 230) {
        return cubic_dirs;
    }

    assert(false);
}

std::vector<std::string> raw_input_to_vector(std::string_view gstrs) {
    std::vector<std::string> result;

    using namespace ranges;
    auto view = gstrs | views::split(std::string_view(";")) | views::transform([](auto &&rng) {
                    return std::string_view(&*rng.begin(), distance(rng));
                });

    for (const auto gstr : view) {
        result.emplace_back(gstr);
    }

    std::sort(result.begin(), result.end());
    return result;
}

std::vector<bool> calc_symmetry(const std::vector<PointGroupElement> &gs, const auto &x) {
    std::vector<bool> result{};
    result.reserve(gs.size());

    for (const auto &g : gs) {
        result.push_back(g * x == x);
    }

    return result;
}

std::vector<bool> intersect(const std::vector<bool> &sym1, const std::vector<bool> &sym2) {
    assert(sym1.size() == sym2.size());
    assert(!sym1.empty());

    std::vector<bool> result;
    result.reserve(sym1.size());

    for (auto i = 0u; i < sym1.size(); ++i) {
        result.push_back(sym1[i] && sym2[i]);
    }

    return result;
}

std::vector<bool> intersect(const std::vector<bool> &sym1,
                            const std::vector<bool> &sym2,
                            const std::vector<bool> &sym3) {
    assert(sym1.size() == sym2.size());
    assert(sym1.size() == sym3.size());
    assert(!sym1.empty());

    std::vector<bool> result;
    result.reserve(sym1.size());

    for (auto i = 0u; i < sym1.size(); ++i) {
        result.push_back(sym1[i] && sym2[i] && sym3[i]);
    }

    return result;
}

void print_subgroups(const std::string &msg_num, std::string_view raw_input) {

    const auto &dirs = get_dirs(msg_num);

    assert(dirs[0].first[0] == 0);
    assert(dirs[0].first[1] == 0);
    assert(dirs[0].first[2] == 0);
    assert(dirs[0].second == "");

    auto gstrs = raw_input_to_vector(raw_input);
    auto gs = std::vector<PointGroupElement>{};
    gs.reserve(gstrs.size());

    for (const auto &gstr : gstrs) {
        gs.emplace_back(PointGroupElement(gstr));
    }

    std::vector<std::pair<std::vector<bool>, std::vector<unsigned int>>> e_symms;
    std::vector<std::pair<std::vector<bool>, std::vector<unsigned int>>> b_symms;
    std::vector<std::pair<std::vector<bool>, std::vector<unsigned int>>> s_symms;

    std::map<std::vector<bool>, std::set<std::array<unsigned int, 3>>> ebs_symms;

    auto textify_symms_singlet = [&dirs](std::string field_label,
                                         const std::vector<unsigned int> &dir_ids) {
        assert(!dir_ids.empty());
        std::vector<std::string> ors;
        for (auto dir_idx : dir_ids) {
            if (dir_idx > 0) {
                ors.push_back(field_label + " " + dirs[dir_idx].second);
            }
        }

        if (ors.empty()) {
            return std::string{};
        };
        if (ors.size() == 1) {
            return ors.front();
        }

        if (ors.back().ends_with(in_generic_direction)) {
            return ors.back();
        }

        std::string result;
        for (int i = 0; i < static_cast<int>(ors.size()) - 1; ++i) {
            result += ors[i] + " or ";
        }
        result += ors.back();
        return '(' + result + ')';
    };

    auto textify_symms_triplet = [&textify_symms_singlet, /*&dirs,*/ &e_symms, &b_symms, &s_symms](
                                     std::array<unsigned int, 3> triplet) {
        std::vector<std::string> ands;

        if (int i = triplet[0]; i > 0) {
            ands.push_back(textify_symms_singlet("$\\bm{E}$", e_symms[i].second));
        }

        if (int i = triplet[1]; i > 0) {
            ands.push_back(textify_symms_singlet("$\\bm{B}$", b_symms[i].second));
        }

        if (int i = triplet[2]; i > 0) {
            ands.push_back(textify_symms_singlet("strain", s_symms[i].second));
        }

        assert(!ands.empty());

        std::string result;

        if (ands.size() == 1) {
            result = ands.front();
        } else if (ands.size() == 2) {
            result = ands[0] + " and " + ands[1];
        } else {
            result = ands[0] + " and " + ands[1] + " and " + ands[2];
        }

        return result;
    };

    auto append = [](auto &vec_pairs, const auto &sym, unsigned int dir_idx) {
        bool found = false;
        for (auto i = 0u; i < vec_pairs.size(); ++i) {
            if (sym == vec_pairs[i].first) {
                assert(!found);
                found = true;
                vec_pairs[i].second.push_back(dir_idx);
            }
        }
        if (!found) {
            vec_pairs.emplace_back(std::pair{sym, std::vector{dir_idx}});
            assert(vec_pairs.back().second.size() == 1);
            assert(vec_pairs.back().second.back() == dir_idx);
        }
    };

    auto broke_nothing = [](const std::vector<bool> &symm) {
        assert(!symm.empty());
        return std::count(symm.begin(), symm.end(), true) == static_cast<int>(symm.size());
    };

    auto non_trivial_breaking = [&gs](const std::vector<bool> &symm) {
        auto count = std::count(symm.begin(), symm.end(), true);
        if (count == static_cast<int>(symm.size())) {
            return false;
        }
        for (auto i = 0u; i < symm.size(); ++i) {
            if (symm[i] && !gs[i].is_identity()) {
                return true;
            }
        }

        return false;
    };

    for (auto i = 0u; i < dirs.size(); ++i) {
        const auto &dir = dirs[i];
        int x = dir.first[0];
        int y = dir.first[1];
        int z = dir.first[2];

        {
            EField e(x, y, z);
            auto symm = calc_symmetry(gs, e);
            // std::cout << std::count(symm.begin(), symm.end(), true) << '\n';
            // std::cout << (broke_everything(symm) ? "Broke EveryTHING\n" : "");
            // std::cout << (broke_nothing(symm) ? "Broke nothing\n" : "");
            append(e_symms, symm, i);
        }

        {
            BField b(x, y, z);
            auto symm = calc_symmetry(gs, b);
            // std::cout << std::count(symm.begin(), symm.end(), true) << '\n';
            // std::cout << (broke_everything(symm) ? "Broke EveryTHING\n" : "");
            // std::cout << (broke_nothing(symm) ? "Broke nothing\n" : "");
            append(b_symms, symm, i);
        }

        {
            UniStrain s(x, y, z);
            auto symm = calc_symmetry(gs, s);
            // std::cout << std::count(symm.begin(), symm.end(), true) << '\n';
            // std::cout << (broke_everything(symm) ? "Broke EveryTHING\n" : "");
            // std::cout << (broke_nothing(symm) ? "Broke nothing\n" : "");
            append(s_symms, symm, i);
        }
    }

    assert(broke_nothing(e_symms.front().first));
    assert(broke_nothing(b_symms.front().first));
    assert(broke_nothing(s_symms.front().first));

    for (auto x = 0u; x < e_symms.size(); ++x) {
        const auto &e_symm = e_symms[x].first;
        for (auto y = 0u; y < b_symms.size(); ++y) {
            const auto &b_symm = b_symms[y].first;
            for (auto z = 0u; z < s_symms.size(); ++z) {
                const auto &s_symm = s_symms[z].first;
                auto total_symm = intersect(e_symm, b_symm, s_symm);
                if (non_trivial_breaking(total_symm)) {

                    if (total_symm != intersect(e_symm, b_symm) &&
                        total_symm != intersect(e_symm, s_symm) &&
                        total_symm != intersect(b_symm, s_symm)) {
                        ebs_symms[total_symm].insert({x, y, z});
                    } else {

                        if (total_symm == intersect(e_symm, b_symm)) {
                            if (total_symm != e_symm && total_symm != b_symm) {
                                ebs_symms[total_symm].insert({x, y, 0});
                            } else {
                                if (total_symm == e_symm) {
                                    ebs_symms[total_symm].insert({x, 0, 0});
                                }
                                if (total_symm == b_symm) {
                                    ebs_symms[total_symm].insert({0, y, 0});
                                }
                            }
                        }

                        if (total_symm == intersect(e_symm, s_symm)) {
                            if (total_symm != e_symm && total_symm != s_symm) {
                                ebs_symms[total_symm].insert({x, 0, z});
                            } else {
                                if (total_symm == e_symm) {
                                    ebs_symms[total_symm].insert({x, 0, 0});
                                }
                                if (total_symm == s_symm) {
                                    ebs_symms[total_symm].insert({0, 0, z});
                                }
                            }
                        }

                        if (total_symm == intersect(b_symm, s_symm)) {
                            if (total_symm != b_symm && total_symm != s_symm) {
                                ebs_symms[total_symm].insert({0, y, z});
                            } else {
                                if (total_symm == b_symm) {
                                    ebs_symms[total_symm].insert({0, y, 0});
                                }
                                if (total_symm == s_symm) {
                                    ebs_symms[total_symm].insert({0, 0, z});
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    auto first_line = true;
    for (const auto &[symm, triplets] : ebs_symms) {
        if (!first_line) {
            std::cout << "\n";
        } else {
            first_line = false;
        }

        std::cout << fmt::format("?{}? ?", preserved_gstrs(gstrs, symm));
        auto first = true;
        for (const auto &triplet : triplets) {
            if (first) {
                first = false;
            } else {
                std::cout << ";";
            }
            std::cout << textify_symms_triplet(triplet);
        }
        std::cout << "?";
    }
}

int main() {
    std::string msg_num, gstrs;

    std::cin >> msg_num >> gstrs;
    print_subgroups(msg_num, gstrs);
}
