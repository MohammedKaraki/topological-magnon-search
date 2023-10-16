#include "fmt/core.h"

#include "common/proto_text_format.hpp"
#include "diagnose2/analyze_perturbed_band_structure.hpp"
#include "google/protobuf/text_format.h"

int main(int argc, const char **argv) {
    assert(argc == 2);
    magnon::diagnose2::PerturbedBandStructures structures{};
    magnon::common::proto::read_from_text_file(argv[1], structures);
    // std::string output;
    // assert(google::protobuf::TextFormat::PrintToString(structures, &output));
    // fmt::print("{}\n", output);
    for (const auto &structure : structures.structure()) {
        magnon::diagnose2::analyze_perturbed_band_structure(structure);
    }
}
