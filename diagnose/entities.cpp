// #include <vector>
// #include <set>
// #include <string>
// #include <utility>
// #include <tuple>
// #include <numeric>
// #include <cassert>
// #include "fmt/core.h"
// #include "fmt/color.h"
//
// #include "utility.hpp"
// #include "entities.hpp"
// // #include "spectrum_data.hpp"
//
//
// namespace magnon {
//
//
// Bag::Bag(const std::string& superirrep, const SpectrumData& data)
// {
//   const auto& subirreps = data.superirrep_to_all_subirreps.at(superirrep);
//
//   for (const auto& subirrep : subirreps) {
//
//     const auto subk = data.sub_msg.irrep_to_k.at(subirrep);
//     const auto subk_idx = data.sub_msg.k_to_idx(subk);
//     const auto subirrep_idx = data.sub_msg.irrep_to_idx(subirrep);
//
//     subk_idx_and_subirrep_idx_pairs.emplace_back(
//       std::pair{subk_idx, subirrep_idx}
//       );
//   }
//
//   std::sort(subk_idx_and_subirrep_idx_pairs.begin(),
//             subk_idx_and_subirrep_idx_pairs.end()
//            );
// }
//
// Supermode::Supermode(const std::string& superirrep,
//                      const SpectrumData& data)
// {
//   superirrep_idx = data.super_msg.irrep_to_idx(superirrep);
//
//   if (data.superirrep_to_all_subirreps.contains(superirrep)) {
//     bag_idx = data.bag_to_idx(Bag(superirrep, data));
//   } else {
//     // std::cerr << fmt::format(fmt::bg(fmt::color::red),
//     //                          "Warning: added this branch because of core "
//     //                          "dump on\n"
//     //                          "230.148 24c 11 due to superirrep P_1 "
//     //                          "doesn't match a subirrep in P-1\n");
//     // std::cerr << fmt::format(fmt::bg(fmt::color::red),
//     //                          "Current superirrep: {}\n",
//     //                          superirrep);
//     bag_idx = Bag::invalid_idx;
//     static_assert(Bag::invalid_idx == -1);
//   }
// }
//
// const Bag& Supermode::get_bag(const SpectrumData& data) const
// {
//   return data.unique_bags[bag_idx];
// }
//
// Superband::Superband(const std::vector<std::string>& superirreps,
//                      const SpectrumData& data)
//   : data{data}
// {
//   k_idx_to_e_idx_to_supermode.resize(data.super_msg.ks.size());
//
//   for (const auto& superirrep : superirreps) {
//     const auto k = data.super_msg.irrep_to_k.at(superirrep);
//     const auto k_idx = data.super_msg.k_to_idx(k);
//
//     k_idx_to_e_idx_to_supermode[k_idx].push_back(Supermode(superirrep, data));
//
//     std::sort(k_idx_to_e_idx_to_supermode[k_idx].begin(),
//               k_idx_to_e_idx_to_supermode[k_idx].end()
//              );
//   }
//
//   fix_antiunit_rels();
// }
//
// Subband Superband::make_subband() const
// {
//   Subband subband(data);
//   subband.subk_idx_to_e_idx_to_submode.resize(data.sub_msg.ks.size());
//
//   for (auto& e_idx_to_submode : subband.subk_idx_to_e_idx_to_submode) {
//     e_idx_to_submode.clear();
//   }
//
//   Vector<Vector<int>> subk_idx_to_span_idx_to_span_size(data.sub_msg.ks.size());
//   Vector<Vector<int>> subk_idx_to_span_idx_to_span_dim(data.sub_msg.ks.size());
//
//   for (const auto& e_idx_to_supermode : this->k_idx_to_e_idx_to_supermode) {
//     for (const auto& supermode : e_idx_to_supermode) {
//       const auto bag_idx = supermode.bag_idx;
//       if (bag_idx == Bag::invalid_idx) {
//         break;
//       }
//       const auto& bag = data.unique_bags[bag_idx];
//
//       for (auto& span_sizes : subk_idx_to_span_idx_to_span_size)
//       {
//         span_sizes.push_back(0);
//       }
//       for (auto& span_dims : subk_idx_to_span_idx_to_span_dim)
//       {
//         span_dims.push_back(0);
//       }
//
//       for (const auto& [subk_idx, subirrep_idx]
//            : bag.subk_idx_and_subirrep_idx_pairs)
//       {
//         subband.subk_idx_to_e_idx_to_submode[subk_idx].emplace_back(
//           subirrep_idx);
//
//         subk_idx_to_span_idx_to_span_size[subk_idx].back() += 1;
//         subk_idx_to_span_idx_to_span_dim[subk_idx].back() +=
//           data.sub_msg.dims[subirrep_idx];
//       }
//
//       for (int subk_idx = 0;
//            subk_idx < static_cast<int>(data.sub_msg.ks.size());
//            ++subk_idx)
//       {
//         auto& span_sizes = subk_idx_to_span_idx_to_span_size[subk_idx];
//         auto& span_dims = subk_idx_to_span_idx_to_span_dim[subk_idx];
//         if (span_sizes.back() == 0) {
//           assert(span_dims.back() == 0);
//
//           span_sizes.pop_back();
//           span_dims.pop_back();
//         }
//
//       }
//     }
//   }
//
//
//   std::map<int, Spans> gap_to_localspans;
//   std::map<int, Spans> gap_to_globalspans;
//
//   for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size());
//        ++subk_idx)
//   {
//     const auto& span_sizes = subk_idx_to_span_idx_to_span_size[subk_idx];
//     const auto& span_dims = subk_idx_to_span_idx_to_span_dim[subk_idx];
//
//     assert(span_sizes.size() == span_dims.size());
//
//     auto submode_begin = subband.subk_idx_to_e_idx_to_submode[subk_idx].begin();
//     int prev_gap_end = 0;
//     for (int span_idx = 0; span_idx < static_cast<int>(span_sizes.size());
//          ++span_idx)
//     {
//       const auto span_size = span_sizes[span_idx];
//       const auto span_dim = span_dims[span_idx];
//
//       const auto submode_end = std::next(submode_begin, span_size);
//       const int gap_end = prev_gap_end + span_dim;
//
//       const auto span = Span{submode_begin, submode_end};
//       const auto spanistrivial = (span_size <= 1) || Utility::all_equal(span);
//
//       if (spanistrivial == false) {
//         assert(span_dim >= 2);
//         assert(span_size >= 2);
//
//         if (span_dim == 2) {
//           const int gap = prev_gap_end + 1;
//           gap_to_localspans[gap].push_back(span);
//         } else {
//           for (int gap = prev_gap_end + 1; gap < gap_end; ++gap) {
//             gap_to_globalspans[gap].push_back(span);
//           }
//         }
//       }
//
//       submode_begin = submode_end;
//       prev_gap_end = gap_end;
//     }
//     // std::cout << "subk_idx: " << subk_idx << " last_gap_end: " << prev_gap_end << '\n';
//
//   }
//
//   auto remove_antidepend_spans = [this](auto& spans) {
//     spans.erase(std::remove_if(spans.begin(),
//                                spans.end(),
//                                [this](const auto& span) {
//                                  const auto k_idx = data.sub_msg.irrepidx_to_kidx[
//                                    span.front().subirrep_idx
//                                    ];
//                                  for (const auto& [_, k2idx, __]
//                                       : data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//                                  {
//                                    if (k_idx == k2idx) {
//                                      return true;
//                                    }
//                                  }
//                                  return false;
//                                }
//                               ),
//                 spans.end()
//                );
//   };
//   for (auto& [_, spans] : gap_to_localspans) {
//     remove_antidepend_spans(spans);
//   }
//   for (auto& [_, spans] : gap_to_globalspans) {
//     remove_antidepend_spans(spans);
//   }
//   // std::cout << "gap_to_localspans:\n";
//   // std::cout << gap_to_localspans << "\n\n";
//   // std::cout << "gap_to_globalspans:\n";
//   // std::cout << gap_to_globalspans << "\n\n";
//
//
//
//   Vector<std::set<int>> subk_idx_to_possible_gaps(data.sub_msg.ks.size());
//   for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size());
//        ++subk_idx)
//   {
//     const auto& span_sizes = subk_idx_to_span_idx_to_span_size[subk_idx];
//     const auto& span_dims = subk_idx_to_span_idx_to_span_dim[subk_idx];
//
//     auto submode_begin = subband.subk_idx_to_e_idx_to_submode[subk_idx].begin();
//     int prev_gap_end = 0;
//     for (int span_idx = 0; span_idx < static_cast<int>(span_sizes.size());
//          ++span_idx)
//     {
//       const auto span_size = span_sizes[span_idx];
//       const auto span_dim = span_dims[span_idx];
//
//       const auto submode_end = std::next(submode_begin, span_size);
//       const int gap_end = prev_gap_end + span_dim;
//
//       const auto span = Span{submode_begin, submode_end};
//       Vector<int> span_subdims;
//       for (const auto& submode : span) {
//         span_subdims.push_back(data.sub_msg.dims[submode.subirrep_idx]);
//       }
//       std::sort(span_subdims.begin(), span_subdims.end());
//       do {
//         int gap = 0;
//         for (const auto& delta_gap : span_subdims) {
//           gap += delta_gap;
//           subk_idx_to_possible_gaps[subk_idx].insert(prev_gap_end + gap);
//         }
//       } while (std::next_permutation(span_subdims.begin(), span_subdims.end()));
//
//       submode_begin = submode_end;
//       prev_gap_end = gap_end;
//     }
//   }
//
//
//   // std::cout << "subk_idx_to_possible_gaps:\n";
//   for (int i = 0; i < static_cast<int>(data.sub_msg.ks.size()); ++i) {
//     // std::cout << i << ":\t" << subk_idx_to_possible_gaps[i] << "\n";
//   }
//
//   Vector<int> possible_gaps = Utility::intersect(subk_idx_to_possible_gaps);
//   assert(!possible_gaps.empty());
//
//   subband.num_bands = std::accumulate(
//     subband.subk_idx_to_e_idx_to_submode[0].begin(),
//     subband.subk_idx_to_e_idx_to_submode[0].end(),
//     0,
//     [this](const int lhs, const auto& submode) {
//       return lhs + data.sub_msg.dims[submode.subirrep_idx];
//     }
//     );
//   assert(subband.num_bands == possible_gaps.back());
//
//
//   // std::cout << "\n";
//   // std::cout << "possible_gaps:\n"
//     // << possible_gaps << '\n';
//
//
//   Vector<std::tuple<Vector<int>, Vector<Span>>> gaps_sharedspans_pairs;
//
//   {
//   Vector<int> gaps;
//   Vector<Span> sharedspans;
//   for (int gap = 1; gap <= subband.num_bands; ++gap) {
//     if (!gap_to_globalspans.contains(gap)) {
//       if (!gaps.empty()) {
//         gaps_sharedspans_pairs.emplace_back(gaps, sharedspans);
//       }
//       gaps.clear();
//       sharedspans.clear();
//
//       if (std::find(possible_gaps.begin(), possible_gaps.end(), gap)
//           != possible_gaps.end())
//       {
//         gaps_sharedspans_pairs.emplace_back(Vector<int>{gap},
//                                             Spans{});
//       }
//
//     } else {
//
//       if (std::find(possible_gaps.begin(), possible_gaps.end(), gap)
//           != possible_gaps.end())
//       {
//         gaps.push_back(gap);
//       }
//
//       if (gap_to_globalspans.contains(gap)) {
//         for (const auto& new_sharedspan : gap_to_globalspans.at(gap)) {
//           if (std::find_if(sharedspans.begin(),
//                            sharedspans.end(),
//                            [&new_sharedspan](const auto& span) {
//                              return span.begin() == new_sharedspan.begin();
//                            }
//                           )
//               == sharedspans.end())
//           {
//             sharedspans.push_back(new_sharedspan);
//           }
//         }
//       }
//     }
//   }
//   }
//
//
//   for (const auto& [gaps, sharedspans]
//        : gaps_sharedspans_pairs)
//   {
//     Vector<Span> allspanstopermute{sharedspans};
//     for (const auto& gap : gaps) {
//       if (gap_to_localspans.contains(gap)) {
//         const auto& localspans = gap_to_localspans.at(gap);
//         allspanstopermute.insert(allspanstopermute.end(),
//                                  localspans.begin(),
//                                  localspans.end()
//                                 );
//       }
//     }
//     subband.gaps_allspanstopermute_done_tuples.emplace_back(
//       gaps,
//       allspanstopermute,
//       false);
//     assert(Utility::is_cartesian_sorted(allspanstopermute));
//
//   }
//
//   Vector<int> test1;
//   for (const auto& [gaps, _, __]
//        : subband.gaps_allspanstopermute_done_tuples)
//   {
//     test1.insert(test1.end(), gaps.begin(), gaps.end());
//   }
//   assert(test1 == possible_gaps);
//   subband.all_possible_gaps = possible_gaps;
//
//   subband.fix_antiunit_rels();
//
//   return subband;
// }
//
// bool Subband::next_energetics()
// {
//   for (auto& [gaps, allspanstopermute, done]
//        : gaps_allspanstopermute_done_tuples)
//   {
//     if (!done) {
//       if (Utility::cartesian_permute(allspanstopermute)) {
//         fix_antiunit_rels();
//         return true;
//       } else {
//         done = true;
//         fix_antiunit_rels();
//         return true;
//       }
//     }
//   }
//
//   fix_antiunit_rels();
//   return false;
// }
//
// Vector32<short> Subband::make_br(
//   const Vector8<int>& e_idxs_beg,
//   const Vector8<int>& e_idxs_end,
//   const SpectrumData& data
//   )
// {
//   Vector32<short> result(data.sub_msg.irreps.size(), 0);
//
//   for (unsigned i = 0; i < e_idxs_beg.size(); ++i) {
//     for (int j = e_idxs_beg[i]; j < e_idxs_end[i]; ++j) {
//       ++result[subk_idx_to_e_idx_to_submode[i][j].subirrep_idx];
//     }
//   }
//
//   return result;
// }
//
//
// Vector4<Vector8<int>>
// Subband::dimvalid_e_idxs()
// {
//   Vector4<Vector8<int>> result;
//
//   const auto num_ks = subk_idx_to_e_idx_to_submode.size();
//
//   Vector8<int> k_idx_to_accum(num_ks, 0);
//   Vector8<int> k_idx_to_cur_e_idx(num_ks, 0);
//
//   result.push_back(k_idx_to_cur_e_idx);
//
//   auto min_accum_it = k_idx_to_accum.begin();
//   auto max_accum_it = min_accum_it;
//   int min_accum_k_idx = 0;
//
//   while (k_idx_to_cur_e_idx[min_accum_k_idx] < static_cast<int>(
//       subk_idx_to_e_idx_to_submode[min_accum_k_idx].size())
//         )
//   {
//     *min_accum_it += data.sub_msg.dims[
//         subk_idx_to_e_idx_to_submode[
//           min_accum_k_idx][
//             k_idx_to_cur_e_idx[min_accum_k_idx]++
//           ].subirrep_idx
//     ];
//
//     std::tie(min_accum_it, max_accum_it) = std::minmax_element(
//       k_idx_to_accum.begin(),
//       k_idx_to_accum.end()
//       );
//     min_accum_k_idx = std::distance(k_idx_to_accum.begin(), min_accum_it);
//
//     if (*min_accum_it == *max_accum_it) {
//       result.push_back(k_idx_to_cur_e_idx);
//     }
//   }
//
//   return result;
// }
//
//
// std::ostream& operator<<(std::ostream& out, const Bag& b)
// {
//   return out << b.subk_idx_and_subirrep_idx_pairs;
// }
//
//
// std::ostream& operator<<(std::ostream& out, const Supermode& supermode)
// {
//   return out << fmt::format("({}, {})",
//                             supermode.superirrep_idx,
//                             supermode.bag_idx
//                            );
// }
//
// std::ostream& operator<<(std::ostream& out, const Submode& submode)
// {
//   return out << fmt::format("(sub: {})", submode.subirrep_idx);
// }
//
// std::ostream& operator<<(std::ostream& out, const Superband& b)
// {
//   return out << b.k_idx_to_e_idx_to_supermode;
// }
//
// bool Superband::cartesian_permute()
// {
//   std::set<int> kidxs_to_skip;
//   for (const auto& [_, kidx2, __]
//        : data.super_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//   {
//     kidxs_to_skip.insert(kidx2);
//   }
//
//   for (int kidx = 0;
//        kidx < static_cast<int>(k_idx_to_e_idx_to_supermode.size());
//        ++kidx)
//   {
//     if (kidxs_to_skip.contains(kidx)) {
//       continue;
//     }
//
//     auto& supermodes = k_idx_to_e_idx_to_supermode[kidx];
//     if (std::next_permutation(supermodes.begin(), supermodes.end())) {
//       fix_antiunit_rels();
//       return true;
//     }
//   }
//
//   fix_antiunit_rels();
//   return false;
// }
//
//
// std::map<int, std::pair<bool, IntMatrix>>
// Subband::calc_gap_sis() const
// {
//   std::map<int, std::pair<bool, IntMatrix>> result;
//
//   assert (num_bands >= 1);
//
//   IntMatrix si = 0 * data.si_matrix.col(0);
//   IntMatrix cr = 0 * data.comp_rels_matrix.col(0);
//
//   Vector<int> subk_idx_to_numbandsbelow(data.sub_msg.ks.size(), 0);
//   Vector<typename Vector<Submode>::const_iterator> subk_idx_to_cur_submode_it;
//   for (const auto& e_idx_to_submode : subk_idx_to_e_idx_to_submode) {
//     subk_idx_to_cur_submode_it.push_back(e_idx_to_submode.begin());
//   }
//
//   for (int gap = 1; gap <= num_bands; ++gap) {
//     for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size());
//          ++subk_idx)
//     {
//       auto& numbandsbelow = subk_idx_to_numbandsbelow[subk_idx];
//       while (numbandsbelow < gap) {
//         auto& submode_it = subk_idx_to_cur_submode_it[subk_idx];
//         const int cur_subirrep_idx = submode_it->subirrep_idx;
//
//         numbandsbelow += data.sub_msg.dims[cur_subirrep_idx];
//
//         si += data.si_matrix.col(cur_subirrep_idx);
//         cr += data.comp_rels_matrix.col(cur_subirrep_idx);
//
//         ++submode_it;
//       }
//     }
//
//
//     bool gapped = cr.isZero();
//
//     if (gapped) {
//       const auto numbandsbelow = subk_idx_to_numbandsbelow[0];
//       for (int subk_idx = 0;
//            subk_idx < static_cast<int>(data.sub_msg.ks.size());
//            ++subk_idx)
//       {
//         assert(subk_idx_to_numbandsbelow[subk_idx] == numbandsbelow);
//       }
//       if (numbandsbelow != gap) {
//         gapped = false;
//       }
//     }
//
//     result[gap].first = gapped;
//     if (gapped) {
//       assert(si.size() == static_cast<int>(data.si_orders.size()));
//       for (int i = 0; i < si.size(); ++i) {
//         si(i) %= data.si_orders[i];
//       }
//       result[gap].second = si;
//     }
//   }
//
//   return result;
// }
//
//
// bool Superband::satisfies_antiunit_rels() const
// {
//   for (const auto& [k1_idx, k2_idx, irrep1idx_to_irrep2idx]
//        : data.super_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//   {
//     const auto& first_e_idx_to_supermode = k_idx_to_e_idx_to_supermode[k1_idx];
//     const auto& second_e_idx_to_supermode = k_idx_to_e_idx_to_supermode[k2_idx];
//
//     assert(first_e_idx_to_supermode.size() == second_e_idx_to_supermode.size());
//     for (int e_idx = 0;
//          e_idx < static_cast<int>(first_e_idx_to_supermode.size());
//          ++e_idx)
//     {
//       const auto irrep1_idx = first_e_idx_to_supermode[e_idx].superirrep_idx;
//       const auto irrep2_idx = second_e_idx_to_supermode[e_idx].superirrep_idx;
//       if (irrep1idx_to_irrep2idx.at(irrep1_idx) != irrep2_idx) {
//         return false;
//       }
//     }
//   }
//
//   return true;
// }
//
// bool Subband::satisfies_antiunit_rels() const
// {
//   //
//   // std::set<int> irrepidxs_1, irrepidxs_2;
//   //
//   // for (const auto& [k1_idx, k2_idx, irrep1idx_to_irrep2idx]
//   //      : data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//   // {
//   //   const auto& first_e_idx_to_submode = subk_idx_to_e_idx_to_submode[k1_idx];
//   //   const auto& second_e_idx_to_submode = subk_idx_to_e_idx_to_submode[k2_idx];
//   //
//   //
//   //   int dim1 = 0;
//   //   irrepidxs_1.clear();
//   //
//   //   int dim2 = 0;
//   //   irrepidxs_2.clear();
//   //
//   //
//   //   assert(first_e_idx_to_submode.size() == second_e_idx_to_submode.size());
//   //   for (int e_idx = 0;
//   //        e_idx < static_cast<int>(first_e_idx_to_submode.size());
//   //        ++e_idx)
//   //   {
//   //     const auto irrep1_idx = first_e_idx_to_submode[e_idx].subirrep_idx;
//   //     const auto irrep2_idx = second_e_idx_to_submode[e_idx].subirrep_idx;
//   //
//   //     dim1 += data.sub_msg.dims.at(irrep1_idx);
//   //     dim2 += data.sub_msg.dims.at(irrep2_idx);
//   //
//   //     irrepidxs_1.insert(irrep1idx_to_irrep2idx.at(irrep1_idx));
//   //     irrepidxs_2.insert(irrep2_idx);
//   //
//   //     if (
//   //       (std::find(all_possible_gaps.begin(), all_possible_gaps.end(), dim1)
//   //        != all_possible_gaps.end())
//   //       || (std::find(all_possible_gaps.begin(), all_possible_gaps.end(), dim2)
//   //           != all_possible_gaps.end())
//   //       )
//   //     {
//   //       if (irrepidxs_1 != irrepidxs_2) {
//   //         return false;
//   //       }
//   //     }
//   //   }
//   // }
//   //
//   // return true;
//   for (const auto& [k1_idx, k2_idx, irrep1idx_to_irrep2idx]
//        : data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//   {
//     const auto& first_e_idx_to_submode = subk_idx_to_e_idx_to_submode[k1_idx];
//     const auto& second_e_idx_to_submode = subk_idx_to_e_idx_to_submode[k2_idx];
//
//     assert(first_e_idx_to_submode.size() == second_e_idx_to_submode.size());
//     for (int e_idx = 0;
//          e_idx < static_cast<int>(first_e_idx_to_submode.size());
//          ++e_idx)
//     {
//       const auto irrep1_idx = first_e_idx_to_submode[e_idx].subirrep_idx;
//       const auto irrep2_idx = second_e_idx_to_submode[e_idx].subirrep_idx;
//       if (irrep1idx_to_irrep2idx.at(irrep1_idx) != irrep2_idx) {
//         return false;
//       }
//     }
//   }
//
//   return true;
// }
//
//
// void Subband::fix_antiunit_rels()
// {
//   for (const auto& [k1idx, k2idx, irrepidx1_to_irrepidx2]
//        : data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//   {
//     const auto& submodes1 = subk_idx_to_e_idx_to_submode[k1idx];
//     auto& submodes2 = subk_idx_to_e_idx_to_submode[k2idx];
//     for (int i = 0; i < static_cast<int>(submodes1.size()); ++i) {
//       submodes2[i] = Submode(
//         irrepidx1_to_irrepidx2.at(submodes1[i].subirrep_idx)
//         );
//     }
//   }
// }
//
// void Superband::fix_antiunit_rels()
// {
//   for (const auto& [k1idx, k2idx, irrepidx1_to_irrepidx2]
//        : data.super_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples)
//   {
//     const auto& supermodes1 = k_idx_to_e_idx_to_supermode[k1idx];
//     auto& supermodes2 = k_idx_to_e_idx_to_supermode[k2idx];
//     for (int i = 0; i < static_cast<int>(supermodes1.size()); ++i) {
//       supermodes2[i] = Supermode(
//         data.super_msg.irreps[
//         irrepidx1_to_irrepidx2.at(supermodes1[i].superirrep_idx)
//         ],
//         data
//         );
//     }
//   }
// }
//
// } // namespace magnon
