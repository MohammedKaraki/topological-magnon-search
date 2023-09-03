#pragma once

#include "latexify.hpp"

namespace magnon {

void {
    LatexDoc doc;
    doc << fmt::format(R"(On the Wyckoff position ${0}$ of $G={1}$, the site symmetry group )"
                       R"(representation of the spin-wave variables $(S^x,S^y)$ is ${2}$.)"
                       "\n",
                       data.wp,
                       data.super_msg.label,
                       data.site_irreps_as_str());

    if (supercond_chemistries.size() == 1) {
        doc << fmt::format(R"(The band structure induced from ${0}$ on ${1}$ can be uniquely )"
                           R"(decomposed in terms of EBRs as)",
                           data.site_irreps_as_str(),
                           data.wp);
    } else {
        doc << fmt::format(R"(In terms of EBRs of ${3}$, we find that there are )"
                           R"({2} valid decompositions of the band structure )"
                           R"(induced from ${0}$ on ${1}$:)"
                           "\n",
                           data.site_irreps_as_str(),
                           data.wp,
                           supercond_chemistries.size(),
                           data.super_msg.label);
    }
    doc << latexify_supercond_chemistries(data, supercond_chemistries) << "\n";

    if (physics_and_chemistries_pairs.size() > 1) {
        doc << fmt::format(R"(Consequently, we find {} possible band representations for )"
                           R"(the positive energy magnons:)"
                           "\n",
                           physics_and_chemistries_pairs.size());
    } else {
        doc << R"(This leads to a unique band representation for the positive )"
               R"(energy magnons:)"
               "\n";
    }

    doc << latexify_physics_and_chemistries_pairs(physics_and_chemistries_pairs) << "\n";

    doc << fmt::format(
        R"(For the group ${1}$, we compute and use the following symmetry-indicator {0}.)"
        R"()"
        "\n",
        data.si_orders.size() > 1 ? "formulas" : "formula",
        data.sub_msg.label);
    doc << latexify_sis(data) << "\n";

    doc << "We find the following compatibility relations for isolated bands:"
           "\n";
    doc << latexify_comp_rels(data) << "\n";

    doc.dump(latex_doc_filename);
}

}  // namespace magnon
