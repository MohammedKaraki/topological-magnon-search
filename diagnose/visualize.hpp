#ifndef VISUALIZE_HPP
#define VISUALIZE_HPP


#include <vector>
#include <cassert>
#include <map>
#include <utility>
#include <string>

namespace TopoMagnon {

struct Rgb {
  double r, g, b;

  std::string to_latex() const;
};


class Superband;
class Subband;
struct SpectrumData;

struct IrrepPoint {
  double x, y;
  std::string irrep;
};

enum class LabelPosition {
  NoLabel, Above, Right, Below, Left
};
enum class VisMode {
  Normal, Compact
};

struct VisSpec {
  double band_band_separation = 4.5;
  double subk_min_dist = 2.9;
  double broken_min_dist = 0.9;
  double subband_superband_ratio = 1.0;
  double supermode_separation = 3.5;
  int skip_color = 0;
};


std::pair<VisMode, VisSpec>
mode_spec_pair_from_file(const std::string& filename);

class Visualize {
public:
  Visualize(std::vector<int> drawn_subk_idxs,
            const Superband& superband,
            const Subband& subband,
            const SpectrumData& data,
            VisMode mode,
            VisSpec spec);

  void dump(const std::string& filename);

private:
  void supervisualize_at_x_idx(int x_idx, std::ostringstream& output);
  void subvisualize_at_x_idx(int x_idx, std::ostringstream& output);
  void visualize_superlines(int x1_idx, int x2_idx, std::ostringstream& output);
  void visualize_sublines(int x1_idx, int x2_idx, std::ostringstream& output);
  void visualize_separation(std::ostringstream& output);
  void annotate(int x_idx, std::ostringstream& output);

private:
  int x_idx_to_subk_idx(int x_idx);
  int x_idx_to_superk_idx(int x_idx);
  double x_from_x_idx(int x_idx);
  // double y_from_superk_idx_and_e_idx(int superk_idx, int e_idx);
  // double y_from_subk_idx_and_e_idx(int subk_idx, int e_idx);

  double subband_y_min = 0.0, subband_y_max = 0.0;

  LabelPosition sublabel_position(int sub_e_idx, int num_sub_irreps) const;

  Rgb idx_to_rgb(int idx);
  Rgb idxs_to_rgb(const std::vector<int>& idxs);
  void draw_cluster(double x,
                    double y,
                    double dot_diameter,
                    double theta_shift,
                    int subirrep_idx,
                    std::ostringstream& output
                   );
  void draw_subirrep(double x,
                     double y,
                     int subirrep_idx,
                     LabelPosition label_position,
                     std::ostringstream& output
                    );
  void draw_superirrep(double x,
                       double y,
                       LabelPosition label_pos,
                       int superirrep_idx,
                       bool show_irrep_label,
                       const std::vector<int>& subirrep_idxs,
                       std::ostringstream& output
                      );

private:
  std::map<int, std::vector<IrrepPoint>>
    x_idx_to_subirrep_points,
    x_idx_to_superirrep_points;


private:
  std::vector<int> drawn_subk_idxs;
  const Superband& superband;
  const Subband& subband;
  const SpectrumData& data;


  double superband_height;
  double subband_height;

  VisMode mode;
  VisSpec spec;
};


} // namespace TopoMagnon


#endif // VISUALIZE_HPP
