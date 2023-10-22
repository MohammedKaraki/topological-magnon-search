#include "diagnose2/analyze_perturbation.hpp"

#include <iostream>

#include "common/proto_text_format.hpp"
#include "diagnose2/perturbed_band_structure.pb.h"

constexpr const char *PROCESSED_TABLES_PATH =
    "diagnose2/test_data/processed_tables_205_33_4a_2_4.txtpb";

int main() {
    magnon::diagnose2::PerturbedBandStructure structure{};
    magnon::common::proto::read_from_text_file(PROCESSED_TABLES_PATH, structure);

    std::cout << magnon::common::proto::to_text_format(
        magnon::diagnose2::analyze_perturbation(structure));
}
