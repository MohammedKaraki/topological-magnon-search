#include "spectrum_data.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "utility.hpp"

namespace magnon::diagnose2 {

int SpectrumData::Msg::irrep_to_idx(const std::string irrep) const {
    return find_unique_index(irreps, irrep);
}

int SpectrumData::Msg::k_to_idx(const std::string k) const { return find_unique_index(ks, k); }

std::string SpectrumData::site_irreps_as_str() const {
    std::ostringstream result;

    const auto &[site_irrep1, site_irrep2] = magnon_site_irreps;

    if (site_irrep1 == site_irrep2) {
        result << "2" << site_irrep1;
    } else {
        result << site_irrep1 + R"(\oplus )" + site_irrep2;
    }

    return result.str();
}
std::string SpectrumData::firstsiteirrep_and_wp_as_strkey() const {
    std::ostringstream result;

    const auto &[site_irrep1, _] = magnon_site_irreps;

    result << '(' << site_irrep1 << ',' << wp << ')';
    return result.str();
}

std::string SpectrumData::make_br_label() const {
    std::ostringstream result;

    const auto &[site_irrep1, site_irrep2] = magnon_site_irreps;

    if (site_irrep1 == site_irrep2) {
        result << "2{(" + site_irrep1 + ")}_{" + wp + "}";
    } else {
        result << "{(" + site_irrep1 + "+" + site_irrep2 + ")}_{" + wp + "}";
    }

    return result.str();
}

int SpectrumData::subk_idx_to_superk_idx(int subk_idx) const {
    const auto subk = sub_msg.ks[subk_idx];
    const auto first_superirrep = subk_to_superirrep_to_subirreps.at(subk).begin()->first;
    const auto superk = super_msg.irrep_to_k.at(first_superirrep);
    return super_msg.k_to_idx(superk);
}

int SpectrumData::bag_to_idx(const Bag &bag) const { return find_unique_index(unique_bags, bag); }

void SpectrumData::Msg::populate_irrep_dims() {
    std::transform(
        irreps.begin(), irreps.end(), std::back_inserter(dims), [this](const auto &irrep) {
            return irrep_to_dim.at(irrep);
        });

    assert(dims.size() == irreps.size());
    assert(std::all_of(dims.begin(), dims.end(), [](const auto dim) { return dim >= 1; }));
    for (int i = 0; i < static_cast<int>(irreps.size()); ++i) {
        assert(irrep_to_dim.at(irreps[i]) == dims[i]);
    }
}

MatrixInt SpectrumData::Msg::make_br_vec(const std::vector<std::string> &br_irreps) {
    MatrixInt result(irreps.size(), 1);

    unsigned assert_count = 0;

    for (std::size_t i = 0; i < irreps.size(); ++i) {
        result(i) = std::count(br_irreps.begin(), br_irreps.end(), irreps[i]);
        assert_count += result(i);
    }

    assert(assert_count == br_irreps.size());

    return result;
}

int SpectrumData::Msg::sum_dim(const std::vector<std::string> &irreps) {
    return std::transform_reduce(
        irreps.cbegin(), irreps.cend(), 0, std::plus<>(), [this](const auto &irrep) {
            return irrep_to_dim.at(irrep);
        });
}

MatrixInt construct_matrix(const std::vector<std::vector<int>> &rows) {
    int num_rows = rows.size();
    int num_cols = rows[0].size();

    for (const auto &row : rows) {
        assert(static_cast<int>(row.size()) == num_cols);
    }

    MatrixInt result(num_rows, num_cols);

    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            result(i, j) = rows[i][j];
        }
    }

    return result;
}

SpectrumData::SpectrumData(const PerturbedBandStructure &spectrum) {
    assert(spectrum.atomic_orbital_size() == 1);
    {
        assert(spectrum.atomic_orbital(0).wyckoff_position().has_label());
        wp = spectrum.atomic_orbital(0).wyckoff_position().label();
        assert(spectrum.atomic_orbital(0).site_symmetry_irrep().has_label());
        magnon_site_irreps = {spectrum.atomic_orbital(0).site_symmetry_irrep().label(), ""};
    }
}

