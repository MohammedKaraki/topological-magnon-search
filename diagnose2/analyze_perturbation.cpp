#include "diagnose2/analyze_perturbation.hpp"

#include <chrono>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "fmt/core.h"

#include "si_summary.hpp"
#include "spectrum_data.hpp"

namespace magnon::diagnose2 {

namespace {

constexpr const char *GAPLESS = "G";
constexpr const char *TRIVIAL_OR_GAPLESS = "TorG";

using GapRange = std::pair<int, int>;
using Sis = std::vector<std::string>;
using SisSet = std::set<Sis>;
using SiToPossibs = std::map<std::string, std::set<int>>;

auto now() { return std::chrono::high_resolution_clock::now(); }
auto as_seconds(const auto &duration) { return std::chrono::duration<double>(duration).count(); }

std::string si_to_str(const MatrixInt &si) {
    std::ostringstream result;
    for (int i = 0; i < si.size(); ++i) {
        result << si(i);
    }
    return result.str();
}

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

std::tuple<std::string, std::map<std::string, std::set<int>>, std::map<int, std::set<std::string>>>
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

    all_sis.insert(TRIVIAL_OR_GAPLESS);
    all_sis.insert(GAPLESS);

    std::map<std::string, std::set<int>> finalsi_to_possibcounts;
    std::map<std::string, std::set<int>> si_to_possibcounts;

    for (const auto &[gap_range, sis_set] : gap_range_sis_set_pairs) {
        const auto &[gap_begin, gap_end] = gap_range;

        for (const auto &key_si : all_sis) {
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

            if (key_si == TRIVIAL_OR_GAPLESS) {
                for (const auto &sis : sis_set) {
                    cur_possibcounts.insert(
                        std::count_if(sis.begin(), sis.end(), [&trivial_si](const auto &si) {
                            return si == GAPLESS || si == trivial_si;
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
    for (auto count : finalsi_to_possibcounts.at(TRIVIAL_OR_GAPLESS)) {
        assert(count >= 1);
        gappednontrivial_possibcounts_exctopband.insert(num_bands - count);
    }
    finalsi_to_possibcounts["nontrivial"] = gappednontrivial_possibcounts_exctopband;
    finalsi_to_possibcounts.erase(TRIVIAL_OR_GAPLESS);

    finalsi_to_possibcounts["gapless"] = finalsi_to_possibcounts.at(GAPLESS);
    finalsi_to_possibcounts.erase(GAPLESS);

    std::set<int> correct_trivial_counts;
    for (const auto &incorrect_count : finalsi_to_possibcounts.at(trivial_si)) {
        assert(incorrect_count >= 1);
        correct_trivial_counts.insert(incorrect_count - 1);
    }
    finalsi_to_possibcounts.at(trivial_si) = correct_trivial_counts;
    return {trivial_si, finalsi_to_possibcounts, gap_to_possibsis};
}

}  // namespace

PerturbedStructureSearchResult analyze_perturbation(const PerturbedBandStructure &structure,
                                                    double timeout) {
    const auto start_time = now();
    SpectrumData data(structure);
    PerturbedStructureSearchResult result{};

    const auto &positive_energy_irreps = [&]() {
        std::vector<std::string> result{};
        for (const auto &irrep : structure.unperturbed_band_structure().supergroup_little_irrep()) {
            result.push_back(irrep.label());
        }
        return result;
    }();
    auto superband = Superband(positive_energy_irreps, data);
    superband.fix_antiunit_rels();

    Subband subband = superband.make_subband();

    bool type_i_excluded = false;
    std::optional<SiSummary> final_lower, final_upper;

    std::vector<std::pair<GapRange, SisSet>> gap_range_and_sis_set_pairs;
    result.set_is_timeout(false);
    long counter = 0;
    do {

        assert(superband.satisfies_antiunit_rels());

        Subband subband = superband.make_subband();
        std::map<int, std::optional<SiSummary>> firstgap_to_lower, firstgap_to_upper;
        do {
            ++counter;
            if (counter % 1000 == 0) {
                if (timeout > 0.0) {
                    if (as_seconds(now() - start_time) > timeout) {
                        result.set_is_timeout(true);
                        type_i_excluded = true;
                        break;
                    }
                }
            }
            assert(subband.satisfies_antiunit_rels());

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
                        sis.push_back(GAPLESS);
                    }
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

        } while (subband.next_energetics());
    } while (!type_i_excluded && superband.cartesian_permute());

    result.mutable_metadata()->set_compute_time_s(as_seconds(now() - start_time));
    result.set_supergroup_label(structure.supergroup().label());
    result.set_supergroup_number(structure.supergroup().number());
    result.set_subgroup_label(structure.subgroup().label());
    result.set_subgroup_number(structure.subgroup().number());
    *result.mutable_supergroup_from_subgroup_basis() =
        structure.group_subgroup_relation().supergroup_from_subgroup_standard_basis();
    *result.mutable_atomic_orbital() = structure.unperturbed_band_structure().atomic_orbital();

    if (result.is_timeout()) {
        return result;
    }

    if (type_i_excluded) {
        result.set_is_negative_diagnosis(true);
    } else {
        result.set_is_negative_diagnosis(false);
        assert(final_lower);
        assert(final_upper);
        const auto [trivial_si, si_to_possible_counts, gap_to_possibsis] =
            summarize(gap_range_and_sis_set_pairs, subband.get_num_bands());

        for (const auto &[si, possible_counts] : si_to_possible_counts) {
            PerturbedStructureSearchResult::GapCounts gap_counts{};
            for (const auto &gap_count : possible_counts) {
                gap_counts.add_gap_count(gap_count);
            }
            (*result.mutable_si_to_possible_gap_count())[si] = gap_counts;
        }

        for (const auto &[gap, possible_sis] : gap_to_possibsis) {
            PerturbedStructureSearchResult::SIs sis_proto{};
            for (const auto &si : possible_sis) {
                *sis_proto.add_si() = si;
            }
            (*result.mutable_gap_to_possible_si_values())[gap] = sis_proto;
        }
    }
    return result;
}

}  // namespace magnon::diagnose2
