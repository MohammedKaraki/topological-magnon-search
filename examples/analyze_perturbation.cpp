#include "diagnose2/analyze_perturbation.hpp"

#include <iostream>

#include "diagnose2/perturbed_band_structure.pb.h"
#include "utils/proto_text_format.hpp"

constexpr const char *PROCESSED_TABLES_PATH =
    "diagnose2/test_data/processed_tables_205_33_4a_2_4.txtpb";

int main() {
    magnon::diagnose2::PerturbedBandStructure structure{};
    magnon::utils::proto::read_from_text_file(PROCESSED_TABLES_PATH, structure);

    std::cout << magnon::utils::proto::to_text_format(
        magnon::diagnose2::analyze_perturbation(structure));
}
