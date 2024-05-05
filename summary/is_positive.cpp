#include "summary/is_positive.hpp"

namespace magnon::summary {

// Note: `diagnosed_positive` is different from `positive`. The distinction is due to the cases
// where timeout prevents a resolution on the case.
bool is_diagnosed_positive(const diagnose2::SearchResult &search_result) {
    if (search_result.is_timeout()) {
        return false;
    }
    assert(search_result.has_is_negative_diagnosis());
    return !search_result.is_negative_diagnosis();
}

bool is_diagnosed_positive(
    const MsgsSummary::MsgSummary::WpsSummary::PerturbationSummary &perturbation_summary) {
    if (perturbation_summary.perturbation().subgroup().is_trivial_symmetry_indicator_group()) {
        return false;
    }
    return is_diagnosed_positive(perturbation_summary.search_result());
}

bool is_diagnosed_positive(const MsgsSummary::MsgSummary::WpsSummary &wps_summary) {
    assert(wps_summary.perturbation_summary_size() > 0);
    for (const auto &perturbation_summary : wps_summary.perturbation_summary()) {
        if (is_diagnosed_positive(perturbation_summary)) {
            return true;
        }
    }
    return false;
}

bool is_diagnosed_positive(const MsgsSummary::MsgSummary &msg_summary) {
    assert(msg_summary.wps_summary_size() > 0);
    for (const auto &wps_summary : msg_summary.wps_summary()) {
        if (is_diagnosed_positive(wps_summary)) {
            return true;
        }
    }
    return false;
}

}  // namespace magnon::summary
