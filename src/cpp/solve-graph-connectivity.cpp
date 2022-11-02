#include <vector>
#include <numeric>
#include <regex>
#include <algorithm>
#include <set>
#include <type_traits>
#include <cassert>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>
#include <tuple>
#include <stdexcept>

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/algorithm/string/join.hpp>

#include <Eigen/Core>
#include <Eigen/LU>

#include <robin_hood.h> // high performance hash table

#include <fmt/ostream.h>
#include <fmt/format.h>


namespace x3 = boost::spirit::x3;

using Matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>;
using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>;

struct IntMatrixLess {
  bool operator()(const IntMatrix& lhs, const IntMatrix& rhs) const
  {
    if (lhs.rows() != rhs.rows()) {
      return lhs.rows() < rhs.rows();
    }

    if (lhs.cols() != rhs.cols()) {
      return lhs.cols() < rhs.cols();
    }

    for (auto i = 0u; i < lhs.size(); ++i) {
      if (lhs.data()[i] != rhs.data()[i]){
        return lhs.data()[i] < rhs.data()[i];
      }
    }
    return false;
  }
};

using IntMatrixSet = std::set<IntMatrix, IntMatrixLess>;


// Parse compatibility relations text file and return a hash table
// representation.
auto parse_input(const std::string& input)
{
  using x3::int_, x3::alpha, x3::alnum;
  using x3::ascii::space;

  auto relations = robin_hood::unordered_map<std::string,
                                      robin_hood::unordered_map<std::string,
                                                         int> >{};
  auto point_irrep_num_copies = robin_hood::unordered_map<std::string, int>{};

  auto insert_rel = [&](const auto& ctx) {
    const auto attr = x3::_attr(ctx);
    const auto cur_point_label = attr.first.first;
    const auto cur_point_irrep_copies = attr.first.second.value_or(1);
    const auto line_map = attr.second;

    if (point_irrep_num_copies.contains(cur_point_label)
        && point_irrep_num_copies.at(cur_point_label) != cur_point_irrep_copies)
    {
      throw std::runtime_error(fmt::format("[Parsing Error]: "
                                           "Conflicting specifications of "
                                           "the number of copies of the irrep "
                                           "'{}'",
                                           cur_point_label));
    }
    point_irrep_num_copies[cur_point_label] = cur_point_irrep_copies;

    for (auto& [line_copies, line_label] : line_map) {
      if (!relations[cur_point_label].contains(line_label)) {
        relations[cur_point_label][line_label] = line_copies.value_or(1);
      } else {
        relations[cur_point_label][line_label] += line_copies.value_or(1);
      }
    }
  };

  using IrrepLabel = std::string;
  using PointIrrepLabel = std::pair<IrrepLabel, std::optional<int>>;
  using LineIrrepLabel = std::pair<std::optional<int>, IrrepLabel>;
  using LineIrrepLabels = std::vector<LineIrrepLabel>;

  auto irrep_label = x3::rule<class irrep_label, IrrepLabel>{}
                   = x3::lexeme[alpha >> *alnum];
  auto point_irrep_label = x3::rule<class point_irrep_label,
                                    std::pair<IrrepLabel,
                                              std::optional<int>>>{}
                         = irrep_label >> -('(' >> int_ >> ')');
  auto line_irrep_label = x3::rule<class line_irrep_label, LineIrrepLabel>{}
                        = -int_ >> irrep_label;
  auto line_irrep_labels = x3::rule<class line_irrep_labels, LineIrrepLabels>{}
                         = line_irrep_label >> *('+' >> line_irrep_label);
  auto comp_rel = x3::rule<class comp_rel,
                           std::pair<PointIrrepLabel, LineIrrepLabels> >{}
                = point_irrep_label >> "->" >> line_irrep_labels;


  auto irreps = std::vector<std::string>{};
  auto begin = input.begin();
  auto end = input.end();
  if (!x3::phrase_parse(begin,
                        end,
                        -(comp_rel[insert_rel]) % ';',
                        space)
      || begin != end)
  {
    throw std::runtime_error("Parsing Error");
  }

  return std::pair(relations, point_irrep_num_copies);
}

auto extract_point_label(const std::string& point_irrep_label)
{
  auto sm = std::smatch{};
  std::regex_search(point_irrep_label,
                    sm,
                    std::regex{R"(^[a-zA-Z]\{1,\})", std::regex::grep});
  return std::string{sm[0]};
}

