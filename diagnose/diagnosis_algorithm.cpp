#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <tuple>

#include "fmt/core.h"
#include "nlohmann/json.hpp"

#include "config/config.pb.h"
#include "entities.hpp"
#include "k_path.hpp"
#include "latexify.hpp"
#include "ostream_utility.hpp"
#include "physics_and_chemistry.hpp"
#include "sisummary.hpp"
#include "spectrum_data.hpp"
#include "visualize.hpp"

using namespace magnon;

namespace std {
template <typename T>
struct less<std::vector<T>> {
    bool operator()(const std::vector<T> &l, const std::vector<T> &r) const {
        return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
    }
};
}  // namespace std

namespace {

const std::string GLOBAL_CONFIG_PATH = "config/config.cfg";

const auto json_dir = "/tmp";
const auto subsection_out_dir = "/tmp";
const auto figure_out_dir = "/tmp";

std::string si_to_str(const IntMatrix &si) {
    std::ostringstream result;
    for (int i = 0; i < si.size(); ++i) {
        result << si(i);
    }
    return result.str();
}

std::string to_humanread(const std::set<int> &counts) {
    std::ostringstream result;
    assert(!counts.empty());
    auto first = counts.begin(), last = counts.end();

    if (counts.size() == 1) {
        result << *first;
        return result.str();
    } else if (counts.size() == 2) {
        result << *first << " or " << *std::next(first);
        return result.str();
    } else if (counts.size() == 3) {
        result << *first << ", " << *std::next(first) << " or " << *std::next(std::next(first));
        return result.str();
    } else {
        auto it = first;
        auto v = *it;
        while (it++ != last) {
            if (*it != ++v) {
                auto it2 = first;
                result << *it2;
                for (; std::next(it2) != last; ++it2) {
                    result << ", " << *it2;
                }
                result << " or " << *it2;
                return result.str();
            }

            result << *first << R"(, \dots, )" << *std::prev(last);
            return result.str();
        }
    }
    assert(false);
}

std::optional<std::pair<Superband, Subband>> find_example(Superband superband,
                                                          const int low,
                                                          const int high,
                                                          const int exact) {
    constexpr auto N = 100'000'000;
    static auto rng = std::mt19937{116};

    for (int i = 0; i < N; ++i) {
        for (int k_idx = 0; k_idx < static_cast<int>(superband.k_idx_to_e_idx_to_supermode.size());
             ++k_idx) {
            auto &supermodes = superband.k_idx_to_e_idx_to_supermode.at(k_idx);
            std::shuffle(supermodes.begin(), supermodes.end(), rng);
        }
        superband.fix_antiunit_rels();
        Subband subband = superband.make_subband();
        for (const auto &[_, spans, __] : subband.gaps_allspanstopermute_done_tuples) {
            for (const auto &span : spans) {
                std::shuffle(span.begin(), span.end(), rng);
            }
        }
        subband.fix_antiunit_rels();

        int trivialorgappless_count = 0;
        for (const auto &[gap, isgapped_and_si] : subband.calc_gap_sis()) {
            const auto &[is_gapped, si] = isgapped_and_si;
            // std::cerr << "GAP: " << gap << " " << is_gapped << " " << si.transpose() << '\n';
            if (!is_gapped || si.isZero()) {
                ++trivialorgappless_count;
            }
        }
        // std::cerr << trivialorgappless_count << '\n';
        assert(trivialorgappless_count >= low);
        assert(trivialorgappless_count <= high);
        if (trivialorgappless_count == exact) {
            return std::pair{superband, subband};
        }
    }

    assert(false);
    return {};
}

std::string transform_less(const std::string &si) {
    if (si == "-") {
        return "gapless";
    }
    return si;
}

using GapRange = std::pair<int, int>;
using Sis = std::vector<std::string>;
using SisSet = std::set<Sis>;
using SiToPossibs = std::map<std::string, std::set<int>>;

std::set<int> all_sums(const std::set<int> &a, const std::set<int> &b) {
    assert(!a.empty());
    assert(!b.empty());

    std::set<int> result;
    for (const auto &x : a) {
        for (const auto &y : b) {
            result.insert(x + y);
        }
    }

    return result;
}

// template<typename T>
// void cartesian_product(auto first_set_it,
//                        auto last_set_it,
//                        std::set<T> cur_set,
//                        std::vector<std::set<int>>& sets_out,
//                        auto binary_op)
// {
//   if (first_set_it == last_set_it) {
//     sets_out.push_back(std::move(cur_set));
//     return;
//   }
//
//   cartesian_product(std::next(first_set_it),
//                     last_set_it,
//                     binary_op(*first_set_it, cur_set),
//                     sets_out,
//                     binary_op);
// }

static std::
    tuple<std::string, std::map<std::string, std::set<int>>, std::map<int, std::set<std::string>>>
    summarize(const std::vector<std::pair<GapRange, SisSet>> &gap_range_sis_set_pairs,
              const int num_bands) {
    std::map<int, std::set<std::string>> gap_to_possibsis;
    for (const auto &[gap_range, sis_set] : gap_range_sis_set_pairs) {
        const auto &[gap_begin, gap_end] = gap_range;
        for (const auto &sis : sis_set) {
            for (int gap = gap_begin; gap <= gap_end; ++gap) {
                gap_to_possibsis[gap].insert(sis.at(gap - gap_begin));
            }
        }
    }

    std::set<std::string> all_sis;
    for (const auto &[gap_range, sis_set] : gap_range_sis_set_pairs) {
        for (const auto &sis : sis_set) {
            for (const auto &si : sis) {
                all_sis.insert(si);
            }
        }
    }

    std::string trivial_si;
    for (const auto &si : all_sis) {
        if (std::all_of(si.begin(), si.end(), [](const char c) { return c == '0'; })) {
            assert(!si.empty());
            for (char c : si) {
                assert(c == '0');
            }
            trivial_si = si;
            break;
        }
    }
    assert(!trivial_si.empty());
    // std::cerr << "Trivial si issssssss: " << trivial_si << '\n';

    all_sis.insert("-0");
    all_sis.insert("-");

    std::map<std::string, std::set<int>> finalsi_to_possibcounts;
    std::map<std::string, std::set<int>> si_to_possibcounts;

    for (const auto &[gap_range, sis_set] : gap_range_sis_set_pairs) {
        const auto &[gap_begin, gap_end] = gap_range;

        for (const auto &key_si : all_sis) {
            // std::cerr << gap_begin << " " << gap_end << '\n';
            // std::cerr << key_si << '\n';
            //
            if (gap_begin == 1) {  // started super Hamiltonian
                si_to_possibcounts[key_si] = {0};
            }
            if (gap_end == 0) {  // finished super Hamiltonian
                assert(gap_begin - 1 == num_bands);

                finalsi_to_possibcounts[key_si].insert(si_to_possibcounts.at(key_si).begin(),
                                                       si_to_possibcounts.at(key_si).end());
                si_to_possibcounts[key_si].clear();
                continue;
            }

            std::set<int> cur_possibcounts;

            if (key_si == "-0") {
                for (const auto &sis : sis_set) {
                    cur_possibcounts.insert(
                        std::count_if(sis.begin(), sis.end(), [&trivial_si](const auto &si) {
                            return si == "-" || si == trivial_si;
                        }));
                }
            } else {
                for (const auto &sis : sis_set) {
                    cur_possibcounts.insert(std::count(sis.begin(), sis.end(), key_si));
                }
            }

            si_to_possibcounts[key_si] = all_sums(si_to_possibcounts.at(key_si), cur_possibcounts);
        }
    }

    std::set<int> gappednontrivial_possibcounts_exctopband;
    for (auto count : finalsi_to_possibcounts.at("-0")) {
        assert(count >= 1);
        gappednontrivial_possibcounts_exctopband.insert(num_bands - count);
    }
    finalsi_to_possibcounts["well-defined \\& nontrivial"] =
        gappednontrivial_possibcounts_exctopband;
    finalsi_to_possibcounts.erase("-0");

    finalsi_to_possibcounts["undefined (gap closed)"] = finalsi_to_possibcounts.at("-");
    finalsi_to_possibcounts.erase("-");

    std::set<int> correct_trivial_counts;
    for (const auto &incorrect_count : finalsi_to_possibcounts.at(trivial_si)) {
        assert(incorrect_count >= 1);
        correct_trivial_counts.insert(incorrect_count - 1);
    }
    finalsi_to_possibcounts.at(trivial_si) = correct_trivial_counts;
    return {trivial_si, finalsi_to_possibcounts, gap_to_possibsis};
}

}  // namespace