// void from_json(const json &j, SpectrumData &data) {
//     j["wp"].get_to(data.wp);
//     j["magnon_site_irreps"].get_to(data.magnon_site_irreps);
//
//     j["posbrsiteirrep"].get_to(data.pos_neg_siteirreps.first);
//     j["posbrirreps"].get_to(data.pos_neg_magnonirreps.first);
//     // j["negbrsiteirrep"].get_to(data.pos_neg_siteirreps.second);
//     // j["negbrirreps"].get_to(data.pos_neg_magnonirreps.second);
//     j["super_msg_label"].get_to(data.super_msg.label);
//     j["super_msg_number"].get_to(data.super_msg.number);
//     j["super_msg_irreps"].get_to(data.super_msg.irreps);
//     j["superirrep_to_dim"].get_to(data.super_msg.irrep_to_dim);
//     j["superirrep_to_k"].get_to(data.super_msg.irrep_to_k);
//     j["super_msg_kcoords"].get_to(data.super_msg.kcoords);
//     data.super_msg.populate_irrep_dims();
//
//     j["sub_msg_label"].get_to(data.sub_msg.label);
//     j["sub_msg_number"].get_to(data.sub_msg.number);
//     j["sub_msg_irreps"].get_to(data.sub_msg.irreps);
//     j["subirrep_to_dim"].get_to(data.sub_msg.irrep_to_dim);
//     j["subirrep_to_k"].get_to(data.sub_msg.irrep_to_k);
//     j["sub_msg_kcoords"].get_to(data.sub_msg.kcoords);
//     data.sub_msg.populate_irrep_dims();
//
//     {
//         std::string presc_str;
//         j["presc"].get_to(presc_str);
//         presc_str = std::regex_replace(presc_str, std::regex(R"(generic)"), R"(low-symmetry)");
//
//         size_t last = 0;
//         size_t next;
//         while ((next = presc_str.find(";", last)) != std::string::npos) {
//             data.presc.push_back(presc_str.substr(last, next - last));
//             last = next + 1;
//         }
//         data.presc.push_back(presc_str.substr(last));
//     }
//     j["super_msg_ks"].get_to(data.super_msg.ks);
//     j["sub_msg_ks"].get_to(data.sub_msg.ks);
//
//     j["subk_to_g_and_superk"].get_to(data.subk_to_g_and_superk);
//
//     {
//         std::vector<std::vector<int>> tmp;
//         j["comp_rels_matrix"].get_to(tmp);
//         data.comp_rels_matrix = construct_matrix(tmp);
//     }
//
//     j["si_orders"].get_to(data.si_orders);
//
//     if (!data.si_orders.empty()) {
//         std::vector<std::vector<int>> tmp;
//         j["si_matrix"].get_to(tmp);
//         data.si_matrix = construct_matrix(tmp);
//     }
//     assert(static_cast<int>(data.si_orders.size()) == data.si_matrix.rows());
//
//     j["superirrep_to_all_subirreps"].get_to(data.superirrep_to_all_subirreps);
//     j["subk_to_superirrep_to_subirreps"].get_to(data.subk_to_superirrep_to_subirreps);
//
//     j["super_k1_to_k2_to_irrep_to_lineirreps"].get_to(
//         data.super_msg.k1_to_k2_to_irrep_to_lineirreps);
//     j["sub_k1_to_k2_to_irrep_to_lineirreps"].get_to(data.sub_msg.k1_to_k2_to_irrep_to_lineirreps);
//
//     auto fill_helper_1 = [&j](const std::string &src_key, auto &target_msg) {
//         std::vector<
//             std::tuple<std::string, std::string, std::vector<std::pair<std::string,
//             std::string>>>> k1_k2_irrep1irrep2pairs_tuples;
//         j[src_key].get_to(k1_k2_irrep1irrep2pairs_tuples);
//         for (const auto &[k1, k2, irrep1irrep2pairs] : k1_k2_irrep1irrep2pairs_tuples) {
//             const auto k1_idx = target_msg.k_to_idx(k1);
//             const auto k2_idx = target_msg.k_to_idx(k2);
//
//             std::map<int, int> irrep1idx_to_irrep2idx;
//             for (const auto &[irrep1, irrep2] : irrep1irrep2pairs) {
//                 const auto &irrep1_idx = target_msg.irrep_to_idx(irrep1);
//                 const auto &irrep2_idx = target_msg.irrep_to_idx(irrep2);
//
//                 // irrep1_idx should be used only once as a key
//                 assert(!irrep1idx_to_irrep2idx.contains(irrep1_idx));
//
//                 irrep1idx_to_irrep2idx[irrep1_idx] = irrep2_idx;
//
//                 // irrep2_idx should never appear on the left hand side of the map
//                 assert(!irrep1idx_to_irrep2idx.contains(irrep2_idx));
//             }
//
//             target_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples.emplace_back(
//                 k1_idx, k2_idx, irrep1idx_to_irrep2idx);
//         }
//     };
//     fill_helper_1("k1_k2_irrep1irrep2pairs_tuples_of_supermsg", data.super_msg);
//     fill_helper_1("k1_k2_irrep1irrep2pairs_tuples_of_submsg", data.sub_msg);
//
//     j["super_to_sub"].get_to(data.super_to_sub);
//
//     for (const auto &[subk, superirrep_to_subirreps] : data.subk_to_superirrep_to_subirreps) {
//         for (const auto &[superirrep, subirreps] : superirrep_to_subirreps) {
//             assert(data.super_msg.irrep_to_dim.at(superirrep) ==
//             data.sub_msg.sum_dim(subirreps));
//         }
//     }
//
//     for (const auto &[superirrep, subirreps] : data.superirrep_to_all_subirreps) {
//         data.unique_bags.emplace_back(superirrep, data);
//     }
//     std::sort(data.unique_bags.begin(), data.unique_bags.end());
//     data.unique_bags.erase(std::unique(data.unique_bags.begin(), data.unique_bags.end()),
//                            data.unique_bags.end());
//
//     auto fill_helper_2 = [](auto &msg) {
//         for (int i = 0; i < static_cast<int>(msg.irreps.size()); ++i) {
//             const auto &k = msg.irrep_to_k.at(msg.irreps[i]);
//             const auto k_idx = msg.k_to_idx(k);
//             msg.irrepidx_to_kidx.push_back(k_idx);
//         }
//     };
//     fill_helper_2(data.sub_msg);
//     fill_helper_2(data.super_msg);
// }

