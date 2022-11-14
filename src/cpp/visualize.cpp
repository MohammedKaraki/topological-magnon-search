#include <array>
#include <cassert>
#include <numeric>
#include <utility>
#include <string>
#include <regex>
#include <sstream>
#include <fstream>
#include <numbers>
#include <queue>

#include <fmt/core.h>
#include <llvm/ADT/ArrayRef.h>

#include "visualize.hpp"
#include "spectrum_data.hpp"
#include "entities.hpp"



namespace TopoMagnon {

using std::numbers::pi;


const std::string latex_template =
R"(\documentclass{standalone}
\usepackage{amsmath,amssymb,dsfont,mathtools,microtype,bm,xcolor,tikz}

\pgfdeclarelayer{edgelayer}
\pgfdeclarelayer{nodelayer}
\pgfsetlayers{edgelayer,nodelayer,main}
\usetikzlibrary{arrows.meta}

\newcommand{\debug}{
DEBUG-CODE
}

\begin{document}

\tikzset{dot/.style args={#1}{draw,draw opacity=.4,circle,
minimum size=#1,inner sep=0,outer sep=0pt},}

\begin{tikzpicture}[]

\begin{pgfonlayer}{nodelayer}
NODE-LAYER-CODE
\debug{}
\end{pgfonlayer}

\begin{pgfonlayer}{edgelayer}
EDGE-LAYER-CODE
\end{pgfonlayer}

\end{tikzpicture}

\end{document})";


constexpr std::array palette = {
  Rgb{ 15, 181, 174},
  Rgb{222,  61, 130},
  Rgb{ 64,  70, 202},
  Rgb{246, 133,  17},
  Rgb{126, 132, 250},
  Rgb{114, 224, 106},
  Rgb{ 20, 122, 243},
  Rgb{115,  38, 211},
  Rgb{232, 198,   0},
  Rgb{203,  93,   0},
  Rgb{  0, 143,  93},
  Rgb{188, 233,  49}
};


static Rgb idx_to_rgb(int idx)
{
  return palette[idx % palette.size()];
}

static Rgb idxs_to_rgb(const std::vector<int>& idxs)
{
  int N = idxs.size();
  int rsq = 0, gsq = 0, bsq = 0;
  for (auto idx : idxs) {
    auto rgb = palette[idx % palette.size()];

    rsq += std::pow(rgb.r, 2) / N;
    gsq += std::pow(rgb.g, 2) / N;
    bsq += std::pow(rgb.b, 2) / N;
  }

  return Rgb{std::sqrt(rsq), std::sqrt(gsq), std::sqrt(bsq)};
}

std::string Rgb::to_latex() const
{
  return fmt::format("{{rgb,1:red,{:3.3f};green,{:3.3f};blue,{:3.3f}}}",
                     r/255.0, g/255.0, b/255.0
                    );
}

static double scale_idx(int idx, int N)
{
  assert(N >= 1);
  assert(idx < N);

  if (N == 1) {
    return 0.5;
  }

  return static_cast<double>(idx) / (N - 1);
}


struct LineMode {
  IrrepPoint left, right;
  std::string irrep;

  std::pair<double, double> center() const
  {
    return {0.5*(left.x+right.x), 0.5*(left.y+right.y)};
  }

  bool operator<(const LineMode& rhs) const
  {
    return (left.y + right.y) < (rhs.left.y + rhs.right.y);
  }
};

enum class LabelPosition {
  NoLabel, Above, Right, Below, Left
};

LabelPosition sublabel_position(int sub_e_idx, int num_sub_irreps)
{
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


std::vector<std::pair<double, double>> make_orbit(int N,
                                                  float shift = 0.0f)
{
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
    result.push_back({scale * std::cos(delta_theta*i - shift),
                      scale * std::sin(delta_theta*i - shift)}
                    );
  }

  return result;
}


static void draw_cluster(double x,
                         double y,
                         double dot_diameter,
                         double theta_shift,
                         int subirrep_idx,
                         const SpectrumData& data,
                         std::ostringstream& output
                        )
{
  for (const auto& [dx, dy]
       : make_orbit(data.sub_msg.dims[subirrep_idx], theta_shift))
  {
    output << fmt::format(
      R"(\node [dot={{{:3.3f}cm}},fill={},] )"
      R"(at ({:3.3f}cm, {:3.3f}cm) {{}};)" "\n",
      dot_diameter,
      idx_to_rgb(subirrep_idx).to_latex(),
      x + 0.4*dot_diameter*dx,
      y + 0.4*dot_diameter*dy
      );
  }
}

static std::string mathify_label(const std::string& label)
{
  return
    "$" + std::regex_replace(label, std::regex(R"(GM)"), R"(\Gamma)") + "$";
}

static void draw_subirrep(double x,
                          double y,
                          int subirrep_idx,
                          LabelPosition label_position,
                          const SpectrumData& data,
                          std::ostringstream& output
                         )
{
  std::string label, label_position_code;

  if (label_position != LabelPosition::NoLabel) {
    label = mathify_label(data.sub_msg.irreps[subirrep_idx]);

    switch (label_position) {
      case LabelPosition::Above: label_position_code = "above"; break;
      case LabelPosition::Right: label_position_code = "right"; break;
      case LabelPosition::Below: label_position_code = "below"; break;
      case LabelPosition::Left: label_position_code = "left"; break;
      case LabelPosition::NoLabel: assert(false);
    }
  }


  draw_cluster(x, y, .4, 0.0, subirrep_idx, data, output);

  output << fmt::format(
    R"(\node [label={}:\huge\;{}] )"
    R"(at ({:3.3f}cm, {:3.3f}cm) {{}};)" "\n",
    label_position_code,
    label,
    x,
    y
    );
}

static void draw_superirrep(double x,
                            double y,
                            int superirrep_idx,
                            bool show_irrep_label,
                            const std::vector<int>& subirrep_idxs,
                            const SpectrumData& data,
                            std::ostringstream& output
                           )
{
  std::string label = mathify_label(data.super_msg.irreps[superirrep_idx]);

  output << fmt::format(
    R"(\node[minimum size=1.2cm,label=above:\huge{}, circle,)"
    R"(line width=0.1cm,color={},fill={},draw] at ({}cm,{}cm) {{}};)" "\n",
    show_irrep_label ? label : "",
    idxs_to_rgb(subirrep_idxs).to_latex(),
    Rgb({240, 245, 250}).to_latex(),
    x,
    y
    );
}

struct BrokenSupermode {
  int superirrep_idx;
  std::vector<int> subirrep_idxs;
};

std::vector<BrokenSupermode> make_broken_supermodes(
  const llvm::ArrayRef<Supermode> supermodes,
  const llvm::ArrayRef<Submode> submodes,
  const SpectrumData& data)
{
  auto get_subdim = [&data](int subirrep_idx) {
    return data.sub_msg.dims[subirrep_idx];
  };
  auto get_superdim = [&data](int superirrep_idx) {
    return data.super_msg.dims[superirrep_idx];
  };

  std::vector<BrokenSupermode> result;

  auto submode_it = submodes.begin();

  for (const auto& supermode : supermodes) {
    BrokenSupermode broken_supermode{
      .superirrep_idx = supermode.superirrep_idx,
      .subirrep_idxs = {}
    };

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
          const Superband& superband,
          const Subband& subband,
          const SpectrumData& data)
  : drawn_subk_idxs{std::move(drawn_subk_idxs)},
  superband{superband},
  subband{subband},
  data{data}
{
  const int max_superirreps_at_fixed_k = std::accumulate(
    superband.k_idx_to_e_idx_to_supermode.begin(),
    superband.k_idx_to_e_idx_to_supermode.end(),
    0,
    [](const auto& l, const auto& r) {
      return std::max(l, static_cast<int>(r.size()));
    }
    );
  superband_height = 2.5 * (max_superirreps_at_fixed_k-1);
  subband_height = 1.3 * superband_height;

}

static void draw_perturbation_label(const std::string& label,
                                    double x1, double y1,
                                    double x2, double y2,
                                    std::ostringstream& out)
{
  out << fmt::format(
    R"(\draw [line width=4pt,arrows={{-Latex[width=20pt,length=20pt]}}])"
    R"(({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)"
    R"(\node [label=left:\huge {}] at ({:3.3f}cm, {:3.3f}cm) {{}};)"
    "\n",
    x1, y1,
    x2, y2,
    label,
    0.5*(x1 + x2),
    (6.0*y1 + 5.0*y2) / 11.0
    );
}

void Visualize::dump(const std::string& output_path)
{
  std::ostringstream debug_code;
  std::ostringstream node_code;
  std::ostringstream line_code;

  {
    double center_x = subk_min_dist * (drawn_subk_idxs.size()-1) / 2.0;
    double center_y = subband_height + 0.5*band_band_separation;


    const std::string perturbation_label = fmt::format(
      R"($\underset{{({})}}{{{}}}\longrightarrow\underset{{({})}}{{{}}}\;$)",
      data.super_msg.number,
      data.super_msg.label,
      data.sub_msg.number,
      data.sub_msg.label
      );

    draw_perturbation_label(perturbation_label,
                            center_x, center_y + 1.5,
                            center_x, center_y - 1.25,
                            line_code);


    // debug_code << fmt::format(
    //   R"(\node [label=AAAA] )"
    //   R"(at ({:3.3f}cm, {:3.3f}cm) {{}};)" "\n",
    //   center_x,
    //   2.0 * center_y
    //   );
    //
  }

  for (int x_idx = 0;
       x_idx < static_cast<int>(drawn_subk_idxs.size());
       ++x_idx)
  {
    visualize_x_idx(x_idx, node_code);

    if (x_idx > 0) {
      visualize_superlines(x_idx-1, x_idx, line_code);
      visualize_sublines(x_idx-1, x_idx, line_code);
    }
  }

  std::string output = latex_template;

  output = std::regex_replace(output,
                              std::regex(R"(DEBUG-CODE)"),
                              debug_code.str()
                             );
  output = std::regex_replace(output,
                              std::regex(R"(EDGE-LAYER-CODE)"),
                              line_code.str()
                             );
  output = std::regex_replace(output,
                              std::regex(R"(NODE-LAYER-CODE)"),
                              node_code.str()
                             );

  std::ofstream(output_path) << output;
}

void Visualize::visualize_x_idx(int x_idx, std::ostringstream& output)
{
  supervisualize_at_x_idx(x_idx, output);
  subvisualize_at_x_idx(x_idx, output);


  const int subk_idx = x_idx_to_subk_idx(x_idx);
  const int superk_idx = x_idx_to_superk_idx(x_idx);

  const std::string subk = data.sub_msg.ks[subk_idx];
  const std::string superk = data.super_msg.ks[superk_idx];

  std::string gk_code;

  std::string g = data.subk_to_g_and_superk.at(subk).first;

  if (g != R"({1|0})") {
    g = std::regex_replace(g,
                           std::regex(R"(([0-9])/([0-9]))"),
                           R"(\frac{$1}{$2})"
                          );
    gk_code = fmt::format(
      R"(label=\Large {{$\{{{}\}}{{\bm{{k}}}}_{{{}}}$}})",
      g.substr(1, g.size()-2),
      superk
      );

  }

  output << fmt::format(
    R"(\node[{}]at({:3.3f}cm,{:3.3f}cm){{}};)" "\n",
    gk_code,
    x_from_x_idx(x_idx),
    subband_height + superband_height + band_band_separation + 1
    );
}

void Visualize::supervisualize_at_x_idx(int x_idx, std::ostringstream& output)
{
  int subk_idx = x_idx_to_subk_idx(x_idx);
  int superk_idx = x_idx_to_superk_idx(x_idx);

  const std::string subk = data.sub_msg.ks[subk_idx];
  const std::string g = data.subk_to_g_and_superk.at(subk).first;


  const auto& supermodes = superband.k_idx_to_e_idx_to_supermode[superk_idx];
  const auto& submodes = subband.subk_idx_to_e_idx_to_submode[subk_idx];
  const auto& broken_supermodes = make_broken_supermodes(
    supermodes,
    submodes,
    data
    );

  for (int e_idx = 0;
       e_idx < static_cast<int>(broken_supermodes.size());
       ++e_idx)
  {
    const auto& broken_supermode = broken_supermodes[e_idx];

    double supermode_x = x_from_x_idx(x_idx);
    double supermode_y =
      subband_height
      + band_band_separation
      + superband_height * scale_idx(e_idx, supermodes.size());

    draw_superirrep(
      supermode_x,
      supermode_y,
      broken_supermode.superirrep_idx,
      g == R"({1|0})",
      broken_supermode.subirrep_idxs,
      data,
      output
      );

    x_idx_to_superirrep_points[x_idx].push_back(
      {supermode_x,
        supermode_y,
        data.super_msg.irreps[broken_supermode.superirrep_idx]
      }
      );

    const int num_subirreps = broken_supermode.subirrep_idxs.size();
    const auto orbit = make_orbit(num_subirreps, pi/2);

    for (int sub_e_idx = 0;
         sub_e_idx < num_subirreps;
         ++sub_e_idx)
    {
      const int subirrep_idx = broken_supermode.subirrep_idxs[sub_e_idx];

      const auto diameter = 0.3;
      double orbit_scale = 1.0;
      if (data.sub_msg.dims[subirrep_idx] > 1 && num_subirreps > 2) {
        orbit_scale = 1.5;
      }

      float theta_shift = 0;
      if (num_subirreps == 3) {
        theta_shift =   -2*pi*(sub_e_idx)/num_subirreps;
      }
      draw_cluster(supermode_x + orbit_scale*0.4*diameter*orbit[sub_e_idx].first,
                   supermode_y + orbit_scale*0.4*diameter*orbit[sub_e_idx].second,
                   diameter,
                   theta_shift,
                   subirrep_idx,
                   data,
                   output);
    }
  }
}

void Visualize::subvisualize_at_x_idx(int x_idx, std::ostringstream& output)
{
  int subk_idx = x_idx_to_subk_idx(x_idx);
  int superk_idx = x_idx_to_superk_idx(x_idx);

  const auto& supermodes = superband.k_idx_to_e_idx_to_supermode[superk_idx];
  const auto& submodes = subband.subk_idx_to_e_idx_to_submode[subk_idx];
  const auto& broken_supermodes = make_broken_supermodes(
    supermodes,
    submodes,
    data
    );

  for (int e_idx = 0;
       e_idx < static_cast<int>(broken_supermodes.size());
       ++e_idx)
  {
    const auto& broken_supermode = broken_supermodes[e_idx];
    double supermode_x = x_from_x_idx(x_idx);
    double supermode_y = subband_height * scale_idx(e_idx, supermodes.size());

    const int num_subirreps = broken_supermode.subirrep_idxs.size();

    if (num_subirreps > 1) {
      output << fmt::format(
        R"(\path [draw, dashed, draw opacity=.5, fill opacity=.4, fill={}])"
        R"(({:3.3f}cm, {:3.3f}cm) ellipse ({:3.3f}cm and {:3.3f}cm);)" "\n",
        Rgb({180, 220, 255}).to_latex(),
        supermode_x,
        supermode_y,
        0.60,
        broken_min_dist * (num_subirreps-1+2) * .5
        );
    }

    for (int sub_e_idx = 0;
         sub_e_idx < num_subirreps;
         ++sub_e_idx)
    {
      const int subirrep_idx = broken_supermode.subirrep_idxs[sub_e_idx];


      const double submode_x{supermode_x};
      double submode_y{supermode_y
        + broken_min_dist*(sub_e_idx-(num_subirreps-1)*0.5)};

      draw_subirrep(submode_x,
                    submode_y,
                    subirrep_idx,
                    sublabel_position(sub_e_idx, num_subirreps),
                    data,
                    output
                   );
      x_idx_to_subirrep_points[x_idx].push_back(
        {submode_x,
          submode_y,
          data.sub_msg.irreps[subirrep_idx]
        }
        );
    }
  }
}

void Visualize::visualize_superlines(int x1_idx, int x2_idx,
                                std::ostringstream& output)
{
  const int superk1_idx = x_idx_to_superk_idx(x1_idx);
  const int superk2_idx = x_idx_to_superk_idx(x2_idx);
  const std::string superk1 = data.super_msg.ks[superk1_idx];
  const std::string superk2 = data.super_msg.ks[superk2_idx];


  const auto& superirrep_to_lineirreps
    = data.super_msg.k1_to_k2_to_irrep_to_lineirreps.at(superk1).at(superk2);

  std::map<std::string, std::pair<std::queue<IrrepPoint>, std::queue<IrrepPoint>>>
    lineirrep_to_left_and_right_queues;

  const auto& irrep_points1 = x_idx_to_superirrep_points.at(x1_idx);
  const auto& irrep_points2 = x_idx_to_superirrep_points.at(x2_idx);

  for (const auto& irrep_point1 : irrep_points1) {
    for (const auto& lineirrep
         : superirrep_to_lineirreps.at(irrep_point1.irrep))
    {
      lineirrep_to_left_and_right_queues[lineirrep].first.push(irrep_point1);
    }
  }

  for (const auto& irrep_point2 : irrep_points2) {
    for (const auto& lineirrep
         : superirrep_to_lineirreps.at(irrep_point2.irrep))
    {
      lineirrep_to_left_and_right_queues[lineirrep].second.push(irrep_point2);
    }
  }

  std::vector<LineMode> line_modes;

  for (auto& [lineirrep,  left_and_right_queues]
       : lineirrep_to_left_and_right_queues)
  {

    auto& [left_queue, right_queue] = left_and_right_queues;
    assert(left_queue.size() == right_queue.size());
    while (!left_queue.empty()) {

      const auto left_irrep_point = left_queue.front();
      const auto right_irrep_point = right_queue.front();
      left_queue.pop();
      right_queue.pop();

      line_modes.push_back(LineMode{
        .left = left_irrep_point,
          .right = right_irrep_point,
          .irrep = lineirrep}
          );
    }
  }
  std::sort(line_modes.begin(),
            line_modes.end()
           );



  int line_mode_idx = -1;
  double next_line_y = -1000;
  for (const auto& line_mode : line_modes) {
    const auto& [center_x, center_y] = line_mode.center();
    if (center_y > next_line_y + 0.3) {
      next_line_y = center_y;
    } else {
      next_line_y += 0.3;
    }

    ++line_mode_idx;
    output << fmt::format(
      R"(\draw[rounded corners=40pt,line width=2,color=gray,opacity=.4]({:3.3f}cm,{:3.3f}cm))"
      R"(--({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)" "\n",
      line_mode.left.x,
      line_mode.left.y,
      center_x,
      next_line_y,//+0.3*(scale_idx(line_mode_idx, line_modes.size())-0.5),
      line_mode.right.x,
      line_mode.right.y
      );
  }
}


void Visualize::visualize_sublines(int x1_idx, int x2_idx,
                                   std::ostringstream& output)
{
  const int subk1_idx = x_idx_to_subk_idx(x1_idx);
  const int subk2_idx = x_idx_to_subk_idx(x2_idx);
  const std::string subk1 = data.sub_msg.ks[subk1_idx];
  const std::string subk2 = data.sub_msg.ks[subk2_idx];


  const auto& subirrep_to_lineirreps
    = data.sub_msg.k1_to_k2_to_irrep_to_lineirreps.at(subk1).at(subk2);

  std::map<std::string, std::pair<std::queue<IrrepPoint>, std::queue<IrrepPoint>>>
    lineirrep_to_left_and_right_queues;

  const auto& irrep_points1 = x_idx_to_subirrep_points.at(x1_idx);
  const auto& irrep_points2 = x_idx_to_subirrep_points.at(x2_idx);

  int line_modes_count = 0;

  for (const auto& irrep_point1 : irrep_points1) {
    for (const auto& lineirrep
         : subirrep_to_lineirreps.at(irrep_point1.irrep))
    {
      lineirrep_to_left_and_right_queues[lineirrep].first.push(irrep_point1);
      ++line_modes_count;
    }
  }

  for (const auto& irrep_point2 : irrep_points2) {
    for (const auto& lineirrep
         : subirrep_to_lineirreps.at(irrep_point2.irrep))
    {
      lineirrep_to_left_and_right_queues[lineirrep].second.push(irrep_point2);
    }
  }


  std::vector<LineMode> line_modes;

  // int color_idx = 1;
  for (auto& [lineirrep,  left_and_right_queues]
       : lineirrep_to_left_and_right_queues)
  {
    // ++color_idx;

    auto& [left_queue, right_queue] = left_and_right_queues;
    assert(left_queue.size() == right_queue.size());
    while (!left_queue.empty()) {

      const auto left_irrep_point = left_queue.front();
      const auto right_irrep_point = right_queue.front();
      left_queue.pop();
      right_queue.pop();

      line_modes.push_back(LineMode{
        .left = left_irrep_point,
          .right = right_irrep_point,
          .irrep = lineirrep}
          );
    }
  }
  std::sort(line_modes.begin(),
            line_modes.end()
           );


  int line_mode_idx = -1;
  double next_line_y = -1000;
  for (const auto& line_mode : line_modes) {
    const auto& [center_x, center_y] = line_mode.center();
    if (center_y > next_line_y + 0.3) {
      next_line_y = center_y;
    } else {
      next_line_y += 0.3;
    }

    ++line_mode_idx;
    output << fmt::format(
      R"(\draw[rounded corners=40pt,line width=2,color=gray,opacity=.4]({:3.3f}cm,{:3.3f}cm))"
      R"(--({:3.3f}cm,{:3.3f}cm)--({:3.3f}cm,{:3.3f}cm);)",
      line_mode.left.x,
      line_mode.left.y,
      center_x,
      next_line_y,
      line_mode.right.x,
      line_mode.right.y
      );
  }
}


int Visualize::x_idx_to_subk_idx(int x_idx)
{
  const auto subk_idx = drawn_subk_idxs[x_idx];

  return subk_idx;
}

int Visualize::x_idx_to_superk_idx(int x_idx)
{
  const auto subk_idx = x_idx_to_subk_idx(x_idx);
  const auto superk_idx = data.subk_idx_to_superk_idx(subk_idx);
  return superk_idx;
}

double Visualize::x_from_x_idx(int x_idx)
{
  return x_idx * subk_min_dist;
}


} // namespace TopoMagnon
