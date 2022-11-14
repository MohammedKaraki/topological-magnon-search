#ifndef PARSE_UTILITY_HPP
#define PARSE_UTILITY_HPP

#include <string>
#include <regex>
#include <cassert>


namespace TopoMagnon {

class LittleIrrep {
public:
  LittleIrrep(std::string source_str) : src{std::move(source_str)}
  {
    const std::regex regex{R"((([A-Z]+).*)\(([0-9]+)\))"};
    std::smatch match;
    assert(std::regex_match(this->src, match, regex));

    irreplabel = match[1];
    ksymbol = match[2];
    dim = match[3];
  }

  const std::string& get_irreplabel() const { return irreplabel; }
  const std::string& get_ksymbol() const { return ksymbol; }
  const std::string& get_dim() const { return dim; }
  const std::string& get_src() const { return src; }

private:
  std::string irreplabel, ksymbol, dim;
  std::string src;
};

class Irrep1wp {
public:
  Irrep1wp(std::string source_str) : src{std::move(source_str)}
  {
    const std::regex regex{R"(\(([^,]+),([^,]+)\))"};
    std::smatch match;
    assert(std::regex_match(this->src, match, regex));

    irrep1 = match[1];
    wp = match[2];
  }

  const std::string& get_irrep1() const { return irrep1; }
  const std::string& get_wp() const { return wp; }
  const std::string& get_src() const { return src; }

private:
  std::string irrep1, wp;
  std::string src;
};


class Irrep12wp {
public:
  Irrep12wp(std::string source_str) : src{std::move(source_str)}
  {
    const std::regex regex{R"(\(([^,]+),([^,]+),([^,]+)\))"};
    std::smatch match;
    assert(std::regex_match(src, match, regex));

    irrep1 = match[1];
    irrep2 = match[2];
    wp = match[3];
  }

  const std::string& get_irrep1() const { return irrep1; }
  const std::string& get_irrep2() const { return irrep2; }
  const std::string& get_wp() const { return wp; }
  const std::string& get_src() const { return src; }

private:
  std::string irrep1, irrep2, wp;
  std::string src;
};


} // namespace TopoMagnon

#endif // PARSE_UTILITY_HPP