Bag::Bag(const std::string &superirrep, const SpectrumData &data) {
    const auto &subirreps = data.superirrep_to_all_subirreps.at(superirrep);

    for (const auto &subirrep : subirreps) {

        const auto subk = data.sub_msg.irrep_to_k.at(subirrep);
        const auto subk_idx = data.sub_msg.k_to_idx(subk);
        const auto subirrep_idx = data.sub_msg.irrep_to_idx(subirrep);

        subk_idx_and_subirrep_idx_pairs.emplace_back(std::pair{subk_idx, subirrep_idx});
    }

    std::sort(subk_idx_and_subirrep_idx_pairs.begin(), subk_idx_and_subirrep_idx_pairs.end());
}

Supermode::Supermode(const std::string &superirrep, const SpectrumData &data) {
    superirrep_idx = data.super_msg.irrep_to_idx(superirrep);

    if (data.superirrep_to_all_subirreps.contains(superirrep)) {
        bag_idx = data.bag_to_idx(Bag(superirrep, data));
    } else {
        // std::cerr << fmt::format(fmt::bg(fmt::color::red),
        //                          "Warning: added this branch because of core "
        //                          "dump on\n"
        //                          "230.148 24c 11 due to superirrep P_1 "
        //                          "doesn't match a subirrep in P-1\n");
        // std::cerr << fmt::format(fmt::bg(fmt::color::red),
        //                          "Current superirrep: {}\n",
        //                          superirrep);
        bag_idx = Bag::invalid_idx;
        static_assert(Bag::invalid_idx == -1);
    }
}

const Bag &Supermode::get_bag(const SpectrumData &data) const { return data.unique_bags[bag_idx]; }

Superband::Superband(const std::vector<std::string> &superirreps, const SpectrumData &data)
    : data{data} {
    k_idx_to_e_idx_to_supermode.resize(data.super_msg.ks.size());

    for (const auto &superirrep : superirreps) {
        const auto k = data.super_msg.irrep_to_k.at(superirrep);
        const auto k_idx = data.super_msg.k_to_idx(k);

        k_idx_to_e_idx_to_supermode[k_idx].push_back(Supermode(superirrep, data));

        std::sort(k_idx_to_e_idx_to_supermode[k_idx].begin(),
                  k_idx_to_e_idx_to_supermode[k_idx].end());
    }

    fix_antiunit_rels();
}

