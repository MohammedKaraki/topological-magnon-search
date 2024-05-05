#pragma once

#include "diagnose2/search_result.pb.h"
#include "summary/msg_summary.pb.h"

namespace magnon::summary {

// Note: `diagnosed_positive` is different from `positive`. The distinction is due to the cases
// where timeout prevents a resolution on the case.
bool is_diagnosed_positive(const diagnose2::SearchResult &search_result);
bool is_diagnosed_positive(
    const MsgsSummary::MsgSummary::WpsSummary::PerturbationSummary &perturbation_summary);
bool is_diagnosed_positive(const MsgsSummary::MsgSummary::WpsSummary &wps_summary);
bool is_diagnosed_positive(const MsgsSummary::MsgSummary &msg_summary);

}  // namespace magnon::summary
