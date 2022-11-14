#include <regex>
#include <sstream>
#include <cassert>
#include <fmt/core.h>

#include "latexify.hpp"


namespace TopoMagnon {

Littleirrep::Littleirrep(std::string source_str) : src{std::move(source_str)}
{
  const std::regex regex{R"((([A-Z]+).*)\(([0-9]+)\))"};
  std::smatch match;
  assert(std::regex_match(this->src, match, regex));

  irreplabel = match[1];
  ksymbol = match[2];
  dim = match[3];
}

std::string Littleirrep::latexify() const
{
  return std::regex_replace(irreplabel, std::regex(R"(GM)"), R"(\Gamma)");
}

Irrep1wp::Irrep1wp(std::string src)
{
  const std::regex regex{R"(\(([^,]+),([^,]+)\))"};
  std::smatch match;
  assert(std::regex_match(src, match, regex));

  irrep1 = match[1];
  wp = match[2];
}

const std::string Irrep1wp::to_str() const
{
  return "(" + irrep1 + "," + wp + ")";
}

std::string Irrep1wp::latexify() const
{
  return fmt::format(R"({{({})}}_{{{}}})",
                     irrep1,
                     wp
                    );
}


Irrep12wp::Irrep12wp(std::string source_str) : src{std::move(source_str)}
{
  const std::regex regex{R"(\(([^,]+),([^,]+),([^,]+)\))"};
  std::smatch match;
  assert(std::regex_match(src, match, regex));

  irrep1 = match[1];
  irrep2 = match[2];
  wp = match[3];
}

std::string Irrep12wp::latexify() const
{
  return fmt::format(R"(({0}({2})\oplus {1}({2}))\uparrow G)",
                     irrep1,
                     irrep2,
                     wp
                    );
}


std::string Irrep1wpDecomp::latexify() const
{
  std::ostringstream result;
  bool past_first = false;
  for (auto it = comps.begin(); it != comps.end(); ) {
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

Irrep12wpDecomp::Irrep12wpDecomp(const std::vector<std::string>& srcs)
{
  for (const auto& src : srcs) {
    irrep12wp_comps.insert(Irrep12wp(src));
  }
}

static std::set<std::multiset<Irrep1wp>>
particle_irrep1wp_decomps_helper(const std::multiset<Irrep1wp>& cur_decomp,
                                 std::multiset<Irrep12wp> remaining_irrep12wps
                                )
{
  if (remaining_irrep12wps.empty()) {
    return {cur_decomp};
  }


  Irrep12wp next_irrep12wp = *remaining_irrep12wps.begin();
  remaining_irrep12wps.erase(remaining_irrep12wps.begin());

  std::set<std::multiset<Irrep1wp>> result;

  {
    std::multiset<Irrep1wp> new_decomp = cur_decomp;
    new_decomp.insert(next_irrep12wp.get_irrep1wp());
    for (const auto& decomp
         : particle_irrep1wp_decomps_helper(new_decomp, remaining_irrep12wps))
    {
      result.insert(decomp);
    }
  }

  if (next_irrep12wp.get_irrep1wp() != next_irrep12wp.get_irrep2wp()) {
    std::multiset<Irrep1wp> new_decomp = cur_decomp;
    new_decomp.insert(next_irrep12wp.get_irrep2wp());
    for (const auto& decomp
         : particle_irrep1wp_decomps_helper(new_decomp, remaining_irrep12wps))
    {
      result.insert(decomp);
    }
  }

  return result;
}

std::set<Irrep1wpDecomp>
Irrep12wpDecomp::find_all_magnon_irrep1wp_decomps() const
{
  std::set<Irrep1wpDecomp> result;

  for (auto& comps : particle_irrep1wp_decomps_helper({}, irrep12wp_comps)) {
    result.insert(Irrep1wpDecomp(comps));
  }

  return result;
}

std::string Irrep12wpDecomp::latexify() const
{
  std::multiset<Irrep1wp> as_irrep1wps;
  for (const auto& irrep12wp : irrep12wp_comps) {
    as_irrep1wps.insert(irrep12wp.get_irrep1wp());
    as_irrep1wps.insert(irrep12wp.get_irrep2wp());
  }

  std::ostringstream result;
  bool past_first = false;
  for (auto it = as_irrep1wps.begin(); it != as_irrep1wps.end(); ) {
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

LittleirrepDecomp::LittleirrepDecomp(const std::vector<std::string>& srcs)
{
  for (const auto& src : srcs) {
    littleirrep_comps.insert(Littleirrep(src));
  }
}

std::string LittleirrepDecomp::latexify() const
{
  std::ostringstream result;

  int terms_counter = 1;

  for (auto it = littleirrep_comps.begin(); it != littleirrep_comps.end(); ) {
    if (terms_counter % 9 == 0) {
      result << R"(\\)" "\n" R"(&\quad )";
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


std::vector<std::string> LittleirrepDecomp::get_irreps_as_strs() const
{
  std::vector<std::string> result;

  for (const auto& littleirrep : littleirrep_comps) {
    result.push_back(littleirrep.get_irreplabel());
  }

  return result;
}

std::string latexify_greeks(const std::string& label)
{
  return
    std::regex_replace(label, std::regex(R"(GM)"), R"(\Gamma)");
}

std::string latexify_row(const IntMatrix& ints,
                         const std::vector<std::string>& strs)
{
  assert(ints.size() == static_cast<int>(strs.size()));
  assert(ints.size() > 0);

  bool nonzero_factor_found = false;

  std::ostringstream result;

  for (int i = 0; i < ints.size(); ++i)
  {
    const int x = ints(i);
    const std::string& s = fmt::format(R"(n^{{{}}})",
                                       latexify_greeks(strs[i])
                                      );

    if (x != 0) {
      nonzero_factor_found = true;

      if (x == 1) {
        result << '+' << s;
      }
      else if (x == -1) {
        result << '-' << s;
      }
      else {
        if (x > 0) {
          result << '+';
        }
        result << x << s;
      }
    }
  }

  assert(nonzero_factor_found);

  return result.str();
}

std::string latexify_matrix(const IntMatrix& matrix,
                            const std::vector<std::string>& strs,
                            bool sis)
{
  std::ostringstream result;

  result << R"(\begin{align})" << '\n';

  for (int i = 0; i < matrix.rows(); ++i) {
    if (sis) {
      result << fmt::format(R"(z_{{{}}} &= {})",
                            i + 1,
                            latexify_row(matrix.row(i), strs)
                           );
    }
    else {
      result << "&" << latexify_row(matrix.row(i), strs) << R"(=0)";
    }

    if (i + 1 < matrix.rows()) {
      result << R"(\\)";
    }

    result << '\n';
  }
  result << R"(\end{align})" << '\n';

  return result.str();
}

} // namespace TopoMagnon
