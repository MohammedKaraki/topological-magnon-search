#ifndef OSTREAM_UTILITY_HPP
#define OSTREAM_UTILITY_HPP

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include "spectrum_data.hpp"

namespace TopoMagnon {


std::ostream& print_indent(std::ostream& out, int N)
{
  for (int i = 0; i < N; ++i) {
    out << ' ';
  }
  return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v)
{
  out << '[';
  if (v.empty()) {
    return out << ']';
  }


  out << v.front();

  for (auto it = std::next(v.begin());
       it != v.end();
       ++it)
  {
    out << ", ";
    out << *it;
  }

  return out << ']';
}


template<typename T>
std::ostream& print(std::ostream& out,
                    const T& t,
                    int total_indent,
                    int consumed_indent
                    )
{
  return print_indent(out, total_indent - consumed_indent) << t;
}

template<typename Key, typename Val>
std::ostream& print(std::ostream& out,
                    const std::map<Key, Val>& map,
                    const int total_indent,
                    int consumed_indent
                   )
{
  auto key_size = [](const auto& keyval_pair) {
    return keyval_pair.first.size();
  };

  const int delta_indent = 5 + key_size(
    *std::max_element(
      map.cbegin(),
      map.cend(),
      [key_size](const auto& lhs,
         const auto& rhs)
      {
        return key_size(lhs) < key_size(rhs);
      }
      )
    );

  for (const auto& [key, val] : map) {
    print_indent(out, total_indent - consumed_indent);
    out << key << ":";

    print(out,
          val,
          total_indent+delta_indent,
          total_indent + 1 + key.size()
          )
      << '\n';
    consumed_indent = 0;
  }

  return out;
}

template<typename Key, typename Val>
std::ostream& operator<<(std::ostream& out, const std::map<Key, Val>& map)
{
  return print(out, map, 0, 0);
}

std::ostream& operator<<(std::ostream& out, const SpectrumData& data)
{
  out << "super_msg_irreps:\n";
  out << data.super_msg.irreps << "\n\n";

  out << "sub_msg_irreps:\n";
  out << data.sub_msg.irreps << "\n\n";

  out << "comp_rels_matrix:\n";
  out << data.comp_rels_matrix << "\n\n";

  out << "si_orders:\n";
  out << data.si_orders << "\n\n";

  out << "si_matrix:\n";
  out << data.si_matrix << "\n\n";

  out << "band_super_irreps:\n";
  out << data.band_super_irreps << "\n\n";

  out << "band_sub_irreps:\n";
  out << data.band_sub_irreps << "\n\n";

  out << "sub_k1_to_k2_to_irrep_to_lineirreps:\n";
  out << data.sub_msg.k1_to_k2_to_irrep_to_lineirreps << '\n';

  out << "super_k1_to_k2_to_irrep_to_lineirreps:\n";
  out << data.super_msg.k1_to_k2_to_irrep_to_lineirreps << '\n';

  out << "subk_to_superirrep_to_subirreps:\n";
  out << data.subk_to_superirrep_to_subirreps << '\n';

  out << "superirrep_to_all_subirreps:\n";
  out << data.superirrep_to_all_subirreps;

  return out;
}

} // namespace

#endif // OSTREAM_UTILITY_HPP
