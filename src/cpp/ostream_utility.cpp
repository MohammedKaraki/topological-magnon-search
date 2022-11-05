#include <iostream>
#include "spectrum_data.hpp"
#include "ostream_utility.hpp"

namespace TopoMagnon {

std::ostream& operator<<(std::ostream& out, const SpectrumData& data)
{

  operator<<(out, std::vector<std::string>());
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

std::ostream& print_indent(std::ostream& out, int N)
{
  for (int i = 0; i < N; ++i) {
    out << ' ';
  }
  return out;
}

} // namespace TopoMagnon