auto construct_point_irrep_vecs(const auto& relations,
                                const auto& point_irrep_counts)
{
  auto table = robin_hood::unordered_map<std::string, int>{};
  for (const auto& [point_irrep_label, _] : relations) {
    table[point_irrep_label] = point_irrep_counts.at(point_irrep_label);
  }

  auto result = robin_hood::unordered_map<std::string,
                                   std::vector<std::string>>{};
  for (const auto& [irrep_label, count] : table) {
    auto point_label = extract_point_label(irrep_label);
    auto& point_vec = result[point_label];

    point_vec.insert(point_vec.end(), count, irrep_label);
  }

  return result;
}

auto construct_copies_seqs(const auto& relations, const auto& point_irrep_vecs)
{
  auto result = robin_hood::unordered_map<std::string,
                                   robin_hood::unordered_map<std::string,
                                                      std::vector<int>>>{};

  for (const auto& [point_label, point_irrep_vec] : point_irrep_vecs) {
    // Relevant line irreps are all the irreps connected to any irrep residing
    // inside the current point being considered.
    auto relevant_line_irreps = std::set<std::string>{};
    for (const auto& point_irrep_label : point_irrep_vec) {
      for (const auto& [line_irrep_label, _] : relations.at(point_irrep_label))
      {
        relevant_line_irreps.insert(line_irrep_label);
      }
    }

    for (const auto& line_irrep_label : relevant_line_irreps) {
      auto copies_seq = std::vector<int>{};
      copies_seq.reserve(point_irrep_vec.size());

      for (const auto& point_irrep_label : point_irrep_vec) {
        if (relations.at(point_irrep_label).contains(line_irrep_label)) {
          copies_seq.push_back(relations.at(point_irrep_label)
                                        .at(line_irrep_label));
        }
        else {
          copies_seq.push_back(0);
        }
      }

      result[point_label][line_irrep_label] = copies_seq;
    }
  }

  return result;
}

auto find_lirrep_copy_count(const auto& line_irrep_label,
                            const auto& lirrep_copies_seq_table,
                            const auto& lirrep_labels_table)
{
  for (const auto& [point_label, _] : lirrep_labels_table) {
    if (lirrep_copies_seq_table.at(point_label).contains(line_irrep_label))
    {
      const auto& count_seq =
        lirrep_copies_seq_table.at(point_label).at(line_irrep_label);
      return std::accumulate(count_seq.begin(), count_seq.end(), 0u);;
    }
  }

  throw std::runtime_error("Line irrep not connected to any point irrep");
}

auto construct_lirrep_labels(const auto& relations)
{
  auto result = std::set<std::string>{};
  for (const auto& [_, line_count_map] : relations) {
    for (const auto& [line_irrep_label, _] : line_count_map) {
      result.insert(line_irrep_label);
    }
  }

  return result;
}

auto construct_lirrep_copy_count_table(const auto& relations,
                                       const auto& lirrep_copy_seq_table,
                                       const auto& point_irreps_vecs)
{
  auto result = robin_hood::unordered_map<std::string, int>{};

  const auto lirrep_labels = construct_lirrep_labels(relations);

  for (const auto& lirrep_label : lirrep_labels) {
    result[lirrep_label] = find_lirrep_copy_count(lirrep_label,
                                                  lirrep_copy_seq_table,
                                                  point_irreps_vecs);
  }

  return result;
}

auto construct_column_idx_table(const auto& lirrep_count_table)
{
  auto lirrep_and_count_arr = std::vector<std::pair<std::string, int>>{};
  for (const auto& [lirrep_label, count] : lirrep_count_table) {
    lirrep_and_count_arr.push_back(std::pair(lirrep_label, count));
  }
  std::sort(lirrep_and_count_arr.begin(),
            lirrep_and_count_arr.end(),
            [](const auto& l, const auto& r) {
              return l.first < r.first;
            });
  assert(!lirrep_and_count_arr.empty());

  auto result = robin_hood::unordered_map<std::string, std::size_t>{};
  result[lirrep_and_count_arr.front().first] = 0;
  for (auto i = 1u; i < lirrep_and_count_arr.size(); ++i) {
    result[lirrep_and_count_arr[i].first] =
      result[lirrep_and_count_arr[i-1].first]
      + lirrep_and_count_arr[i-1].second;
  }
  return result;
}

auto construct_point_num_irreps(const auto& point_irrep_vec_table)
{
  auto result = robin_hood::unordered_map<std::string, int>{};
  for (const auto& [point_label, irrep_labels_vec] : point_irrep_vec_table) {
    result[point_label] = irrep_labels_vec.size();
  }
  return result;
}

