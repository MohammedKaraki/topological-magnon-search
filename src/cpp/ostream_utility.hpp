#ifndef OSTREAM_UTILITY_HPP
#define OSTREAM_UTILITY_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>

namespace TopoMagnon {

struct SpectrumData;

std::ostream& print_indent(std::ostream& out, int N);

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

std::ostream& operator<<(std::ostream& out, const SpectrumData& data);

} // namespace TopoMagnon

#endif // OSTREAM_UTILITY_HPP
