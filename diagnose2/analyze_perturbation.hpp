#include "diagnose2/perturbed_band_structure.pb.h"
#include "diagnose2/result.pb.h"

namespace magnon::diagnose2 {

// Analyze the perturbation and decide if all possible Hamiltonians (for both the unperturbed and
// perturbed systems) lead to topological gaps.
SubgroupWyckoffPositionResult analyze_perturbation(const PerturbedBandStructure &structure,
                                                   double timeout = 0.0);

}  // namespace magnon::diagnose2