Subband Superband::make_subband() const {
    Subband subband(data);
    subband.subk_idx_to_e_idx_to_submode.resize(data.sub_msg.ks.size());

    for (auto &e_idx_to_submode : subband.subk_idx_to_e_idx_to_submode) {
        e_idx_to_submode.clear();
    }

    Vector<Vector<int>> subk_idx_to_span_idx_to_span_size(data.sub_msg.ks.size());
    Vector<Vector<int>> subk_idx_to_span_idx_to_span_dim(data.sub_msg.ks.size());

    for (const auto &e_idx_to_supermode : this->k_idx_to_e_idx_to_supermode) {
        for (const auto &supermode : e_idx_to_supermode) {
            const auto bag_idx = supermode.bag_idx;
            if (bag_idx == Bag::invalid_idx) {
                break;
            }
            const auto &bag = data.unique_bags[bag_idx];

            for (auto &span_sizes : subk_idx_to_span_idx_to_span_size) {
                span_sizes.push_back(0);
            }
            for (auto &span_dims : subk_idx_to_span_idx_to_span_dim) {
                span_dims.push_back(0);
            }

            for (const auto &[subk_idx, subirrep_idx] : bag.subk_idx_and_subirrep_idx_pairs) {
                subband.subk_idx_to_e_idx_to_submode[subk_idx].emplace_back(subirrep_idx);

                subk_idx_to_span_idx_to_span_size[subk_idx].back() += 1;
                subk_idx_to_span_idx_to_span_dim[subk_idx].back() +=
                    data.sub_msg.dims[subirrep_idx];
            }

            for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size());
                 ++subk_idx) {
                auto &span_sizes = subk_idx_to_span_idx_to_span_size[subk_idx];
                auto &span_dims = subk_idx_to_span_idx_to_span_dim[subk_idx];
                if (span_sizes.back() == 0) {
                    assert(span_dims.back() == 0);

                    span_sizes.pop_back();
                    span_dims.pop_back();
                }
            }
        }
    }

    std::map<int, Spans> gap_to_localspans;
    std::map<int, Spans> gap_to_globalspans;

    for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size()); ++subk_idx) {
        const auto &span_sizes = subk_idx_to_span_idx_to_span_size[subk_idx];
        const auto &span_dims = subk_idx_to_span_idx_to_span_dim[subk_idx];

        assert(span_sizes.size() == span_dims.size());

        auto submode_begin = subband.subk_idx_to_e_idx_to_submode[subk_idx].begin();
        int prev_gap_end = 0;
        for (int span_idx = 0; span_idx < static_cast<int>(span_sizes.size()); ++span_idx) {
            const auto span_size = span_sizes[span_idx];
            const auto span_dim = span_dims[span_idx];

            const auto submode_end = std::next(submode_begin, span_size);
            const int gap_end = prev_gap_end + span_dim;

            const auto span = Span{submode_begin, submode_end};
            const auto spanistrivial = (span_size <= 1) || all_equal(span);

            if (spanistrivial == false) {
                assert(span_dim >= 2);
                assert(span_size >= 2);

                if (span_dim == 2) {
                    const int gap = prev_gap_end + 1;
                    gap_to_localspans[gap].push_back(span);
                } else {
                    for (int gap = prev_gap_end + 1; gap < gap_end; ++gap) {
                        gap_to_globalspans[gap].push_back(span);
                    }
                }
            }

            submode_begin = submode_end;
            prev_gap_end = gap_end;
        }
        // std::cout << "subk_idx: " << subk_idx << " last_gap_end: " << prev_gap_end << '\n';
    }

    auto remove_antidepend_spans = [this](auto &spans) {
        spans.erase(std::remove_if(spans.begin(),
                                   spans.end(),
                                   [this](const auto &span) {
                                       const auto k_idx =
                                           data.sub_msg.irrepidx_to_kidx[span.front().subirrep_idx];
                                       for (const auto &[_, k2idx, __] :
                                            data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples) {
                                           if (k_idx == k2idx) {
                                               return true;
                                           }
                                       }
                                       return false;
                                   }),
                    spans.end());
    };
    for (auto &[_, spans] : gap_to_localspans) {
        remove_antidepend_spans(spans);
    }
    for (auto &[_, spans] : gap_to_globalspans) {
        remove_antidepend_spans(spans);
    }

    Vector<std::set<int>> subk_idx_to_possible_gaps(data.sub_msg.ks.size());
    for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size()); ++subk_idx) {
        const auto &span_sizes = subk_idx_to_span_idx_to_span_size[subk_idx];
        const auto &span_dims = subk_idx_to_span_idx_to_span_dim[subk_idx];

        auto submode_begin = subband.subk_idx_to_e_idx_to_submode[subk_idx].begin();
        int prev_gap_end = 0;
        for (int span_idx = 0; span_idx < static_cast<int>(span_sizes.size()); ++span_idx) {
            const auto span_size = span_sizes[span_idx];
            const auto span_dim = span_dims[span_idx];

            const auto submode_end = std::next(submode_begin, span_size);
            const int gap_end = prev_gap_end + span_dim;

            const auto span = Span{submode_begin, submode_end};
            Vector<int> span_subdims;
            for (const auto &submode : span) {
                span_subdims.push_back(data.sub_msg.dims[submode.subirrep_idx]);
            }
            std::sort(span_subdims.begin(), span_subdims.end());
            do {
                int gap = 0;
                for (const auto &delta_gap : span_subdims) {
                    gap += delta_gap;
                    subk_idx_to_possible_gaps[subk_idx].insert(prev_gap_end + gap);
                }
            } while (std::next_permutation(span_subdims.begin(), span_subdims.end()));

            submode_begin = submode_end;
            prev_gap_end = gap_end;
        }
    }

    Vector<int> possible_gaps = intersect(subk_idx_to_possible_gaps);
    assert(!possible_gaps.empty());

    subband.num_bands = std::accumulate(subband.subk_idx_to_e_idx_to_submode[0].begin(),
                                        subband.subk_idx_to_e_idx_to_submode[0].end(),
                                        0,
                                        [this](const int lhs, const auto &submode) {
                                            return lhs + data.sub_msg.dims[submode.subirrep_idx];
                                        });
    assert(subband.num_bands == possible_gaps.back());

    Vector<std::tuple<Vector<int>, Vector<Span>>> gaps_sharedspans_pairs;

    {
        Vector<int> gaps;
        Vector<Span> sharedspans;
        for (int gap = 1; gap <= subband.num_bands; ++gap) {
            if (!gap_to_globalspans.contains(gap)) {
                if (!gaps.empty()) {
                    gaps_sharedspans_pairs.emplace_back(gaps, sharedspans);
                }
                gaps.clear();
                sharedspans.clear();

                if (std::find(possible_gaps.begin(), possible_gaps.end(), gap) !=
                    possible_gaps.end()) {
                    gaps_sharedspans_pairs.emplace_back(Vector<int>{gap}, Spans{});
                }

            } else {

                if (std::find(possible_gaps.begin(), possible_gaps.end(), gap) !=
                    possible_gaps.end()) {
                    gaps.push_back(gap);
                }

                if (gap_to_globalspans.contains(gap)) {
                    for (const auto &new_sharedspan : gap_to_globalspans.at(gap)) {
                        if (std::find_if(sharedspans.begin(),
                                         sharedspans.end(),
                                         [&new_sharedspan](const auto &span) {
                                             return span.begin() == new_sharedspan.begin();
                                         }) == sharedspans.end()) {
                            sharedspans.push_back(new_sharedspan);
                        }
                    }
                }
            }
        }
    }

    for (const auto &[gaps, sharedspans] : gaps_sharedspans_pairs) {
        Vector<Span> allspanstopermute{sharedspans};
        for (const auto &gap : gaps) {
            if (gap_to_localspans.contains(gap)) {
                const auto &localspans = gap_to_localspans.at(gap);
                allspanstopermute.insert(
                    allspanstopermute.end(), localspans.begin(), localspans.end());
            }
        }
        subband.gaps_allspanstopermute_done_tuples.emplace_back(gaps, allspanstopermute, false);
        assert(is_cartesian_sorted(allspanstopermute));
    }

    Vector<int> test1;
    for (const auto &[gaps, _, __] : subband.gaps_allspanstopermute_done_tuples) {
        test1.insert(test1.end(), gaps.begin(), gaps.end());
    }
    assert(test1 == possible_gaps);
    subband.all_possible_gaps = possible_gaps;

    subband.fix_antiunit_rels();

    return subband;
}