namespace magnon {

//
// TODO: Refactor this ugly monster into small, simple library functions.
//
void execute_algorithm(const std::string &msg_number,
                      const std::string &wp,
                      const std::string &subgroup_number,
                      const std::optional<std::string> &config_filename) {
    std::string json_filename_noext_nodir =
        fmt::format(R"({}-{}-{})", msg_number, wp, subgroup_number);

    SpectrumData data;
    {
        auto json_in =
            std::ifstream(fmt::format(R"({}/{}.json)", json_dir, json_filename_noext_nodir));
        json_in >> data;
    }

    // {
    //   LatexDoc doc;
    //   doc << fmt::format(R"(\section{{\label{{subsec:si-{0}}}${1}~({0})$}})" "\n",
    //                      data.sub_msg.number,
    //                      data.sub_msg.label);
    //   doc << "SI formulas:\n";
    //   doc << latexify_sis(data);
    //   doc << "\nCompatibility relations:\n";
    //   doc << latexify_comp_rels(data);
    //   doc.dump(fmt::format("/home/mohammed/Dropbox/research/thesis/appendices/sis/{}.tex",
    //                        data.sub_msg.number),
    //            false);
    // }
    // return 0;

    const std::string main_label =
        fmt::format(R"({0}-{1})", json_filename_noext_nodir, data.sub_msg.number);

    const auto &positive_energy_irreps = data.pos_neg_magnonirreps.first;
    auto superband = Superband(positive_energy_irreps, data);
    superband.fix_antiunit_rels();

    Subband subband = superband.make_subband();

    auto figure_filename_noext_nodir = [&json_filename_noext_nodir,
                                        &data](const std::string &annotation) {
        return fmt::format(
            "{}-{}-figure-{}", json_filename_noext_nodir, data.sub_msg.number, annotation);
    };
    auto figure_filename_noext = [&figure_filename_noext_nodir](const std::string &annotation) {
        return fmt::format("{}/{}", figure_out_dir, figure_filename_noext_nodir(annotation));
    };

    bool type_i_excluded = false;
    int model_counter = 0;
    std::optional<SiSummary> final_lower, final_upper;

    std::vector<std::pair<GapRange, SisSet>> gap_range_and_sis_set_pairs;
    do {
        assert(superband.satisfies_antiunit_rels());

        Subband subband = superband.make_subband();
        std::map<int, std::optional<SiSummary>> firstgap_to_lower, firstgap_to_upper;
        do {
            assert(subband.satisfies_antiunit_rels());

            if (model_counter % 500'000 == 0) {
                std::cerr << "counter: " << model_counter << '\n';
            }

            int gap_bracket_begin = 1;
            int gap_bracket_end = 0;
            for (const auto &[gaps, allspanstopermute, done] :
                 subband.gaps_allspanstopermute_done_tuples) {
                if (!done) {
                    gap_bracket_end = gaps.back();
                    break;
                }
                gap_bracket_begin = gaps.back() + 1;
            }

            GapRange cur_gap_range{gap_bracket_begin, gap_bracket_end};
            if (gap_range_and_sis_set_pairs.empty() ||
                gap_range_and_sis_set_pairs.back().first != cur_gap_range) {
                gap_range_and_sis_set_pairs.emplace_back(cur_gap_range, SisSet{});
            }

            if (gap_bracket_end >= gap_bracket_begin) {
                ++model_counter;
                assert(gap_bracket_end >= gap_bracket_begin);
                const auto gap_to_isgapped_and_si = subband.calc_gap_sis();
                SiSummary cur;

                Sis sis;
                for (int gap = gap_bracket_begin; gap <= gap_bracket_end; ++gap) {
                    const auto &[is_gapped, si] = gap_to_isgapped_and_si.at(gap);

                    if (is_gapped) {
                        cur.increment_si(si);
                        sis.push_back(si_to_str(si));
                    } else {
                        cur.increment_gapless();
                        sis.push_back("-");
                    }
                    // std::cerr << "BRACKET: " << gap_bracket_begin << ", "
                    //   << gap_bracket_end << " : " << is_gapped
                    //   << " : " << si.transpose() << '\n';
                }
                gap_range_and_sis_set_pairs.back().second.insert(sis);

                auto &partial_lower = firstgap_to_lower[gap_bracket_begin];
                auto &partial_upper = firstgap_to_upper[gap_bracket_begin];

                if (!partial_lower) {
                    assert(!partial_upper);
                    partial_lower = cur;
                    partial_upper = cur;
                }

                partial_lower = SiSummary::lower_bound(cur, *partial_lower);
                partial_upper = SiSummary::upper_bound(cur, *partial_upper);

            } else {  // Reached last gap
                SiSummary cur_lower, cur_upper;
                for (const auto &[firstgap, si_summary] : firstgap_to_lower) {
                    cur_lower += si_summary.value();
                }
                for (const auto &[firstgap, si_summary] : firstgap_to_upper) {
                    cur_upper += si_summary.value();
                }

                if (!final_lower) {
                    assert(!final_upper);
                    final_lower = cur_lower;
                    final_upper = cur_upper;
                }
                assert(final_upper);

                final_lower = SiSummary::lower_bound(cur_lower, *final_lower);
                final_upper = SiSummary::upper_bound(cur_upper, *final_upper);

                if (final_upper->get_trivialorgapless_count() >= subband.get_num_bands()) {
                    assert(final_upper->get_trivialorgapless_count() == subband.get_num_bands());
                    type_i_excluded = true;
                    break;
                }
            }

        } while (true && subband.next_energetics());
    } while (true && !type_i_excluded && superband.cartesian_permute());

    // for (const auto& [gap_range, sis_set] : gap_range_and_sis_set_pairs) {
    // std::cerr << fmt::format(R"([{},{}]:)" "\n",
    // gap_range.first, gap_range.second);
    // for (const auto& sis : sis_set) {
    // std::cerr << '\t' << sis << '\n';
    // }
    // }

    const auto [trivial_si, si_to_possibcounts, gap_to_possibsis] =
        summarize(gap_range_and_sis_set_pairs, subband.get_num_bands());
    const auto trivialorgapless_counts = [&si_to_counts = si_to_possibcounts, &subband]() {
        std::vector<int> result;
        const auto &counts = si_to_counts.at("well-defined \\& nontrivial");
        for (auto count : counts) {
            result.push_back(subband.get_num_bands() - 1 - count);
        }
        return result;
    }();
    const auto [low, high] =
        std::minmax_element(trivialorgapless_counts.begin(), trivialorgapless_counts.end());

    // for (const auto& [si, possibcounts] : si_to_possibcounts) {
    // std::cerr << si << ": " << possibcounts << '\n';
    // }
    // std::cerr << '\n';
    // for (const auto& [gap, possiblesis] : gap_to_possibsis) {
    // std::cerr << gap << ": " << possiblesis << '\n';
    // }

    if (type_i_excluded) {
        std::cerr << "\nType-I excluded!\n";
    } else {
        assert(final_lower);
        std::cerr << "\nTotal number of models: " << model_counter << '\n';
        std::cerr << "Lower bounds:\n'";
        final_lower->print(std::cerr);
        std::cerr << '\n';

        assert(final_upper);
        final_upper->print(std::cerr);
    }

    auto make_figure =
        [&data, low = low, high = high, &superband, &config_filename, &figure_filename_noext
         // &figure_filename_noext_nodir
    ](bool target_low) {
            const std::string low_or_high_str = target_low ? "low" : "high";
            const int low_or_high_int = target_low ? *low : *high;

            std::vector<int> subk_idxs;
            auto all_edges = k_idxs_path(data.sub_msg, false);
            subk_idxs.insert(subk_idxs.end(), all_edges.begin(), all_edges.end());
            {
                std::ifstream in("../cpp/drawn_axes");
                int subkidx;
                while (in >> subkidx) {
                    subk_idxs.push_back(subkidx);
                }
            }
            complement_subk_idxs(subk_idxs, data.sub_msg);
            // std::swap(subk_idxs[1], subk_idxs[2]);

            const auto superband_subband_pair_opt =
                find_example(superband, *low + 1, *high + 1, low_or_high_int + 1);
            if (superband_subband_pair_opt) {
                const auto [superband_vis, subband_vis] = superband_subband_pair_opt.value();
                const auto [mode, spec] = mode_spec_pair_from_file(config_filename);
                Visualize vis(subk_idxs, superband_vis, subband_vis, data, mode, spec);
                std::cerr << figure_filename_noext(low_or_high_str) + ".tex"
                          << "\n";
                vis.dump(figure_filename_noext(low_or_high_str) + ".tex");
            }
            // assert(0 ==
            //        std::system(fmt::format(R"(set -x && pdflatex -output-directory={0}
            //        -jobname={1} )"
            //                                R"({2}.tex 1>/dev/null && )"
            //                                R"(cp {0}/{1}.pdf {0}/recent-fig.pdf)",
            //                                figure_out_dir,
            //                                figure_filename_noext_nodir(low_or_high_str),
            //                                figure_filename_noext(low_or_high_str))
            //                        .c_str()));
        };

    // if (type_i_excluded) {
    //   make_figure(false);
    // }
    if (type_i_excluded) {
        std::cerr << "BAD\n";
    } else {
        std::cerr << "GoooooooooOOOOOOooooooOOODDDDDD!!!!!!!!!!!\n";
    }

    // if (!type_i_excluded)
    {
        LatexDoc doc;
        doc << fmt::format(R"(\subsection{{Subgroup )"
                           R"(\texorpdfstring{{${0}~({1})$}}{{{1}}}}})"
                           "\n"
                           R"(\label{{subsec:{2}}})"
                           "\n",
                           data.sub_msg.label,
                           data.sub_msg.number,
                           main_label);
        doc << fmt::format(R"(Upon applying {} the following {},)",
                           data.presc.size() == 1 ? "" : "one of",
                           data.presc.size() == 1 ? "perturbation" : "perturbations");
        doc << R"(\begin{enumerate}[leftmargin=0.75in])"
               "\n";
        for (const auto &presc : data.presc) {
            doc << R"(\item )" << presc << "\n";
        }
        doc << R"(\end{enumerate})"
               "\n";
        doc << fmt::format(R"(the MSG ${0}~({1})$ is reduced to ${2}~({3})$, for )"
                           R"(which the )"
                           R"(SI group is ${4}$ (we use the SI formulas in )"
                           R"(Sec.~\ref{{subsec:si-{3}}}. The standard axes and origin of )"
                           R"(the subgroup are given by )"
                           "\n",
                           data.super_msg.label,
                           data.super_msg.number,
                           data.sub_msg.label,
                           data.sub_msg.number,
                           data.si_orders_to_latex());

        doc << latexify_super_to_sub_v2(data) << "\n";

        // doc << fmt::format(R"(Next, we compute the decomposition of the little-group irreps of
        // the original MSG (${}$) at high-symmetry $k$-points in terms of irreps of the subgroup
        // (${}$).)",
        //                    data.super_msg.label,
        //                    data.sub_msg.label
        //                   );

        for (auto it = data.subk_to_superirrep_to_subirreps.begin();
             it != data.subk_to_superirrep_to_subirreps.end();
             ++it) {
            const auto &[subk, superirrep_to_subirreps] = *it;
            const bool is_first = it == data.subk_to_superirrep_to_subirreps.begin();
            const bool is_last = std::next(it) == data.subk_to_superirrep_to_subirreps.end();
            assert(!is_first || !is_last);

            const auto subk_idx = data.sub_msg.k_to_idx(subk);
            const auto subk_coords = data.sub_msg.kcoords.at(subk_idx);
            const auto &[g, superk] = data.subk_to_g_and_superk.at(subk);
            const auto superk_idx = data.super_msg.k_to_idx(superk);
            const auto superk_coords = data.super_msg.kcoords.at(superk_idx);

            // if (subk == "GM") {
            //   assert(superk == "GM");
            //   doc << fmt::format(
            //     "{} ${}$,\n",
            //     is_first ? " At" : (is_last ? "Finally, at" : "At"),
            //     latexify_gkcoords("", subk, subk_coords)
            //     );
            // } else {
            //   doc << fmt::format(
            //     "{} ${}$, equivalent to ${}$ in the original MSG,\n",
            //     is_first ? " At" : (is_last ? "Finally, at" : "At"),
            //     latexify_gkcoords("", subk, subk_coords),
            //     latexify_gkcoords(g, superk, superk_coords)
            //     );
            // }
            doc << fmt::format(R"(\midrule${}$ & ${}$\\)"
                               "\n"
                               R"(\midrule)"
                               "\n",
                               latexify_gkcoords(g, superk, superk_coords),
                               latexify_gkcoords("", subk, subk_coords));

            // doc << R"(\begin{gather})" "\n";
            // for (auto it = superirrep_to_subirreps.begin();
            //      it != superirrep_to_subirreps.end();
            //      ++it)
            // {
            //   const auto& [superirrep, subirreps] = *it;
            //
            //   std::vector<std::string> latexified_subirreps;
            //   for (const auto& subirrep : subirreps) {
            //     latexified_subirreps.push_back(
            //       fmt::format("{}({})",
            //                   latexify_korirrep(subirrep),
            //                   data.sub_msg.dims.at(
            //                     data.sub_msg.irrep_to_idx(
            //                       subirrep
            //                       )
            //                     )
            //                  )
            //       );
            //   }
            //
            //   doc << fmt::format(R"({}({})\rightarrow {})",
            //                      latexify_korirrep(superirrep),
            //                      data.super_msg.dims.at(
            //                        data.super_msg.irrep_to_idx(superirrep)
            //                        ),
            //                      latexify_irrepsum(latexified_subirreps)
            //                     );
            //   if (std::next(it) != superirrep_to_subirreps.end()) {
            //     doc << R"(\\)";
            //   }
            //   doc << "\n";
            // }
            // doc << R"(\end{gather})" "\n";
            for (auto it = superirrep_to_subirreps.begin(); it != superirrep_to_subirreps.end();
                 ++it) {
                const auto &[superirrep, subirreps] = *it;

                std::vector<std::string> latexified_subirreps;
                for (const auto &subirrep : subirreps) {
                    latexified_subirreps.push_back(
                        fmt::format("{}({})",
                                    latexify_korirrep(subirrep),
                                    data.sub_msg.dims.at(data.sub_msg.irrep_to_idx(subirrep))));
                }

                doc << fmt::format(R"(\multicolumn{{2}}{{|c|}}{{${}({})\rightarrow {}$}}\\)",
                                   latexify_korirrep(superirrep),
                                   data.super_msg.dims.at(data.super_msg.irrep_to_idx(superirrep)),
                                   latexify_irrepsum(latexified_subirreps));
                doc << "\n";
            }
        }

        const int n1 = subband.get_num_bands() - *high - 1;
        const int n2 = subband.get_num_bands() - *low - 1;
        std::string table_caption_1;
        std::string table_caption_2 = fmt::format(
            R"(All the possible SIs that can be assigned to a specificed magnon gap after perturbing the bands induced from the WP~${0}$ of ${1}~({2})$ and lowering the symmetry to the MSG ${3}~({4})$ described in Sec.~\ref{{subsec:{5}}}. Figure.~\ref{{figure:{5}a}} illustrates an energetics example and shows the SIs assigned to the gaps.)",
            data.wp,
            data.super_msg.label,
            data.super_msg.number,
            data.sub_msg.label,
            data.sub_msg.number,
            main_label,
            n1 == 1 ? "one" : std::to_string(n1),
            n1 == 1 ? "gap" : "gaps");
        if (n1 == n2) {
            table_caption_1 = fmt::format(
                R"(The number of gaps, within the bands of a given Hamiltonian, that are assigned a specified SI after perturbing the magnon bands induced from the WP~${0}$ such that the MSG~${1}~({2})$ is lowered to ${3}~({4})$ as described in Sec.~\ref{{subsec:{5}}}. The last row in the table indicates that such a perturbation always leads to~{6} nontrivial {7} (see Figure.~\ref{{figure:{5}a}}))",
                data.wp,
                data.super_msg.label,
                data.super_msg.number,
                data.sub_msg.label,
                data.sub_msg.number,
                main_label,
                n1 == 1 ? "one" : std::to_string(n1),
                n1 == 1 ? "gap" : "gaps");
        } else {
            table_caption_1 = fmt::format(
                R"(The number of gaps, within the bands of a given Hamiltonian, that are assigned a specified SI after perturbing the magnon bands induced from the WP~${0}$ such that the MSG~${1}~({2})$ is lowered to ${3}~({4})$ as described in Sec.~\ref{{subsec:{5}}}. The last row in the table indicates that such a perturbation always leads to at least~{6} (and at most~{8}) nontrivial gaps (see Figure.~\ref{{figure:{5}a}} and Figure.~\ref{{figure:{5}b}}))",
                data.wp,
                data.super_msg.label,
                data.super_msg.number,
                data.sub_msg.label,
                data.sub_msg.number,
                main_label,
                n1 == 1 ? "one" : std::to_string(n1),
                n1 == 1 ? "gap" : "gaps",
                n2 == 1 ? "one" : std::to_string(n2),
                n2 == 1 ? "gap" : "gaps");
        }

        const std::string head = fmt::format(R"(SI (${}$) &Number of Gaps Acquiring SI\\)"
                                             "\n",
                                             data.si_orders_to_latex());

        doc << R"(\begin{center})"
               "\n"
            << R"(\begin{longtable}{cc})"
               "\n"
            << fmt::format(R"(\caption{{{0}\label{{table:counts-{1}}}}}\\)"
                           "\n",
                           table_caption_1,
                           main_label)
            << R"(\toprule
)" << head << R"(\midrule
\endfirsthead
\multicolumn{2}{@{}l}{\ldots continued}\\
\toprule
)" << head << R"(\midrule
\endhead
\multicolumn{2}{r}{Continued on next page}
\endfoot
\bottomrule
\endlastfoot
)";

        // int rows_count = 1;
        for (const auto &[si, possibcounts] : si_to_possibcounts) {
            assert(!possibcounts.empty());

            bool bold_row = true;
            for (const auto &count : possibcounts) {
                if (count == 0) {
                    bold_row = false;
                    break;
                }
            }
            if (si == trivial_si) {
                bold_row = false;
            }
            const std::string bold_or_empty = (bold_row ? R"(\textbf)" : "");

            doc << fmt::format("{0}{{{1}}} & ", bold_or_empty, si);
            doc << fmt::format(R"({0}{{{1}}})", bold_or_empty, to_humanread(possibcounts));
            doc << R"(\\*)"
                   "\n";
        }

        doc << R"(\end{longtable})"
               "\n";
        doc << R"(\end{center})"
               "\n";

        doc << R"(\begin{center})"
               "\n";
        doc << R"(\begin{longtable}{cc})"
               "\n";
        doc << fmt::format(R"(\caption{{{0}\label{{table:possibs-{1}}}}}\\)"
                           "\n",
                           table_caption_2,
                           main_label);
        doc << R"(\toprule)"
               "\n";
        doc << R"(Number of Bands & Possible SIs\\)"
               "\n";
        doc << R"(Below a Gap & of the Gap\\)"
               "\n";
        doc << R"(\midrule)"
               "\n"
               R"(\endfirsthead)"
               "\n"
               R"(\multicolumn{2}{@{}l}{\ldots continued}\\)"
               "\n"
               R"(\toprule)"
               "\n";
        doc << R"(Number of Bands & Possible SIs\\)"
               "\n";
        doc << R"(Below a Gap & of the Gap\\)"
               "\n";
        doc << R"(\midrule)";
        doc << R"(\endhead)"
               "\n";
        doc << R"(\multicolumn{2}{r}{Continued on next page})"
               "\n";
        doc << R"(\endfoot)"
               "\n";
        doc << R"(\bottomrule)"
               "\n";
        doc << R"(\endlastfoot)"
               "\n";

        for (auto pair_it = gap_to_possibsis.begin(); pair_it != gap_to_possibsis.end();
             ++pair_it) {
            const auto &[gap, possibsis] = *pair_it;
            assert(!possibsis.empty());

            const int N = possibsis.size();
            const int max_sis_per_row = 8;
            const int num_rows = 1 + (N - 1) / max_sis_per_row;

            bool bold_row = true;
            for (const auto &si : possibsis) {
                if (si == "-" || si == trivial_si) {
                    bold_row = false;
                    break;
                }
            }
            const std::string bold_or_empty = (bold_row ? R"(\textbf)" : "");

            doc << fmt::format(R"(\multirow{{{1}}}{{*}}{{{0}{{{2}}}}} & {0}{{{3}}})",
                               bold_or_empty,
                               num_rows,
                               gap,
                               transform_less(*possibsis.begin()));

            int si_counter = 1;
            for (auto it = std::next(possibsis.begin()); it != possibsis.end(); ++it) {
                if (si_counter++ % max_sis_per_row == 0) {
                    doc << bold_or_empty
                        << R"({,}\\*)"
                           "\n"
                           R"( & )";
                } else {
                    doc << bold_or_empty << "{,} ";
                }
                doc << bold_or_empty << "{" << transform_less(*it) << "}";
            }
            doc << R"(\\)"
                   "\n";
            if (std::next(pair_it) != gap_to_possibsis.end()) {
                doc << R"(\midrule)"
                       "\n";
            }
        }

        doc << R"(\end{longtable})"
               "\n";
        doc << R"(\end{center})"
               "\n";

        std::string figure1caption, figure2caption;
        if (*low == *high) {
            const int n = subband.get_num_bands() - *high - 1;
            figure1caption = fmt::format(
                R"(An example of an energetics arrangement of the magnon bands induced from the WP ${0}$ of ${1}~({2})$, before and after applying a perturbation which reduces ${1}~({2})$ to ${3}~({4})$ (see Sec.~\ref{{subsec:{5}}}). The perturbed bands exhibit {6} topologically nontrivial {7}. This number of nontrivial gaps is independent of the spin model (Table.~\ref{{table:counts-{5}}}).)",
                data.wp,
                data.super_msg.label,
                data.super_msg.number,
                data.sub_msg.label,
                data.sub_msg.number,
                main_label,
                n == 1 ? "one" : std::to_string(n),
                n == 1 ? "gap" : "gaps");
        } else {
            const int n1 = subband.get_num_bands() - *high - 1;
            const int n2 = subband.get_num_bands() - *low - 1;
            figure1caption = fmt::format(
                R"(An energetics arrangement which results in~{6} nontrivial {7} in the magnon bands induced from the WP~${0}$ of ${1}~({2})$ when a perturbation lowers the symmetry to the MSG~${3}~({4})$ (Sec.~\ref{{subsec:{5}}}). This is a lower bound on the number of nontrivial gaps that arise upon applying such a perturbation~(Table.~\ref{{table:counts-{5}}}). Additional nontrivial gaps can arise for different energetics. Fig.~\ref{{figure:{5}b}} illustrates a different energetics arrangement which exhibits the maximum number~({8}) of nontrivial gaps.)",
                data.wp,
                data.super_msg.label,
                data.super_msg.number,
                data.sub_msg.label,
                data.sub_msg.number,
                main_label,
                n1 == 1 ? "one" : std::to_string(n1),
                n1 == 1 ? "gap" : "gaps",
                n2);
            figure2caption = fmt::format(
                R"(An illustration of the maximum number (${6}$) of nontrivial gaps which can arise after perturbing the magnon bands induced from the WP~${0}$ of ${1}~({2})$ such that the MSG symmetry is a lowered to ${3}~({4})$ (see Table.~\ref{{table:counts-{5}}} in Sec.~\ref{{subsec:{5}}}). Figure.~\ref{{figure:{5}a}} shows a different energetics ordering with the minimum number~({7}) of nontrivial gaps.)",
                data.wp,
                data.super_msg.label,
                data.super_msg.number,
                data.sub_msg.label,
                data.sub_msg.number,
                main_label,
                n2,
                n1);
        }
        make_figure(false);
        doc << fmt::format(R"(\myfigure{{../ch4-figures/{0}.pdf}}{{figure:{1}a}}{{{2}}})"
                           "\n",
                           figure_filename_noext_nodir("high"),
                           main_label,
                           figure1caption);
        if (*low != *high) {
            make_figure(true);
            doc << fmt::format(R"(\myfigure{{../ch4-figures/{0}.pdf}}{{figure:{1}b}}{{{2}}})"
                               "\n",
                               figure_filename_noext_nodir("low"),
                               main_label,
                               figure2caption);
        }

        // doc << latexify_sis(data);
        {
            const std::string filename = fmt::format(R"({}/recent-doc.tex)",
                                                     subsection_out_dir,
                                                     json_filename_noext_nodir,
                                                     data.sub_msg.number);
            doc.dump(filename);
            std::cerr << "output: " << filename << "\n";
        }
        {
            const std::string filename = fmt::format(R"({}/{}-{}.tex)",
                                                     subsection_out_dir,
                                                     json_filename_noext_nodir,
                                                     data.sub_msg.number);
            doc.dump(filename, false);
            std::cerr << "output: " << filename << "\n";
        }
        // assert(0 ==
        //        std::system(fmt::format(R"(set -x && pdflatex -output-directory={0} -jobname={1}
        //        )"
        //                                R"({0}/recent-doc.tex 1>/dev/null&& )"
        //                                R"(cp {0}/{1}.pdf {0}/recent-doc.pdf )",
        //                                subsection_out_dir,
        //                                json_filename_noext_nodir + "-" + data.sub_msg.number)
        //                        .c_str()));
        // assert(0 ==
        //        std::system(fmt::format(R"(set -x && pdflatex -output-directory={0} -jobname={1}
        //        )"
        //                                R"({0}/recent-doc.tex 1>/dev/null&& )"
        //                                R"(cp {0}/{1}.pdf {0}/recent-doc.pdf )",
        //                                subsection_out_dir,
        //                                json_filename_noext_nodir + "-" + data.sub_msg.number)
        //                        .c_str()));
    }

