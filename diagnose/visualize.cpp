#include "visualize.hpp"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <numbers>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "fmt/core.h"

#include "config/visualize_config.pb.h"
#include "entities.hpp"
#include "latexify.hpp"
#include "spectrum_data.hpp"
#include "utils/proto_text_format.hpp"

namespace magnon {

using std::numbers::pi;

const std::string latex_figure_template =
    R"(\documentclass{standalone}
\usepackage{amsmath,amssymb,dsfont,mathtools,microtype,bm,xcolor,tikz}

\pgfdeclarelayer{edgelayer}
\pgfdeclarelayer{nodelayer}
\pgfsetlayers{edgelayer,nodelayer,main}
\usetikzlibrary{arrows.meta}

\newcommand{\siscode}{
SIS-CODE
}

\begin{document}

\tikzset{dot/.style args={#1}{draw,draw opacity=.4,circle,
minimum size=#1,inner sep=0,outer sep=0pt},}

\begin{tikzpicture}[]

\begin{pgfonlayer}{nodelayer}
NODE-LAYER-CODE
\siscode{}
\end{pgfonlayer}

\begin{pgfonlayer}{edgelayer}
EDGE-LAYER-CODE
\end{pgfonlayer}

\end{tikzpicture}

\end{document})";

constexpr std::array palette = {
    Rgb{15, 181, 174},
    Rgb{222, 61, 130},
    Rgb{64, 70, 202},
    Rgb{246, 133, 17},
    Rgb{126, 132, 250},
    Rgb{114, 224, 106},
    Rgb{20, 122, 243},
    Rgb{115, 38, 211},
    // Rgb{232, 198,   0},
    Rgb{255, 200, 0},
    Rgb{203, 93, 0},
    Rgb{0, 143, 93},
    Rgb{188, 233, 49},

    // Rgb{230,  25,  75},
    // Rgb{229, 104, 103},
    // Rgb{221, 242,  31},
    // Rgb{245,  93, 165},
    // Rgb{0, 100, 255},
    // Rgb{100, 0, 255},

    // Rgb{255, 0, 100},
    // Rgb{0, 200, 255},
    // Rgb{255, 200, 0},
    // Rgb{200, 0, 255},
    // Rgb{255, 100, 0},
};

Rgb Visualize::idx_to_rgb(int idx) { return palette[(idx + spec.skip_color) % palette.size()]; }

Rgb Visualize::idxs_to_rgb(const std::vector<int> &idxs) {
    int N = idxs.size();
    int rsq = 0, gsq = 0, bsq = 0;
    for (auto idx : idxs) {
        auto rgb = idx_to_rgb(idx);  // palette[idx % palette.size()];

        rsq += std::pow(rgb.r, 2) / N;
        gsq += std::pow(rgb.g, 2) / N;
        bsq += std::pow(rgb.b, 2) / N;
    }

    return Rgb{std::sqrt(rsq), std::sqrt(gsq), std::sqrt(bsq)};
}

std::string Rgb::to_latex() const {
    return fmt::format(
        "{{rgb,1:red,{:3.3f};green,{:3.3f};blue,{:3.3f}}}", r / 255.0, g / 255.0, b / 255.0);
}

static double scale_idx(int idx, const std::vector<double> &weights) {
    if (weights.size() == 1) {
        assert(idx == 0);
        return 0.5;
    }

    double upstairs = 0.0;
    double downstairs = 0.0;
    for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
        if (i < idx) {
            upstairs += static_cast<double>(weights.at(i));
        }
        downstairs += static_cast<double>(weights.at(i));
    }

    upstairs += static_cast<double>(weights.at(idx)) / 2.0;

    const double min = static_cast<double>(weights.front()) / 2.0;
    const double max = downstairs - static_cast<double>(weights.back()) / 2.0;

    return static_cast<double>(upstairs - min) / static_cast<double>(max - min);
}

struct LineMode {
    IrrepPoint left, right;
    std::string irrep;

    std::pair<double, double> center() const {
        return {0.5 * (left.x + right.x), 0.5 * (left.y + right.y)};
    }