bool Subband::next_energetics() {
    for (auto &[gaps, allspanstopermute, done] : gaps_allspanstopermute_done_tuples) {
        if (!done) {
            if (cartesian_permute(allspanstopermute)) {
                fix_antiunit_rels();
                return true;
            } else {
                done = true;
                fix_antiunit_rels();
                return true;
            }
        }
    }

    fix_antiunit_rels();
    return false;
}

Vector<short> Subband::make_br(const Vector<int> &e_idxs_beg,
                               const Vector<int> &e_idxs_end,
                               const SpectrumData &data) {
    Vector<short> result(data.sub_msg.irreps.size(), 0);

    for (unsigned i = 0; i < e_idxs_beg.size(); ++i) {
        for (int j = e_idxs_beg[i]; j < e_idxs_end[i]; ++j) {
            ++result[subk_idx_to_e_idx_to_submode[i][j].subirrep_idx];
        }
    }

    return result;
}

Vector<Vector<int>> Subband::dimvalid_e_idxs() {
    Vector<Vector<int>> result;

    const auto num_ks = subk_idx_to_e_idx_to_submode.size();

    Vector<int> k_idx_to_accum(num_ks, 0);
    Vector<int> k_idx_to_cur_e_idx(num_ks, 0);

    result.push_back(k_idx_to_cur_e_idx);

    auto min_accum_it = k_idx_to_accum.begin();
    auto max_accum_it = min_accum_it;
    int min_accum_k_idx = 0;

    while (k_idx_to_cur_e_idx[min_accum_k_idx] <
           static_cast<int>(subk_idx_to_e_idx_to_submode[min_accum_k_idx].size())) {
        *min_accum_it +=
            data.sub_msg.dims[subk_idx_to_e_idx_to_submode[min_accum_k_idx]
                                                          [k_idx_to_cur_e_idx[min_accum_k_idx]++]
                                                              .subirrep_idx];

        std::tie(min_accum_it, max_accum_it) =
            std::minmax_element(k_idx_to_accum.begin(), k_idx_to_accum.end());
        min_accum_k_idx = std::distance(k_idx_to_accum.begin(), min_accum_it);

        if (*min_accum_it == *max_accum_it) {
            result.push_back(k_idx_to_cur_e_idx);
        }
    }

    return result;
}