/*
   Sample return:
   {"gm" : 0,
    "m"  : 2,
    "k"  : 4}
*/
auto construct_row_idx_table_and_row_names(const auto& point_irrep_vec_table)
{
  auto point_and_point_irrep_count = std::vector<std::pair<std::string, int>>{};
  for (const auto& [point_label, point_vec] : point_irrep_vec_table) {
    point_and_point_irrep_count.push_back(
      std::pair(point_label, point_vec.size()));
  }

  std::sort(point_and_point_irrep_count.begin(),
            point_and_point_irrep_count.end(),
            [](const auto& l, const auto& r) {
              return l.first < r.first;
            });

  assert(!point_and_point_irrep_count.empty());

  auto result1 = robin_hood::unordered_map<std::string, int>{};
  result1[point_and_point_irrep_count.front().first] = 0;
  for (auto i = 1u; i < point_and_point_irrep_count.size(); ++i) {
    result1[point_and_point_irrep_count[i].first] =
      result1[point_and_point_irrep_count[i-1].first]
      + point_and_point_irrep_count[i-1].second;
  }

  auto result2 = std::vector<std::string>{};
  for (const auto& [point_label, count] : point_and_point_irrep_count) {
    const auto& irrep_labels = point_irrep_vec_table.at(point_label);
    result2.insert(result2.end(), irrep_labels.begin(), irrep_labels.end());
  }

  return std::pair(result1, result2);
}

class SymmetryData {
public:
  SymmetryData(const std::string& input)
    : relations_and_pirrep_count{parse_input(input)},
    relations{relations_and_pirrep_count.first},
    pirrep_count{relations_and_pirrep_count.second},
    point_irrep_vecs{construct_point_irrep_vecs(relations, pirrep_count)},
    copies_seqs{construct_copies_seqs(relations, point_irrep_vecs)},
    lirrep_copies_count_table{construct_lirrep_copy_count_table(
    relations,
    copies_seqs,
    point_irrep_vecs)},
    column_idx_table{construct_column_idx_table(lirrep_copies_count_table)},
    point_num_irreps{construct_point_num_irreps(point_irrep_vecs)},
    row_idx_table_and_row_names{
      construct_row_idx_table_and_row_names(point_irrep_vecs)},
    row_idx_table{row_idx_table_and_row_names.first},
    row_names{row_idx_table_and_row_names.second}
  { }

  const auto& get_line_irrep_copies() { return lirrep_copies_count_table; }
  const auto& get_column_idx() { return column_idx_table; }
  const auto& get_point_num_irreps() { return point_num_irreps; }
  const auto& get_point_irreps() { return point_irrep_vecs; }
  const auto& get_row_idxs() { return row_idx_table; }
  const auto& get_row_names() { return row_names; }
  const auto& get_copies_seqs() { return copies_seqs; }

private:
  const std::pair<robin_hood::unordered_map<std::string,
        robin_hood::unordered_map<std::string, int>>,
        robin_hood::unordered_map<std::string, int>> relations_and_pirrep_count;
  const robin_hood::unordered_map<std::string,
        robin_hood::unordered_map<std::string, int>>& relations;
  const robin_hood::unordered_map<std::string, int>& pirrep_count;
  const robin_hood::unordered_map<std::string, std::vector<std::string>>
    point_irrep_vecs;
  const robin_hood::unordered_map<std::string,
        robin_hood::unordered_map<std::string, std::vector<int>>> copies_seqs;
  const robin_hood::unordered_map<std::string, int> lirrep_copies_count_table;
  const robin_hood::unordered_map<std::string, std::size_t> column_idx_table;
  const robin_hood::unordered_map<std::string, int> point_num_irreps;
  const std::pair<robin_hood::unordered_map<std::string, int>,
        std::vector<std::string>> row_idx_table_and_row_names;
  const robin_hood::unordered_map<std::string, int>& row_idx_table;
  const std::vector<std::string>& row_names;
};

bool is_zero(float x)
{
  constexpr auto cutoff = 1.0e-10f;
  if (std::abs(x) < cutoff) {
    return true;
  }

  return false;
}

void make_rref_process(auto& matrix, auto row_p, auto col_p)
{
  matrix.row(row_p) /= matrix(row_p, col_p);

  for (auto row = 0u; row < matrix.rows(); ++row) {
    if (row != row_p) {
      matrix.row(row) -= matrix(row, col_p) * matrix.row(row_p).eval();
    }
  }
}

void make_rref(Matrix& matrix)
{
  auto processed_rows = std::set<unsigned int>{};
  for (auto col = 0u; col < matrix.cols(); ++col) {
    for (auto row = 0u; row < matrix.rows(); ++row) {
      if (!is_zero(matrix(row, col)) && !processed_rows.contains(row)) {
        make_rref_process(matrix, row, col);
        processed_rows.insert(row);
        break;
      }
    }
  }

  for (auto i = 0u; i < matrix.size(); ++i) {
    auto element = matrix.data()[i];
    assert(is_zero(element) || is_zero(element - 1.0));
  }
}

