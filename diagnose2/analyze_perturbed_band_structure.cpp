#include "diagnose2/analyze_perturbed_band_structure.hpp"

#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "si_summary.hpp"
#include "spectrum_data.hpp"

namespace magnon::diagnose2 {

namespace {

using GapRange = std::pair<int, int>;
using Sis = std::vector<std::string>;
using SisSet = std::set<Sis>;
using SiToPossibs = std::map<std::string, std::set<int>>;

std::string si_to_str(const MatrixInt &si) {
    std::ostringstream result;
    for (int i = 0; i < si.size(); ++i) {
        result << si(i);
    }
    return result.str();
}

}  // namespace

void analyze_perturbed_band_structure(const PerturbedBandStructure &structure) {
    SpectrumData data(structure);

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
}

}  // namespace magnon::diagnose2