bool Superband::cartesian_permute() {
    std::set<int> kidxs_to_skip;
    for (const auto &[_, kidx2, __] : data.super_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples) {
        kidxs_to_skip.insert(kidx2);
    }

    for (int kidx = 0; kidx < static_cast<int>(k_idx_to_e_idx_to_supermode.size()); ++kidx) {
        if (kidxs_to_skip.contains(kidx)) {
            continue;
        }

        auto &supermodes = k_idx_to_e_idx_to_supermode[kidx];
        if (std::next_permutation(supermodes.begin(), supermodes.end())) {
            fix_antiunit_rels();
            return true;
        }
    }

    fix_antiunit_rels();
    return false;
}

std::map<int, std::pair<bool, MatrixInt>> Subband::calc_gap_sis() const {
    std::map<int, std::pair<bool, MatrixInt>> result;

    assert(num_bands >= 1);

    MatrixInt si = 0 * data.si_matrix.col(0);
    MatrixInt cr = 0 * data.comp_rels_matrix.col(0);

    Vector<int> subk_idx_to_numbandsbelow(data.sub_msg.ks.size(), 0);
    Vector<typename Vector<Submode>::const_iterator> subk_idx_to_cur_submode_it;
    for (const auto &e_idx_to_submode : subk_idx_to_e_idx_to_submode) {
        subk_idx_to_cur_submode_it.push_back(e_idx_to_submode.begin());
    }

    for (int gap = 1; gap <= num_bands; ++gap) {
        for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size()); ++subk_idx) {
            auto &numbandsbelow = subk_idx_to_numbandsbelow[subk_idx];
            while (numbandsbelow < gap) {
                auto &submode_it = subk_idx_to_cur_submode_it[subk_idx];
                const int cur_subirrep_idx = submode_it->subirrep_idx;

                numbandsbelow += data.sub_msg.dims[cur_subirrep_idx];

                si += data.si_matrix.col(cur_subirrep_idx);
                cr += data.comp_rels_matrix.col(cur_subirrep_idx);

                ++submode_it;
            }
        }

        bool gapped = cr.isZero();

        if (gapped) {
            const auto numbandsbelow = subk_idx_to_numbandsbelow[0];
            for (int subk_idx = 0; subk_idx < static_cast<int>(data.sub_msg.ks.size());
                 ++subk_idx) {
                assert(subk_idx_to_numbandsbelow[subk_idx] == numbandsbelow);
            }
            if (numbandsbelow != gap) {
                gapped = false;
            }
        }

        result[gap].first = gapped;
        if (gapped) {
            assert(si.size() == static_cast<int>(data.si_orders.size()));
            for (int i = 0; i < si.size(); ++i) {
                si(i) %= data.si_orders[i];
            }
            result[gap].second = si;
        }
    }

    return result;
}

