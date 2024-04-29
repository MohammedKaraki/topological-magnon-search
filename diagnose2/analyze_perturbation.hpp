#include "diagnose2/perturbed_band_structure.pb.h"
#include "diagnose2/search_result.pb.h"

namespace magnon::diagnose2 {

// Analyze the perturbation and decide if all possible Hamiltonians (for both the unperturbed and
// perturbed systems) lead to topological gaps.
PerturbedStructureSearchResult analyze_perturbation(const PerturbedBandStructure &structure,
                                                    double timeout_s = 0.0);

}  // namespace magnon::diagnose2