    bool operator<(const LineMode &rhs) const {
        return (left.y + right.y) < (rhs.left.y + rhs.right.y);
    }
};

LabelPosition Visualize::sublabel_position(int sub_e_idx, int num_sub_irreps) const {
    assert(sub_e_idx < num_sub_irreps);
    assert(num_sub_irreps >= 1);

    if (num_sub_irreps == 1) {
        return LabelPosition::Above;
    }
    if (sub_e_idx == 0) {
        return LabelPosition::Below;
    }
    if (sub_e_idx + 1 == num_sub_irreps) {
        return LabelPosition::Above;
    }

    return LabelPosition::Right;
}

std::vector<std::pair<double, double>> make_orbit(int N, float shift = 0.0f) {
    if (N == 1) {
        return {{0.0, 0.0}};
    }

    const float delta_theta = 2.0 * pi / N;
    const float scale = 1.0 / std::sin(delta_theta / 2.0);

    if (N == 3) {
        shift += pi;
    }

    std::vector<std::pair<double, double>> result;
    for (int i = 0; i < N; ++i) {
        result.push_back(
            {scale * std::cos(delta_theta * i - shift), scale * std::sin(delta_theta * i - shift)});
    }

    return result;
}

void Visualize::draw_cluster(double x,
                             double y,
                             double dot_diameter,
                             double theta_shift,
                             int subirrep_idx,
                             std::ostringstream &output) {
    for (const auto &[dx, dy] : make_orbit(data.sub_msg.dims[subirrep_idx], theta_shift)) {
        output << fmt::format(R"(\node [dot={{{:3.3f}cm}},fill={},] )"
                              R"(at ({:3.3f}cm, {:3.3f}cm) {{}};)"
                              "\n",
                              dot_diameter,
                              idx_to_rgb(subirrep_idx).to_latex(),
                              x + 0.4 * dot_diameter * dx,
                              y + 0.4 * dot_diameter * dy);
    }
}

void Visualize::draw_subirrep(double x,
                              double y,
                              int subirrep_idx,
                              LabelPosition label_position,
                              std::ostringstream &output) {
    std::string label, label_position_code;

    if (label_position != LabelPosition::NoLabel) {
        label = "$" + latexify_korirrep(data.sub_msg.irreps[subirrep_idx]) + "$";

        switch (label_position) {
            case LabelPosition::Above:
                label_position_code = "above";
                break;
            case LabelPosition::Right:
                label_position_code = "right";
                break;
            case LabelPosition::Below:
                label_position_code = "below";
                break;
            case LabelPosition::Left:
                label_position_code = "left";
                break;
            case LabelPosition::NoLabel:
                assert(false);
        }
    }

    draw_cluster(x, y, 0.4, 0.0, subirrep_idx, output);

    output << fmt::format(R"(\node [label={}:{}\;{}] )"
                          R"(at ({:3.3f}cm, {:3.3f}cm) {{}};)"
                          "\n",
                          label_position_code,
                          mode == VisMode::Normal ? R"(\huge)" : R"(\huge)",
                          label,
                          x,
                          y);
}

void Visualize::draw_superirrep(double x,
                                double y,
                                LabelPosition label_pos,
                                int superirrep_idx,
                                bool show_irrep_label,
                                const std::vector<int> &subirrep_idxs,
                                std::ostringstream &output) {
    std::string label = "$" + latexify_korirrep(data.super_msg.irreps[superirrep_idx]) + "$";

    std::string label_position_code;
    if (label_pos != LabelPosition::NoLabel) {

        switch (label_pos) {
            case LabelPosition::Above:
                label_position_code = "above";
                break;
            case LabelPosition::Right:
                label_position_code = "right";
                break;
            case LabelPosition::Below:
                label_position_code = "below";
                break;
            case LabelPosition::Left:
                label_position_code = "left";
                break;
            case LabelPosition::NoLabel:
                assert(false);
        }
    }

    output << fmt::format(
        R"(\node[minimum size=1.2cm,label={{[xshift=0mm,yshift={}mm]{}:{}{}}}, circle,)"
        R"(line width=0.1cm,color={},fill={},draw] at ({}cm,{}cm) {{}};)"
        "\n",
        label_position_code == "above" ? -1 : 0,
        label_position_code,
        mode == VisMode::Normal ? R"(\huge)" : R"(\huge)",
        show_irrep_label ? label : "",
        idxs_to_rgb(subirrep_idxs).to_latex(),
        Rgb({240, 245, 250}).to_latex(),
        x,
        y);
}

struct BrokenSupermode {
    int superirrep_idx;
    std::vector<int> subirrep_idxs;
};

std::vector<BrokenSupermode> make_broken_supermodes(const std::vector<Supermode> &supermodes,
                                                    const std::vector<Submode> &submodes,
                                                    const SpectrumData &data) {
    auto get_subdim = [&data](int subirrep_idx) { return data.sub_msg.dims[subirrep_idx]; };
    auto get_superdim = [&data](int superirrep_idx) { return data.super_msg.dims[superirrep_idx]; };

    std::vector<BrokenSupermode> result;

    auto submode_it = submodes.begin();

    for (const auto &supermode : supermodes) {
        BrokenSupermode broken_supermode{.superirrep_idx = supermode.superirrep_idx,
                                         .subirrep_idxs = {}};

        const int superirrep_dim = get_superdim(supermode.superirrep_idx);
        int subirreps_dim = 0;
        while (subirreps_dim < superirrep_dim) {
            subirreps_dim += get_subdim(submode_it->subirrep_idx);
            broken_supermode.subirrep_idxs.push_back(submode_it->subirrep_idx);

            ++submode_it;
        }
        assert(subirreps_dim == superirrep_dim);

        result.push_back(broken_supermode);
    }

    assert(submode_it == submodes.end());

    return result;
}

Visualize::Visualize(std::vector<int> drawn_subk_idxs,
                     const Superband &superband,
                     const Subband &subband,
                     const SpectrumData &data,
                     VisMode mode,
                     VisSpec spec)
    : drawn_subk_idxs{std::move(drawn_subk_idxs)},
      superband{superband},
      subband{subband},
      data{data},
      mode{mode},
      spec{spec} {
    const int max_superirreps_at_fixed_k = std::accumulate(
        superband.k_idx_to_e_idx_to_supermode.begin(),
        superband.k_idx_to_e_idx_to_supermode.end(),
        0,
        [](const auto &l, const auto &r) { return std::max(l, static_cast<int>(r.size())); });
    superband_height = spec.supermode_separation * (max_superirreps_at_fixed_k - 1);
    subband_height = spec.subband_superband_ratio * superband_height;
}

void Visualize::dump(const std::string &filename) {
    std::ostringstream sis_code;
    std::ostringstream node_code;
    std::ostringstream line_code;

    for (int x_idx = 0; x_idx < static_cast<int>(drawn_subk_idxs.size()); ++x_idx) {
        subvisualize_at_x_idx(x_idx, node_code);
    }

    for (int x_idx = 0; x_idx < static_cast<int>(drawn_subk_idxs.size()); ++x_idx) {
        supervisualize_at_x_idx(x_idx, node_code);
    }

    for (int x_idx = 0; x_idx < static_cast<int>(drawn_subk_idxs.size()); ++x_idx) {
        annotate(x_idx, node_code);
    }

    visualize_separation(line_code);

    for (int x_idx = 1; x_idx < static_cast<int>(drawn_subk_idxs.size()); ++x_idx) {
        if (mode != VisMode::Compact) {
            visualize_superlines(x_idx - 1, x_idx, line_code);
        }
        visualize_sublines(x_idx - 1, x_idx, line_code);
    }

    if (!superband.satisfies_antiunit_rels()) {
        const auto left = 0.0;
        const auto right = left + spec.subk_min_dist * (drawn_subk_idxs.size() - 1);
        const auto bottom = subband_y_max + spec.band_band_separation;
        const auto top = bottom + superband_height;

        node_code << fmt::format(R"(\draw [line width=4, red] ({0}cm,{1}cm) -- ({2}cm, {3}cm);)"
                                 "\n"
                                 R"(\draw [line width=4, red] ({0}cm,{3}cm) -- ({2}cm, {1}cm);)"
                                 "\n",
                                 left,
                                 top + 0.1 * superband_height,
                                 right,
                                 bottom - 0.1 * superband_height);
    }

    if (!subband.satisfies_antiunit_rels()) {
        const auto left = 0.0;
        const auto right = left + spec.subk_min_dist * (drawn_subk_idxs.size() - 1);
        const auto bottom = 0.0;
        const auto top = bottom + subband_height;

        node_code << fmt::format(R"(\draw [line width=4, red] ({0}cm,{1}cm) -- ({2}cm, {3}cm);)"
                                 "\n"
                                 R"(\draw [line width=4, red] ({0}cm,{3}cm) -- ({2}cm, {1}cm);)"
                                 "\n",
                                 left,
                                 top + 0.1 * subband_height,
                                 right,
                                 bottom - 0.1 * subband_height);
    }

    for (const auto &[gap, isgapped_and_si] : subband.calc_gap_sis()) {
        if (gap == subband.get_num_bands()) {
            break;
        }
        const auto &[isgapped, si] = isgapped_and_si;
        if (isgapped) {
            const auto x_idx = 0;
            const auto k_idx = drawn_subk_idxs[x_idx];
            const auto &submodes = subband.subk_idx_to_e_idx_to_submode[k_idx];
            int dim = 0;
            int e_idx = 0;
            while (dim < gap) {
                const int subirrep_dim = data.sub_msg.dims.at(submodes[e_idx].subirrep_idx);
                dim += subirrep_dim;
                ++e_idx;
            }
            assert(dim == gap);
            assert(e_idx > 0);

            const auto to_str = [](const IntMatrix &si) {
                std::ostringstream result;
                for (int i = 0; i < si.size(); ++i) {
                    result << si(i);
                }
                return result.str();
            };

            IrrepPoint p1 = x_idx_to_subirrep_points[x_idx][e_idx - 1];
            IrrepPoint p2 = x_idx_to_subirrep_points[x_idx][e_idx];

            const double x1 = 0.5 * (p1.x + p2.x) - 0.2;
            const double y1 = 0.5 * (p1.y + p2.y);
            const double x2 = 0.5 * (p1.x + p2.x) - 0.9;
            const double y2 = 0.5 * (p1.y + p2.y);

            const char *color = si.isZero() ? "black" : "red";

            sis_code << fmt::format(
                R"(\draw[-{{Latex[length=3mm,width=2.25mm]}},line width=.95pt,{},)"
                R"(rounded corners]({}cm,{}cm))"
                R"(--({}cm,{}cm)--({}cm,{}cm);)"
                "\n",
                color,
                x2,
                y2 + 0.25,
                x2,
                y2,
                x1,
                y1);
            sis_code << fmt::format(R"(\node[{},above]at({}cm,{}cm){{\Large\bf{{{}}}}};)"
                                    "\n",
                                    color,
                                    x2,
                                    y2 + .22,
                                    to_str(si));
        }
    }

    std::string result = latex_figure_template;

    result = std::regex_replace(result, std::regex(R"(SIS-CODE)"), sis_code.str());
    result = std::regex_replace(result, std::regex(R"(EDGE-LAYER-CODE)"), line_code.str());
    result = std::regex_replace(result, std::regex(R"(NODE-LAYER-CODE)"), node_code.str());

    auto out = std::ofstream(filename);
    out << result;
}

void Visualize::visualize_separation(std::ostringstream &out) {

    const double xmax = spec.subk_min_dist * (drawn_subk_idxs.size() - 1);
    double ycenter = subband_y_max + 0.45 * spec.band_band_separation;

    if (mode == VisMode::Compact) {
        ycenter = subband_y_max + 3.25;
    }

    std::string label1 = fmt::format(
        R"({{$\underset{{\textrm{{WP: }}{}}}{{{}~\scriptstyle{{({})}}}})"
        R"(\rightarrow\underset{{\textrm{{SI: }}{}}}{{{}~\scriptstyle{{({})}}}}\;\;$}})",
        data.wp,
        data.super_msg.label,
        data.super_msg.number,
        data.si_orders_to_latex(),
        data.sub_msg.label,
        data.sub_msg.number);
    std::string label2 = fmt::format(R"({{${}$}})", latexify_super_to_sub(data));

    double y1 = ycenter + 0.35 * spec.band_band_separation;
    double y2 = ycenter - 0.3 * spec.band_band_separation;

    std::string size1 = "huge";
    std::string size2 = "Large";

    if (drawn_subk_idxs.size() < 8) {
        size1 = "LARGE";
        size2 = "Large";

        label1 = label1 + R"(\;\;\;\;\;)";
    }

    out << fmt::format(R"(\node [label=center:\{} {{{}}}] at ({:3.3f}cm, {:3.3f}cm) {{}};)"
                       "\n",
                       size1,
                       label1,
                       xmax / 4.0,
                       0.5 * (y1 + y2));
    out << fmt::format(R"(\node [label=center:\{} {{{}}}] at ({:3.3f}cm, {:3.3f}cm) {{}};)"
                       "\n",
                       size2,
                       label2,
                       3.0 * xmax / 4.0,
                       0.5 * (y1 + y2));

    double downarrow_x = xmax / 2.0;
    if (drawn_subk_idxs.size() % 2 == 1) {
        downarrow_x += 0.5 * spec.subk_min_dist;
    }

    if (mode == VisMode::Normal) {
        out << fmt::format(R"(\draw [line width=4pt,arrows={{-Latex[width=20pt,length=20pt]}}])"
                           R"(({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
                           "\n",
                           downarrow_x,
                           y1,
                           downarrow_x,
                           y2);
    }
}

void Visualize::annotate(int x_idx, std::ostringstream &output) {
    const int subk_idx = x_idx_to_subk_idx(x_idx);
    const int superk_idx = x_idx_to_superk_idx(x_idx);

    const std::string subk = data.sub_msg.ks[subk_idx];
    std::string subkcoords = data.sub_msg.kcoords[subk_idx];
    const std::string superk = data.super_msg.ks[superk_idx];
    std::string superkcoords = data.super_msg.kcoords[superk_idx];

    std::string gk_code;

    std::string g = data.subk_to_g_and_superk.at(subk).first;

    g = std::regex_replace(g, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");
    subkcoords =
        std::regex_replace(subkcoords, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");
    superkcoords =
        std::regex_replace(superkcoords, std::regex(R"(([0-9])/([0-9]))"), R"(\frac{$1}{$2})");

    if (g == R"({1|0})") {
        g = "";
        gk_code = fmt::format(R"(\Large {{${}({})$}})", latexify_korirrep(superk), superkcoords);
    } else {
        gk_code = fmt::format(
            R"(\Large {{$\{{{}\}}{{\bm{{k}}}}_{{{}}}$}})",
            std::regex_replace(g.substr(1, g.size() - 2), std::regex(R"(-([0-9]))"), R"(\bar{$1})"),
            latexify_korirrep(superk));
    }

    if (mode == VisMode::Normal) {
        output << fmt::format(R"(\node[]at({:3.3f}cm,{:3.3f}cm){{{}}};)"
                              "\n",
                              x_from_x_idx(x_idx),
                              subband_y_max + superband_height + spec.band_band_separation + 1.95,
                              gk_code);
        if (x_idx == 0) {
            output << fmt::format(
                R"(\draw[]({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
                "\n",
                -1.0,
                subband_y_max + superband_height + spec.band_band_separation + 1.95 - 0.4,
                +1.0 + x_from_x_idx(drawn_subk_idxs.size() - 1),
                subband_y_max + superband_height + spec.band_band_separation + 1.95 - 0.4);
            output << fmt::format(R"(\draw[]({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
                                  "\n",
                                  -1.0,
                                  subband_y_min - 0.8 + 0.4,
                                  +1.0 + x_from_x_idx(drawn_subk_idxs.size() - 1),
                                  subband_y_min - 0.8 + 0.4);
        }
    } else if (mode == VisMode::Compact) {
        output << fmt::format(R"(\node[]at({:3.3f}cm,{:3.3f}cm){{{}}};)"
                              "\n",
                              x_from_x_idx(x_idx),
                              subband_y_max + 1.0,
                              gk_code);
        if (x_idx == 0) {
            // output << fmt::format(
            //   R"(\draw[]({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)" "\n",
            //   -1.0, subband_y_max+1.95+0.4,
            //   +1.0+x_from_x_idx(drawn_subk_idxs.size()-1), subband_y_max+1.95+0.4);
            output << fmt::format(R"(\draw[]({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
                                  "\n",
                                  -1.0,
                                  subband_y_max + 1.5 - 0.4,
                                  +1.0 + x_from_x_idx(drawn_subk_idxs.size() - 1),
                                  subband_y_max + 1.5 - 0.4);
            output << fmt::format(R"(\draw[]({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
                                  "\n",
                                  -1.0,
                                  subband_y_min - 0.8 + 0.4,
                                  +1.0 + x_from_x_idx(drawn_subk_idxs.size() - 1),
                                  subband_y_min - 0.8 + 0.4);
        }
    }
    output << fmt::format(R"(\node[]at({:3.3f}cm,{:3.3f}cm){{\Large ${}$}};)"
                          "\n",
                          x_from_x_idx(x_idx),
                          subband_y_min - 0.8,
                          latexify_korirrep(subk) + "(" + subkcoords + ")");
}

void Visualize::supervisualize_at_x_idx(int x_idx, std::ostringstream &output) {
    int subk_idx = x_idx_to_subk_idx(x_idx);
    int superk_idx = x_idx_to_superk_idx(x_idx);

    const std::string subk = data.sub_msg.ks[subk_idx];
    const std::string g = data.subk_to_g_and_superk.at(subk).first;

    const auto &supermodes = superband.k_idx_to_e_idx_to_supermode[superk_idx];
    const auto &submodes = subband.subk_idx_to_e_idx_to_submode[subk_idx];
    const auto &broken_supermodes = make_broken_supermodes(supermodes, submodes, data);
    auto supermode_weights = std::vector<double>{};
    for (const auto &broken_supermode : broken_supermodes) {
        supermode_weights.push_back(
            std::sqrt(static_cast<double>(broken_supermode.subirrep_idxs.size())));
    }

    bool all_right = true;
    for (int e_idx = 0; e_idx < static_cast<int>(broken_supermodes.size()); ++e_idx) {
        const auto &broken_supermode = broken_supermodes[e_idx];

        auto calc_y = [this, &supermode_weights](int idx) {
            return subband_y_max + spec.band_band_separation +
                   superband_height * scale_idx(idx, supermode_weights);
        };
        const double supermode_x = x_from_x_idx(x_idx);
        const double supermode_y = calc_y(e_idx);
        const double room_above = (e_idx + 1 == static_cast<int>(broken_supermodes.size()))
                                      ? 1000.0
                                      : calc_y(e_idx + 1) - calc_y(e_idx);

        x_idx_to_superirrep_points[x_idx].push_back(
            {supermode_x, supermode_y, data.super_msg.irreps[broken_supermode.superirrep_idx]});
        auto label_pos = LabelPosition::Right;

        if (room_above > 2.05) {
            if (e_idx + 1 != static_cast<int>(supermodes.size()) || e_idx == 0 || !all_right) {
                label_pos = LabelPosition::Above;
                all_right = false;
            }
        }

        if (mode == VisMode::Compact) {
            continue;
        }

        draw_superirrep(supermode_x,
                        supermode_y,
                        label_pos,
                        broken_supermode.superirrep_idx,
                        g == R"({1|0})",
                        broken_supermode.subirrep_idxs,
                        output);

        const int num_subirreps = broken_supermode.subirrep_idxs.size();
        const auto orbit = make_orbit(num_subirreps, pi / 2);

        auto sorted_subirrep_idxs = [&broken_supermode]() {
            std::vector<int> result = broken_supermode.subirrep_idxs;
            std::sort(result.begin(), result.end());
            return result;
        }();

        for (int sub_e_idx = 0; sub_e_idx < num_subirreps; ++sub_e_idx) {
            const int subirrep_idx = sorted_subirrep_idxs[sub_e_idx];

            const auto diameter = 0.3;
            double orbit_scale = 1.0;
            if (data.sub_msg.dims[subirrep_idx] > 1 && num_subirreps > 2) {
                orbit_scale = 1.5;
            }

            float theta_shift = 0;
            if (num_subirreps == 3) {
                theta_shift = -2 * pi * (sub_e_idx) / num_subirreps;
            }
            draw_cluster(supermode_x + orbit_scale * 0.4 * diameter * orbit[sub_e_idx].first,
                         supermode_y + orbit_scale * 0.4 * diameter * orbit[sub_e_idx].second,
                         diameter,
                         theta_shift,
                         subirrep_idx,
                         output);
        }
    }
}

void Visualize::subvisualize_at_x_idx(int x_idx, std::ostringstream &output) {
    int subk_idx = x_idx_to_subk_idx(x_idx);
    int superk_idx = x_idx_to_superk_idx(x_idx);

    const auto &supermodes = superband.k_idx_to_e_idx_to_supermode[superk_idx];
    const auto &submodes = subband.subk_idx_to_e_idx_to_submode[subk_idx];
    const auto &broken_supermodes = make_broken_supermodes(supermodes, submodes, data);
    auto supermode_weights = std::vector<double>{};
    for (const auto &broken_supermode : broken_supermodes) {
        supermode_weights.push_back(
            // std::sqrt(static_cast<double>(broken_supermode.subirrep_idxs.size()))
            std::pow(static_cast<double>(broken_supermode.subirrep_idxs.size()), 0.5));
    }

    double prev_y = -1000.0;
    bool all_right = true;
    for (int e_idx = 0; e_idx < static_cast<int>(broken_supermodes.size()); ++e_idx) {
        const auto &broken_supermode = broken_supermodes[e_idx];

        auto calc_supery = [this, supermode_weights](int e_idx) {
            return subband_height * scale_idx(e_idx, supermode_weights);
        };
        double supermode_x = x_from_x_idx(x_idx);
        double supermode_y = calc_supery(e_idx);

        const int num_subirreps = broken_supermode.subirrep_idxs.size();

        if (num_subirreps > 1) {
            output << fmt::format(
                R"(\path [draw, dashed, draw opacity=.5, fill opacity=.4, fill={}])"
                R"(({:3.3f}cm, {:3.3f}cm) ellipse ({:3.3f}cm and {:3.3f}cm);)"
                "\n",
                Rgb({180, 220, 255}).to_latex(),
                supermode_x,
                supermode_y,
                0.60,
                spec.broken_min_dist * (num_subirreps - 1 + 1.5) * 0.5);
        }

        for (int sub_e_idx = 0; sub_e_idx < num_subirreps; ++sub_e_idx) {
            const int subirrep_idx = broken_supermode.subirrep_idxs[sub_e_idx];

            auto calc_y = [this, supermode_y, num_subirreps](int sub_e_idx) {
                return supermode_y + spec.broken_min_dist * (sub_e_idx - (num_subirreps - 1) * 0.5);
            };
            const double submode_x{supermode_x};
            const double submode_y{calc_y(sub_e_idx)};

            const double room_below = 0.5 * (submode_y - prev_y);
            prev_y = submode_y;
            const double room_above =
                (e_idx + 1 == static_cast<int>(supermodes.size()))
                    ? 1000.0
                    : 0.5 * (calc_supery(e_idx + 1) - calc_supery(e_idx) -
                             (broken_supermodes[e_idx + 1].subirrep_idxs.size() - 1 +
                              num_subirreps - 1) *
                                 0.5 * spec.broken_min_dist);

            auto label_pos = sublabel_position(sub_e_idx, num_subirreps);
            subband_y_max = std::max(subband_y_max, submode_y);
            if (label_pos == LabelPosition::Below && e_idx == 0 && sub_e_idx == 0) {
                subband_y_min = std::min(subband_y_min, submode_y - 0.75);
            } else {
                subband_y_min = std::min(subband_y_min, submode_y);
            }
            if (e_idx + 1 == static_cast<int>(supermodes.size()) &&
                sub_e_idx + 1 == num_subirreps) {
                if (e_idx + sub_e_idx > 0 && all_right) {
                    label_pos = LabelPosition::Right;
                }
            }
            if (label_pos == LabelPosition::Below && room_below < 0.95) {
                label_pos = LabelPosition::Right;
            }
            if (label_pos == LabelPosition::Above && room_above < 0.95) {
                label_pos = LabelPosition::Right;
            }
            if (label_pos != LabelPosition::Right) {
                all_right = false;
            }
            draw_subirrep(submode_x, submode_y, subirrep_idx, label_pos, output);
            x_idx_to_subirrep_points[x_idx].push_back(
                {submode_x, submode_y, data.sub_msg.irreps[subirrep_idx]});
        }
    }
}

void Visualize::visualize_superlines(int x1_idx, int x2_idx, std::ostringstream &output) {
    const int superk1_idx = x_idx_to_superk_idx(x1_idx);
    const int superk2_idx = x_idx_to_superk_idx(x2_idx);
    const std::string superk1 = data.super_msg.ks[superk1_idx];
    const std::string superk2 = data.super_msg.ks[superk2_idx];

    const auto &superirrep_to_lineirreps =
        data.super_msg.k1_to_k2_to_irrep_to_lineirreps.at(superk1).at(superk2);

    std::map<std::string, std::pair<std::queue<IrrepPoint>, std::queue<IrrepPoint>>>
        lineirrep_to_left_and_right_queues;

    const auto &irrep_points1 = x_idx_to_superirrep_points.at(x1_idx);
    const auto &irrep_points2 = x_idx_to_superirrep_points.at(x2_idx);

    for (const auto &irrep_point1 : irrep_points1) {
        for (const auto &lineirrep : superirrep_to_lineirreps.at(irrep_point1.irrep)) {
            lineirrep_to_left_and_right_queues[lineirrep].first.push(irrep_point1);
        }
    }

    for (const auto &irrep_point2 : irrep_points2) {
        for (const auto &lineirrep : superirrep_to_lineirreps.at(irrep_point2.irrep)) {
            lineirrep_to_left_and_right_queues[lineirrep].second.push(irrep_point2);
        }
    }

    std::vector<LineMode> line_modes;

    for (auto &[lineirrep, left_and_right_queues] : lineirrep_to_left_and_right_queues) {

        auto &[left_queue, right_queue] = left_and_right_queues;
        assert(left_queue.size() == right_queue.size());
        while (!left_queue.empty()) {

            const auto left_irrep_point = left_queue.front();
            const auto right_irrep_point = right_queue.front();
            left_queue.pop();
            right_queue.pop();

            line_modes.push_back(
                LineMode{.left = left_irrep_point, .right = right_irrep_point, .irrep = lineirrep});
        }
    }
    std::sort(line_modes.begin(), line_modes.end());

    // int line_mode_idx = -1;
    double next_line_y = -1000;
    for (const auto &line_mode : line_modes) {
        const auto &[center_x, center_y] = line_mode.center();
        if (center_y > next_line_y + 0.3) {
            next_line_y = center_y;
        } else {
            next_line_y += 0.3;
        }

        // ++line_mode_idx;
        output << fmt::format(
            R"(\draw[rounded corners=40pt,line width=2,color=gray,opacity=.4]({:3.3f}cm,{:3.3f}cm))"
            R"(--({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
            "\n",
            line_mode.left.x,
            line_mode.left.y,
            center_x,
            next_line_y,  //+0.3*(scale_idx(line_mode_idx, line_modes.size())-0.5),
            line_mode.right.x,
            line_mode.right.y);
    }
}

void Visualize::visualize_sublines(int x1_idx, int x2_idx, std::ostringstream &output) {
    const int subk1_idx = x_idx_to_subk_idx(x1_idx);
    const int subk2_idx = x_idx_to_subk_idx(x2_idx);
    const std::string subk1 = data.sub_msg.ks[subk1_idx];
    const std::string subk2 = data.sub_msg.ks[subk2_idx];

    const auto &subirrep_to_lineirreps =
        data.sub_msg.k1_to_k2_to_irrep_to_lineirreps.at(subk1).at(subk2);

    std::map<std::string, std::pair<std::queue<IrrepPoint>, std::queue<IrrepPoint>>>
        lineirrep_to_left_and_right_queues;

    const auto &irrep_points1 = x_idx_to_subirrep_points.at(x1_idx);
    const auto &irrep_points2 = x_idx_to_subirrep_points.at(x2_idx);

    // int line_modes_count = 0;

    for (const auto &irrep_point1 : irrep_points1) {
        for (const auto &lineirrep : subirrep_to_lineirreps.at(irrep_point1.irrep)) {
            lineirrep_to_left_and_right_queues[lineirrep].first.push(irrep_point1);
            // ++line_modes_count;
        }
    }

    for (const auto &irrep_point2 : irrep_points2) {
        for (const auto &lineirrep : subirrep_to_lineirreps.at(irrep_point2.irrep)) {
            lineirrep_to_left_and_right_queues[lineirrep].second.push(irrep_point2);
        }
    }

    std::vector<LineMode> line_modes;

    // int color_idx = 1;
    for (auto &[lineirrep, left_and_right_queues] : lineirrep_to_left_and_right_queues) {
        // ++color_idx;

        auto &[left_queue, right_queue] = left_and_right_queues;
        assert(left_queue.size() == right_queue.size());
        while (!left_queue.empty()) {

            const auto left_irrep_point = left_queue.front();
            const auto right_irrep_point = right_queue.front();
            left_queue.pop();
            right_queue.pop();

            line_modes.push_back(
                LineMode{.left = left_irrep_point, .right = right_irrep_point, .irrep = lineirrep});
        }
    }
    std::sort(line_modes.begin(), line_modes.end());

    // int line_mode_idx = -1;
    double next_line_y = -1000;
    for (const auto &line_mode : line_modes) {
        const auto &[center_x, center_y] = line_mode.center();
        if (center_y > next_line_y + 0.3) {
            next_line_y = center_y;
        } else {
            next_line_y += 0.3;
        }

        // ++line_mode_idx;
        output << fmt::format(
            R"(\draw[rounded corners=40pt,line width=2,color=gray,opacity=.4]({:3.3f}cm,{:3.3f}cm))"
            R"(--({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)",
            line_mode.left.x,
            line_mode.left.y,
            center_x,
            next_line_y,
            line_mode.right.x,
            line_mode.right.y);
    }
}

int Visualize::x_idx_to_subk_idx(int x_idx) {
    const auto subk_idx = drawn_subk_idxs[x_idx];

    return subk_idx;
}

int Visualize::x_idx_to_superk_idx(int x_idx) {
    const auto subk_idx = x_idx_to_subk_idx(x_idx);
    const auto superk_idx = data.subk_idx_to_superk_idx(subk_idx);
    return superk_idx;
}

double Visualize::x_from_x_idx(int x_idx) { return x_idx * spec.subk_min_dist; }

std::pair<VisMode, VisSpec> mode_spec_pair_from_file(const std::optional<std::string> filename) {
    const std::string VISUALIZE_CONFIG_DEFAULT_PATH = "config/visualize_config_base.cfg";
    const std::string actual_filename =
        filename.has_value() ? filename.value() : VISUALIZE_CONFIG_DEFAULT_PATH;
    VisMode mode{VisMode::Normal};
    VisSpec spec{};

    magnon::config::VisualizeConfig visualize_config{};
    magnon::utils::proto::read_from_text_file(actual_filename, visualize_config);

    if (visualize_config.has_mode()) {
        switch (visualize_config.mode()) {
            case magnon::config::VisualizeConfig::NORMAL:
                mode = VisMode::Normal;
                break;
            case magnon::config::VisualizeConfig::COMPACT:
                mode = VisMode::Compact;
                break;
            default:
                assert(false);
        }
    }
    if (visualize_config.has_band_band_separation()) {
        spec.band_band_separation = visualize_config.band_band_separation();
    }
    if (visualize_config.has_subk_min_dist()) {
        spec.subk_min_dist = visualize_config.subk_min_dist();
    }
    if (visualize_config.has_subband_superband_ratio()) {
        spec.subband_superband_ratio = visualize_config.subband_superband_ratio();
    }
    if (visualize_config.has_supermode_separation()) {
        spec.supermode_separation = visualize_config.supermode_separation();
    }
    if (visualize_config.has_skip_color()) {
        spec.skip_color = visualize_config.skip_color();
    }

    return std::pair(mode, spec);
}

}  // namespace magnon