bool Superband::satisfies_antiunit_rels() const {
    for (const auto &[k1_idx, k2_idx, irrep1idx_to_irrep2idx] :
         data.super_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples) {
        const auto &first_e_idx_to_supermode = k_idx_to_e_idx_to_supermode[k1_idx];
        const auto &second_e_idx_to_supermode = k_idx_to_e_idx_to_supermode[k2_idx];

        assert(first_e_idx_to_supermode.size() == second_e_idx_to_supermode.size());
        for (int e_idx = 0; e_idx < static_cast<int>(first_e_idx_to_supermode.size()); ++e_idx) {
            const auto irrep1_idx = first_e_idx_to_supermode[e_idx].superirrep_idx;
            const auto irrep2_idx = second_e_idx_to_supermode[e_idx].superirrep_idx;
            if (irrep1idx_to_irrep2idx.at(irrep1_idx) != irrep2_idx) {
                return false;
            }
        }
    }

    return true;
}

bool Subband::satisfies_antiunit_rels() const {
    for (const auto &[k1_idx, k2_idx, irrep1idx_to_irrep2idx] :
         data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples) {
        const auto &first_e_idx_to_submode = subk_idx_to_e_idx_to_submode[k1_idx];
        const auto &second_e_idx_to_submode = subk_idx_to_e_idx_to_submode[k2_idx];

        assert(first_e_idx_to_submode.size() == second_e_idx_to_submode.size());
        for (int e_idx = 0; e_idx < static_cast<int>(first_e_idx_to_submode.size()); ++e_idx) {
            const auto irrep1_idx = first_e_idx_to_submode[e_idx].subirrep_idx;
            const auto irrep2_idx = second_e_idx_to_submode[e_idx].subirrep_idx;
            if (irrep1idx_to_irrep2idx.at(irrep1_idx) != irrep2_idx) {
                return false;
            }
        }
    }

    return true;
}

void Subband::fix_antiunit_rels() {
    for (const auto &[k1idx, k2idx, irrepidx1_to_irrepidx2] :
         data.sub_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples) {
        const auto &submodes1 = subk_idx_to_e_idx_to_submode[k1idx];
        auto &submodes2 = subk_idx_to_e_idx_to_submode[k2idx];
        for (int i = 0; i < static_cast<int>(submodes1.size()); ++i) {
            submodes2[i] = Submode(irrepidx1_to_irrepidx2.at(submodes1[i].subirrep_idx));
        }
    }
}

void Superband::fix_antiunit_rels() {
    for (const auto &[k1idx, k2idx, irrepidx1_to_irrepidx2] :
         data.super_msg.k1idx_k2idx_irrep1idxtoirrep2idx_tuples) {
        const auto &supermodes1 = k_idx_to_e_idx_to_supermode[k1idx];
        auto &supermodes2 = k_idx_to_e_idx_to_supermode[k2idx];
        for (int i = 0; i < static_cast<int>(supermodes1.size()); ++i) {
            supermodes2[i] = Supermode(
                data.super_msg.irreps[irrepidx1_to_irrepidx2.at(supermodes1[i].superirrep_idx)],
                data);
        }
    }
}

}  // namespace magnon::diagnose2
