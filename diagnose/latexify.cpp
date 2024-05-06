#include "diagnose/latexify.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "fmt/core.h"

#include "irreps.hpp"

namespace magnon {

std::string Littleirrep::latexify() const {
    return std::regex_replace(irreplabel, std::regex(R"(GM)"), R"(\Gamma)");
}

std::string Irrep1wp::latexify() const { return fmt::format(R"({{({})}}_{{{}}})", irrep1, wp); }

std::string Irrep12wp::latexify() const {
    return fmt::format(R"(({0}({2})\oplus {1}({2}))\uparrow G)", irrep1, irrep2, wp);
}

std::string Irrep1wpDecomp::latexify() const {
    std::ostringstream result;
    bool past_first = false;
    for (auto it = comps.begin(); it != comps.end();) {
        if (past_first) {
            result << R"(\oplus )";
        }
        past_first = true;

        auto cnt = comps.count(*it);
        if (cnt != 1) {
            result << cnt;
        }
        result << it->latexify();

        std::advance(it, cnt);
    }

    return result.str();
}

std::string Irrep12wpDecomp::latexify() const {
    std::multiset<Irrep1wp> as_irrep1wps;
    for (const auto &irrep12wp : irrep12wp_comps) {
        as_irrep1wps.insert(irrep12wp.get_irrep1wp());
        as_irrep1wps.insert(irrep12wp.get_irrep2wp());
    }

    std::ostringstream result;
    bool past_first = false;
    for (auto it = as_irrep1wps.begin(); it != as_irrep1wps.end();) {
        if (past_first) {
            result << R"(\oplus )";
        }
        past_first = true;

        auto cnt = as_irrep1wps.count(*it);
        if (cnt != 1) {
            result << cnt;
        }
        result << it->latexify();

        std::advance(it, cnt);
    }

    return result.str();
}

std::string LittleirrepDecomp::latexify() const {
    std::ostringstream result;

    int terms_counter = 1;

    for (auto it = littleirrep_comps.begin(); it != littleirrep_comps.end();) {
        if (terms_counter % 6 == 0) {
            result << R"(\nonumber\\)"
                      "\n"
                      R"(&\quad\quad )";
        }

        if (terms_counter != 1) {
            result << R"(\oplus )";
        }

        auto cnt = littleirrep_comps.count(*it);
        if (cnt != 1) {
            result << cnt;
        }
        result << it->latexify();

        std::advance(it, cnt);
        ++terms_counter;
    }

    return result.str();
}

std::string latexify_greeks(const std::string &label) {
    return std::regex_replace(label, std::regex(R"(GM)"), R"(\Gamma)");
}

std::string latexify_row(const IntMatrix &ints, const std::vector<std::string> &strs) {
    assert(ints.size() == static_cast<int>(strs.size()));
    assert(ints.size() > 0);

    bool nonzero_factor_found = false;

    std::ostringstream out;

    for (int i = 0; i < ints.size(); ++i) {
        const int x = ints(i);
        const std::string &s = fmt::format(R"(n^{{{}}})", latexify_greeks(strs[i]));

        if (x != 0) {
            nonzero_factor_found = true;

            if (x == 1) {
                out << '+' << s;
            } else if (x == -1) {
                out << '-' << s;
            } else {
                if (x > 0) {
                    out << '+';
                }
                out << x << s;
            }
        }
    }

    assert(nonzero_factor_found);
    auto result = out.str();
    if (result[0] == '+') {
        return result.substr(1);
    } else {
        assert(result[0] == '-');
        return result.substr(0);
    }
}

std::string latexify_sis(const SpectrumData &data) {
    std::ostringstream result;

    result << R"(\begin{align})" << '\n';

    for (int i = 0; i < data.si_matrix.rows(); ++i) {
        result << fmt::format(R"(z_{{{}}} &= {})",
                              i + 1,
                              latexify_row(data.si_matrix.row(i), data.sub_msg.irreps))
               << R"( \bmod )" << data.si_orders[i];

        if (i + 1 < data.si_matrix.rows()) {
            result << R"(\\)";
        }

        result << '\n';
    }
    result << R"(\end{align})";

    return result.str();
}

std::string latexify_comp_rels(const SpectrumData &data) {
    std::ostringstream result;

    result << R"(\begin{align})" << '\n';

    auto first_nonzero_is_positive = [](const auto &row) {
        for (int i = 0; i < row.size(); ++i) {
            if (row(i) != 0) {
                return row(i) > 0;
            }
        }
        assert(false);
    };

    for (int i = 0; i < data.comp_rels_matrix.rows(); ++i) {
        IntMatrix row_copy = data.comp_rels_matrix.row(i);
        if (!first_nonzero_is_positive(row_copy)) {
            row_copy *= -1;
        }
        assert(first_nonzero_is_positive(row_copy));

        result << "&" << latexify_row(row_copy, data.sub_msg.irreps) << R"(=0)";

        if (i + 1 < data.comp_rels_matrix.rows()) {
            result << R"(\\)";
        }

        result << '\n';
    }
    result << R"(\end{align})" << '\n';

    return result.str();
}

std::string latexify_supercond_chemistries(const SpectrumData &data,
                                           const SupercondChemistries &sc_chems) {
    std::ostringstream result;

    result << R"(\begin{align})"
              "\n"
              R"((S^x,S^y)_{)"
           << data.wp << "}\n";
    for (auto it = sc_chems.begin(); it != sc_chems.end(); ++it) {
        result << "&= " << it->latexify();
        if (std::next(it) != sc_chems.end()) {
            result << "\\\\\n";
        }
    }
    result << R"(\end{align})";

    return result.str();
}

std::string latexify_physics_and_chemistries_pairs(
    const std::vector<std::pair<Physics, Chemistries>> &pairs) {
    std::ostringstream result;

    result << R"(\begin{align})"
              "\n";
    for (int i = 0; i < static_cast<int>(pairs.size()); ++i) {
        const auto &[physics, chemistries] = pairs[i];
        if (i > 0) {
            result << R"(\\)"
                      "\n";
        }
        if (pairs.size() == 1) {
            result << "b\n";
        } else {
            result << "b_{" << i + 1 << "}\n";
        }
        for (const auto &chemistry : chemistries) {
            result << "&= " << chemistry.latexify()
                   << R"(\\)"
                      "\n";
        }
        result << "&= " << physics.latexify() << "\n";
    }
    result << R"(\end{align})";

    return result.str();
}

void LatexDoc::dump(const std::string &filename, bool standalone) {
    auto out = std::ofstream(filename);

    if (!standalone) {
        out << code.str();
    } else {
        out << R"(\documentclass{article})"
               "\n"
               R"(\usepackage{amsmath,amssymb,dsfont})"
               "\n"
               R"(\usepackage{mathtools,microtype,bm,xcolor})"
               "\n"
               R"(\usepackage{adjustbox,enumitem})"
               "\n"
               R"(\usepackage{booktabs,longtable,multirow})"
               "\n"
               R"(\usepackage{hyperref})"
               "\n"
               R"(\usepackage[margin=1in]{geometry})"
               "\n"
               R"()"
               "\n"
               R"(\newcommand{\myfigure}[3]{%)"
               "\n"
               R"(\begin{figure}[b])"
               "\n"
               R"(\centering)"
               "\n"
               R"(\maxsizebox{\textwidth}{%)"
               "\n"
               R"(\textheight-\abovecaptionskip-\belowcaptionskip-\parskip}%)"
               "\n"
               R"({\IfFileExists{#1}{\includegraphics{#1}}{????}})"
               "\n"
               R"(\caption{\label{#2}#3})"
               "\n"
               R"(\end{figure})"
               "\n"
               R"(})"
               "\n"
               R"(\begin{document})"
               "\n"
            << code.str()
            << "\n"
               R"(\end{document})";
    }
}

void LatexDoc::describe_sc_chems(const SpectrumData &data) {

    SupercondChemistries sc_chems;
    const auto phys_chems_pairs = find_physics_and_chemistries_pairs(data, sc_chems);

    *this << fmt::format(R"(On the Wyckoff position ${0}$ of $G={1}$, the site symmetry group )"
                         R"(representation of the spin-wave variables $(S^x,S^y)$ is ${2}$.)"
                         "\n",
                         data.wp,
                         data.super_msg.label,
                         data.site_irreps_as_str());

    if (sc_chems.size() == 1) {
        *this << fmt::format(R"(The band structure induced from ${0}$ on ${1}$ can be uniquely )"
                             R"(decomposed in terms of EBRs as)",
                             data.site_irreps_as_str(),
                             data.wp);
    } else {
        *this << fmt::format(R"(In terms of EBRs of ${3}$, we find that there are )"
                             R"({2} valid decompositions of the band structure )"
                             R"(induced from ${0}$ on ${1}$:)"
                             "\n",
                             data.site_irreps_as_str(),
                             data.wp,
                             sc_chems.size(),
                             data.super_msg.label);
    }
    *this << latexify_supercond_chemistries(data, sc_chems) << "\n";

    if (phys_chems_pairs.size() > 1) {
        *this << fmt::format(R"(Consequently, we find {} possible band representations for )"
                             R"(the positive energy magnons:)"
                             "\n",
                             phys_chems_pairs.size());
    } else {
        *this << R"(This leads to a unique band representation for the positive )"
                 R"(energy magnons:)"
                 "\n";
    }

    *this << latexify_physics_and_chemistries_pairs(phys_chems_pairs) << "\n";
}

std::string latexify_super_to_sub_axis(const SpectrumData &data, int axis_idx) {
    std::string result;

    const std::array super_axes = {R"(\bm{a})", R"(\bm{b})", R"(\bm{c})"};
    for (int i = 0; i < 3; ++i) {
        std::string coeff = data.super_to_sub[i][axis_idx];
        coeff = std::regex_replace(coeff, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");
        assert(!coeff.empty());
        if (coeff == "0") {
            continue;
        }

        if (!result.empty() || axis_idx == 3) {
            if (coeff[0] != '-') {
                coeff = "+" + coeff;
            }
        }

        if (coeff == "-1") {
            coeff = "-";
        }

        if (coeff == "+1") {
            coeff = "+";
        }

        if (coeff == "1") {
            coeff = "";
        }

        result += coeff + super_axes[i];
    }

    if (axis_idx != 3) {
        assert(!result.empty());
    }

    return result;
}

std::string latexify_super_to_sub_axis(const diagnose2::SpectrumData &data, int axis_idx) {
    std::string result;

    const std::array super_axes = {R"(\bm{a})", R"(\bm{b})", R"(\bm{c})"};
    for (int i = 0; i < 3; ++i) {
        std::string coeff = data.super_to_sub.at(i).at(axis_idx);
        coeff = std::regex_replace(coeff, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");
        assert(!coeff.empty());
        if (coeff == "0") {
            continue;
        }

        if (!result.empty() || axis_idx == 3) {
            if (coeff[0] != '-') {
                coeff = "+" + coeff;
            }
        }

        if (coeff == "-1") {
            coeff = "-";
        }

        if (coeff == "+1") {
            coeff = "+";
        }

        if (coeff == "1") {
            coeff = "";
        }

        result += coeff + super_axes[i];
    }

    if (axis_idx != 3) {
        assert(!result.empty());
    }

    return result;
}

std::string latexify_super_to_sub(const SpectrumData &data) {
    std::string aprime = latexify_super_to_sub_axis(data, 0);
    std::string bprime = latexify_super_to_sub_axis(data, 1);
    std::string cprime = latexify_super_to_sub_axis(data, 2);
    std::string dorigin = latexify_super_to_sub_axis(data, 3);

    std::string breakornot1 = R"(\quad)", breakornot2 = R"(\\[2mm])";
    if (aprime.size() + bprime.size() + cprime.size() > 30) {
        std::swap(breakornot1, breakornot2);
    }

    return fmt::format(R"(\begin{{array}}{{c}}
  \bm{{a}}\rightarrow {0},\quad
  \bm{{b}}\rightarrow {1},{4}
  \bm{{c}}\rightarrow {2},{5}
  \bm{{o}}\rightarrow \bm{{o}}{3}
  \end{{array}})",
                       aprime,
                       bprime,
                       cprime,
                       dorigin,
                       breakornot1,
                       breakornot2);
}

std::string latexify_super_to_sub(const diagnose2::SpectrumData &data) {
    std::string aprime = latexify_super_to_sub_axis(data, 0);
    std::string bprime = latexify_super_to_sub_axis(data, 1);
    std::string cprime = latexify_super_to_sub_axis(data, 2);
    std::string dorigin = latexify_super_to_sub_axis(data, 3);

    std::string breakornot1 = R"(\quad)", breakornot2 = R"(\\[2mm])";
    if (aprime.size() + bprime.size() + cprime.size() > 30) {
        std::swap(breakornot1, breakornot2);
    }

    return fmt::format(R"(\begin{{array}}{{c}}
  \bm{{a}}'= {0},\quad
  \bm{{b}}'= {1},{4}
  \bm{{c}}'= {2},{5}
  \bm{{o}}'= \bm{{o}}{3}
  \end{{array}})",
                       aprime,
                       bprime,
                       cprime,
                       dorigin,
                       breakornot1,
                       breakornot2);
}

std::string latexify_super_to_sub_v2(const SpectrumData &data) {
    std::string aprime = latexify_super_to_sub_axis(data, 0);
    std::string bprime = latexify_super_to_sub_axis(data, 1);
    std::string cprime = latexify_super_to_sub_axis(data, 2);
    std::string dorigin = latexify_super_to_sub_axis(data, 3);

    return fmt::format(R"(\begin{{align}}
  \bm{{a}}&\rightarrow\bm{{a}}'={}\\
  \bm{{b}}&\rightarrow\bm{{b}}'={}\\
  \bm{{c}}&\rightarrow\bm{{c}}'={}\\
  \bm{{o}}&\rightarrow\bm{{o}}'=\bm{{o}}{}
  \end{{align}})",
                       aprime,
                       bprime,
                       cprime,
                       dorigin);
}

