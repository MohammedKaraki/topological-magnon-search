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

class Visualize {
public:
  Visualize(std::vector<int> drawn_subk_idxs,
            const Superband& superband,
            const Subband& subband,
            const SpectrumData& data);

  void dump(const std::string& filename);

private:
  void visualize_x_idx(int x_idx, std::ostringstream& output);
  void supervisualize_at_x_idx(int x_idx, std::ostringstream& output);
  void subvisualize_at_x_idx(int x_idx, std::ostringstream& output);
  void visualize_superlines(int x1_idx, int x2_idx, std::ostringstream& output);
  void visualize_sublines(int x1_idx, int x2_idx, std::ostringstream& output);

private:
  int x_idx_to_subk_idx(int x_idx);
  int x_idx_to_superk_idx(int x_idx);
  double x_from_x_idx(int x_idx);
  // double y_from_superk_idx_and_e_idx(int superk_idx, int e_idx);
  // double y_from_subk_idx_and_e_idx(int subk_idx, int e_idx);


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
  double band_band_separation = 4.5;
  double subk_min_dist = 2.9;
  double broken_min_dist = .8;
};


} // namespace TopoMagnon


#endif // VISUALIZE_HPP