auto conn_set = IntMatrixSet{};

static void laplacize(IntMatrix& matrix)
{
  for (auto row = 0u; row < matrix.rows(); ++row) {
    matrix(row, row) = 0;
    auto sum = 0;
    for (auto col = 0u; col < matrix.cols(); ++col) {
      sum += matrix(row, col);
    }
    matrix(row, row) = -sum;
  }
}


void generate(const auto& idxs_permutation,
              const auto& copies_seq,
              auto line_irrep_dim,
              auto& matrix)
{
    matrix.setZero();

    auto cur_col = 0u;
    for (const auto idx : idxs_permutation) {
      auto copies = copies_seq[idx];
      for (auto i = 0; i < copies; ++i) {
        matrix(idx, cur_col++) = line_irrep_dim;
      }
    }
}


auto make_branch(const IntMatrix& vec, const auto& row_names)
{
  assert(vec.rows() == 1);
  assert(vec.cols() == static_cast<long>(row_names.size()));

  auto result = std::multiset<std::string>{};
  for (auto i = 0u; i < vec.size(); ++i) {
    if (vec(i) == 1) {
      result.insert(row_names[i]);
    }
  }

  return result;
}

auto make_branches_set(const IntMatrix& vecs, const auto& row_names)
{
  assert(vecs.cols() == static_cast<long>(row_names.size()));

  auto result = std::set<std::multiset<std::string>>{};
  for (auto i = 0u; i < vecs.rows(); ++i) {
    result.insert(make_branch(vecs.row(i), row_names));
  }

  return result;
}


auto gen_point_matrices(const std::string& k_point,
                        const auto& copies_seq,
                        const auto& point_num_irreps,
                        const auto& line_irrep_copies,
                        const auto& point_irreps,
                        const auto& column_idx)
{
  const auto& k_point_copies_seq = copies_seq.at(k_point);

  auto num_rows = point_num_irreps.at(k_point);
  auto num_cols = std::accumulate(line_irrep_copies.begin(),
                                  line_irrep_copies.end(),
                                  0,
                                  [](const auto sum, const auto& pair) {
                                    return sum + pair.second;
                                  });

  auto idxs_perm = std::vector<int>(num_rows);
  std::iota(idxs_perm.begin(), idxs_perm.end(), 0);



  auto result = IntMatrixSet{};

  auto considered_labels_perm = std::set<std::vector<std::string>>{};
  do {

    // if the same labels permutation happens to arise from a different
    // idxs permutation, then skip:
    auto curr_labels_perm = std::vector<std::string>(
      point_num_irreps.at(k_point));
    for (auto i = 0; i < point_num_irreps.at(k_point); ++i) {
      curr_labels_perm[i] = point_irreps.at(k_point)[idxs_perm[i]];
    }
    if (considered_labels_perm.contains(curr_labels_perm)) {
      // fmt::print("******************{}**************\n", k_point);
      continue;
    }
    considered_labels_perm.insert(curr_labels_perm);

    IntMatrix matrix = IntMatrix::Zero(num_rows, num_cols);
    for (const auto& [line_irrep_label, cur_copies_seq] : k_point_copies_seq) {
      auto output_block = matrix.block(0,
                                       column_idx.at(line_irrep_label),
                                       num_rows,
                                       line_irrep_copies.at(line_irrep_label));
      generate(idxs_perm,
               cur_copies_seq,
               1, //line_irrep_dim.at(line_irrep_label),
               output_block);
    }

    result.insert(matrix);
  } while (std::next_permutation(idxs_perm.begin(), idxs_perm.end()));

  return result;
}




bool advance(auto& curs, const auto& begins, const auto& ends)
{
  assert(!curs.empty());
  assert(curs.size() == begins.size() && begins.size() == ends.size());
  auto N = curs.size();

  ++curs.back();
  for (auto i = N-1; i > 0; --i) {
    if (curs[i] == ends[i]) {
      ++curs[i-1];
      for (auto j = i; j < N; ++j) {
        curs[j] = begins[j];
      }
    }
  }

  if (curs.front() == ends.front()) {
    return false;
  }
  return true;
}