std::string latexify_super_to_sub_v2(const diagnose2::SpectrumData &data) {
    std::string aprime = latexify_super_to_sub_axis(data, 0);
    std::string bprime = latexify_super_to_sub_axis(data, 1);
    std::string cprime = latexify_super_to_sub_axis(data, 2);
    std::string dorigin = latexify_super_to_sub_axis(data, 3);

    return fmt::format(R"(\begin{{align}}
  \bm{{a}}&\rightarrow\bm{{a}}'={}\\
  \bm{{b}}&\rightarrow\bm{{b}}'={}\\
  \bm{{c}}&\rightarrow\bm{{c}}'={}\\
  \bm{{o}}&\rightarrow\bm{{o}}'=\bm{{o}}{}
  \end{{align}})",
                       aprime,
                       bprime,
                       cprime,
                       dorigin);
}

std::string latexify_korirrep(std::string label) {
    label = std::regex_replace(label, std::regex(R"(GM)"), R"(\Gamma)");
    label = std::regex_replace(label, std::regex(R"(LA)"), R"(\mathit{LA})");
    label = std::regex_replace(label, std::regex(R"(VA)"), R"(\mathit{VA})");
    label = std::regex_replace(label, std::regex(R"(KA)"), R"(\mathit{KA})");
    label = std::regex_replace(label, std::regex(R"(HA)"), R"(\mathit{HA})");
    label = std::regex_replace(label, std::regex(R"(NA)"), R"(\mathit{NA})");
    label = std::regex_replace(label, std::regex(R"(TA)"), R"(\mathit{TA})");
    return R"({)" + label + "}";
}

