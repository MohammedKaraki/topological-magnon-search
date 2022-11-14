#ifndef IRREPS_HPP
#define IRREPS_HPP

#include <string>
#include <set>
#include <vector>

namespace TopoMagnon {

class Littleirrep {
public:
  Littleirrep(std::string source_str);

  std::string latexify() const;

  const std::string& get_irreplabel() const { return irreplabel; }
  const std::string& get_ksymbol() const { return ksymbol; }
  const std::string& get_dim() const { return dim; }
  const std::string& to_str() const { return src; }

  auto operator<=>(const Littleirrep& rhs) const = default;

private:
  std::string irreplabel, ksymbol, dim;
  std::string src;
};

class Irrep1wp {
public:
  Irrep1wp(std::string src);

  Irrep1wp(std::string irrep1, std::string wp)
    : irrep1{std::move(irrep1)}, wp{std::move(wp)}
  {

  }

  const std::string& get_irrep1() const { return irrep1; }
  const std::string& get_wp() const { return wp; }
  const std::string to_str() const;

  std::string latexify() const;

  auto operator<=>(const Irrep1wp& rhs) const = default;

private:
  std::string irrep1, wp;
};


class Irrep12wp {
public:
  Irrep12wp(std::string source_str);

  Irrep1wp get_irrep1wp() const { return Irrep1wp(irrep1, wp); }
  Irrep1wp get_irrep2wp() const { return Irrep1wp(irrep2, wp); }
  const std::string& get_wp() const { return wp; }
  const std::string& get_src() const { return src; }

  std::string latexify() const;

  auto operator<=>(const Irrep12wp& rhs) const = default;

private:
  std::string irrep1, irrep2, wp;
  std::string src;
};


class Irrep1wpDecomp {
public:
  Irrep1wpDecomp(std::multiset<Irrep1wp> comps) : comps{std::move(comps)}
  {
  }

  std::string latexify() const;

  const auto& get_comps() const
  {
    return comps;
  }

  auto operator<=>(const Irrep1wpDecomp& ) const = default;

private:
  std::multiset<Irrep1wp> comps;
};


class Irrep12wpDecomp {
public:
  Irrep12wpDecomp(const std::vector<std::string>& srcs);

  std::string latexify() const;
  std::set<Irrep1wpDecomp> find_all_magnon_irrep1wp_decomps() const;
  auto operator<=>(const Irrep12wpDecomp& ) const = default;

private:
  std::multiset<Irrep12wp> irrep12wp_comps;
};


class LittleirrepDecomp {
public:
  LittleirrepDecomp(const std::vector<std::string>& srcs);
  std::string latexify() const;
  std::vector<std::string> get_irreps_as_strs() const;
  auto operator<=>(const LittleirrepDecomp& rhs) const = default;

private:
  std::multiset<Littleirrep> littleirrep_comps;
};

} // namespace TopoMagnon

#endif // IRREPS_HPP