void each_combination(const auto& matrix_sets, auto func)
{
  using MatrixSetIterator = typename decltype(
    matrix_sets.begin()->second)::iterator;

  auto labels = std::vector<std::string>{};
  auto begins = std::vector<MatrixSetIterator>{};
  auto ends = std::vector<MatrixSetIterator>{};

  for (const auto& [point_label, set] : matrix_sets) {
    labels.push_back(point_label);
    begins.push_back(set.begin());
    ends.push_back(set.end());
  }

  auto curs = begins;
  do {
    func(labels, curs);
  } while (advance(curs, begins, ends));
}

int main()
{
  auto ss = std::ostringstream{};
  ss << std::cin.rdbuf();
  SymmetryData data{ss.str()};
  const auto line_irrep_copies = data.get_line_irrep_copies();
  const auto line_total_cols = std::accumulate(line_irrep_copies.begin(),
                                               line_irrep_copies.end(),
                                               0,
                                               [](auto sum, const auto& r) {
                                                 return sum + r.second;
                                               });
  const auto column_idx = data.get_column_idx();
  const auto point_num_irreps = data.get_point_num_irreps();
  const auto point_irreps = data.get_point_irreps();
  const auto row_idxs = data.get_row_idxs();
  const auto points = [&]{
    auto result = robin_hood::unordered_set<std::string>{};
    for (const auto& [key, _] : row_idxs) {
      result.insert(key);
    }
    return result;
  }();
  const auto row_names = data.get_row_names();
  const auto point_total_rows = std::accumulate(point_num_irreps.begin(),
                                                point_num_irreps.end(),
                                                0,
                                                [](auto sum, const auto& r) {
                                                  return sum + r.second;
                                                });
  const auto copies_seq = data.get_copies_seqs();





  auto k_point_matrix_sets = robin_hood::unordered_map<std::string,
       IntMatrixSet>{};
  for (const auto& point : points) {
    k_point_matrix_sets[point] = gen_point_matrices(point,
                                                    copies_seq,
                                                    point_num_irreps,
                                                    line_irrep_copies,
                                                    point_irreps,
                                                    column_idx);
  }

  auto iters = std::vector<
    decltype(k_point_matrix_sets.begin()->second)::iterator
    >{};

  each_combination(k_point_matrix_sets,
                   [&, k=0] (const auto& labels, const auto& curs) mutable {
                     // fmt::print("-------------{}-----------\n", k++);

                     auto block_matrix = IntMatrix(
                       IntMatrix::Zero(point_total_rows,
                                       line_total_cols));

                     for (auto i = 0u; i < labels.size(); ++i) {
                       block_matrix.block(row_idxs.at(labels[i]),
                                         0,
                                         point_num_irreps.at(labels[i]),
                                         line_total_cols) = *curs[i];
                     }

                     // fmt::print("{}\n\n", block_matrix);

                     const auto aa_rows = point_total_rows;
                     const auto aa_cols = aa_rows;

                     const auto bb_rows = line_total_cols;
                     const auto bb_cols = bb_rows;

                     auto full_matrix = IntMatrix(aa_rows+bb_rows,
                                                  aa_cols+bb_cols);

                     full_matrix.block(0, 0, aa_rows, aa_cols).setZero();
                     full_matrix.block(aa_rows, aa_cols, bb_rows, bb_cols)
                       .setZero();

                     full_matrix.block(0, aa_cols, aa_rows, bb_cols)
                       = block_matrix;

                     full_matrix.block(aa_rows, 0, bb_rows, aa_cols)
                       = block_matrix.transpose();

                     laplacize(full_matrix);
                     // fmt::print("{}\n\n", full_matrix);


                     auto lu = Eigen::FullPivLU<Matrix>(
                       full_matrix.cast<float>());

                     auto kernel = Matrix(lu.kernel().transpose());
                     kernel = kernel.block(0,
                                           0,
                                           kernel.rows(),
                                           point_total_rows
                                           ).eval();
                     // fmt::print("{}\n\n", kernel);
                     for (auto row = 0u; row < kernel.rows(); ++row) {
                       for (auto col = 0u; col < kernel.cols(); ++col) {
                        kernel(row, col) += 0.5f;
                       }
                     }

                     auto int_kernel = IntMatrix(kernel.cast<int>());
                     conn_set.insert(int_kernel);
                     // fmt::print("{}\n\n", int_kernel);
                   });


  auto connectivies_set = std::set<std::set<std::multiset<std::string>>>{};

  for (const auto& conn : conn_set) {
    auto branches = make_branches_set(conn, row_names);
    connectivies_set.insert(branches);
  }

  for (const auto& branches : connectivies_set){
    for (const auto& branch : branches) {
      for (const auto& point_label : branch) {
        fmt::print("{} ", point_label);
      }
      fmt::print("\n");
    }
    fmt::print("\n");
  }
}