    if (!type_i_excluded) {
        std::cout << fmt::format(
            R"({{
  "type-i": true,
  "trivialorgapless_lower_bound": {},
  "trivialorgapless_upper_bound": {},
  "num_bands": {},
  "super_msg_number": "{}",
  "super_msg_label": "{}",
  "wp": "{}",
  "sub_msg_number": "{}",
  "sub_msg_label": "{}",
  "subgroup_number": "{}",
  "subsection_filename": "{}",
  "subsection_main_label": "{}")"
            "\n}}",
            *low,
            *high,
            subband.get_num_bands(),
            data.super_msg.number,
            data.super_msg.label,
            data.wp,
            data.sub_msg.number,
            data.sub_msg.label,
            subgroup_number,
            fmt::format(R"({}-{}.tex)", json_filename_noext_nodir, data.sub_msg.number),
            main_label);
    } else {
        std::cout << fmt::format(R"({{
  "type-i": false,
  "trivialorgapless_upper_bound": {},
  "num_bands": {},
  "super_msg_number": "{}",
  "super_msg_label": "{}",
  "wp": "{}",
  "sub_msg_number": "{}",
  "sub_msg_label": "{}",
  "subgroup_number": "{}")"
                                 "\n}}",
                                 *high,
                                 subband.get_num_bands(),
                                 data.super_msg.number,
                                 data.super_msg.label,
                                 data.wp,
                                 data.sub_msg.number,
                                 data.sub_msg.label,
                                 subgroup_number);
    }
    nlohmann::json final_json;
    std::array a = {1, 2, 3, 4};
    final_json["a"] = "asdfasf";
    final_json["b"] = true;
    final_json["c"] = a;
    std::cout << final_json << "\n";
}

}  // namespace magnon
