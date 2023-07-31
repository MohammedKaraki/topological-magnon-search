#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

#include "spectrum_data.hpp"
#include "k_path.hpp"

namespace TopoMagnon {

static bool is_high_symmetry_line(int i, int j,
                                  const SpectrumData::Msg& msg)
{
  std::string k1 = msg.ks[i];
  std::string k2 = msg.ks[j];

  std::vector<std::string> all_lineirreps;
  const auto& irrep_to_lineirreps
    = msg.k1_to_k2_to_irrep_to_lineirreps.at(k1).at(k2);
  for (const auto& [_, lineirreps] : irrep_to_lineirreps) {
    all_lineirreps.insert(all_lineirreps.end(),
                          lineirreps.begin(),
                          lineirreps.end()
                         );
  }

  std::sort(all_lineirreps.begin(), all_lineirreps.end());
  all_lineirreps.erase(std::unique(all_lineirreps.begin(),
                                   all_lineirreps.end()),
                       all_lineirreps.end()
                      );

  return all_lineirreps.size() > 1;
}

std::vector<int> k_idxs_path(const SpectrumData::Msg& msg, bool all_edges)
{
  std::vector<int> k_idxs;

  std::map<int, std::vector<int>> remaining_edges;

  int num_ks = msg.ks.size();

  for (int i = 0; i < num_ks; ++i) {
    for (int j = 0; j < num_ks; ++j) {
      if (i == j) {
        continue;
      };

      assert(is_high_symmetry_line(i, j, msg)
             == is_high_symmetry_line(j, i, msg));

      if (!all_edges && !is_high_symmetry_line(i, j, msg)) {
        continue;
      }

      remaining_edges[i].push_back(j);
    }
  }

  auto find_next_point = [&remaining_edges]() {
    for (const auto& [left, rights] : remaining_edges) {
      if (!rights.empty()) {
        return left;
      }
    }
    return -1;
  };

  while (find_next_point() != -1) {
    k_idxs.push_back(find_next_point());

    while (!remaining_edges.at(k_idxs.back()).empty()) {
      const int curr_k = k_idxs.back();
      const int next_k = remaining_edges.at(curr_k).front();
      k_idxs.push_back(next_k);
      remaining_edges[curr_k].erase(
        std::remove(remaining_edges.at(curr_k).begin(),
                    remaining_edges.at(curr_k).end(),
                    next_k),
        remaining_edges.at(curr_k).end()
        );
      remaining_edges[next_k].erase(
        std::remove(remaining_edges.at(next_k).begin(),
                    remaining_edges.at(next_k).end(),
                    curr_k),
        remaining_edges.at(next_k).end()
        );
    }
  }

  return k_idxs;
}

void complement_subk_idxs(std::vector<int>& subk_idxs,
                          const SpectrumData::Msg& msg)
{
  for (int i = 0; i < static_cast<int>(msg.ks.size()); ++i) {
    if (std::find(subk_idxs.begin(), subk_idxs.end(), i)
        == subk_idxs.end())
    {
      subk_idxs.push_back(i);
    }
  }
}

} // namespace TopoMagnon