std::string latexify_gkcoords(std::string g, std::string k, std::string coords) {
    coords = std::regex_replace(coords, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");

    g = std::regex_replace(g, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");

    if (g.empty() || g == R"({1|0})") {
        return fmt::format(R"({{{}({})}})", latexify_korirrep(k), coords);
    } else {
        return fmt::format(
            // R"({{\{{{}\}}{{\bm{{k}}}}_{{{}({})}}}})",
            R"({{\{{{}\}}{{{}({})}}}})",
            std::regex_replace(g.substr(1, g.size() - 2), std::regex(R"(-([0-9]))"), R"(\bar{$1})"),
            latexify_korirrep(k),
            coords);
    }
}

std::string latexify_irrepsum(const std::vector<std::string> &irreps) {
    std::multiset<std::string> sorted_irreps;
    sorted_irreps.insert(irreps.begin(), irreps.end());

    std::ostringstream result;

    for (auto it = sorted_irreps.begin(); it != sorted_irreps.end();) {
        const auto cnt = sorted_irreps.count(*it);

        if (it != sorted_irreps.begin()) {
            result << R"(\oplus )";
        }

        if (cnt > 1) {
            result << cnt << R"(\;)";
        }

        result << *it;

        std::advance(it, cnt);
    }

    return result.str();
}

}  // namespace magnon
