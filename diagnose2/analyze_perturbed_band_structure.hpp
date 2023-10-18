#include "diagnose2/perturbed_band_structure.pb.h"
#include "diagnose2/result.pb.h"

namespace magnon::diagnose2 {

SubgroupWyckoffPositionResult analyze_perturbed_band_structure(
    const PerturbedBandStructure &structure, double timeout = 0.0);

}
