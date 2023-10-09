#include "fmt/core.h"

#include "common/proto_text_format.hpp"
#include "diagnose2/analyze_perturbed_band_structure.hpp"
#include "google/protobuf/text_format.h"

int main() {
    magnon::diagnose2::PerturbedBandStructure structure{};
    magnon::common::proto::read_from_text_file("/tmp/processed_tables.txtpb", structure);
    // std::string output;
    // assert(google::protobuf::TextFormat::PrintToString(structure, &output));
    // fmt::print("{}\n", output);
    magnon::diagnose2::analyze_perturbed_band_structure(structure);
}
