#include <fstream>

#include "fmt/color.h"
#include "fmt/core.h"

#include "diagnose2/analyze_perturbation.hpp"
#include "diagnose2/search_result.pb.h"
#include "google/protobuf/text_format.h"
#include "utils/proto_text_format.hpp"

constexpr double TIMEOUT_S = 1.0e+10;
constexpr const char *PROCESSED_TABLES_PATH = "/tmp/intermediate_result_1.txtpb";
constexpr const char *OUTPUT_PATH = "/tmp/intermediate_result_2.txtpb";

int main() {
    magnon::diagnose2::PerturbedBandStructures structures{};
    magnon::utils::proto::read_from_text_file(PROCESSED_TABLES_PATH, structures);

    std::ofstream out(OUTPUT_PATH);
    for (const auto &structure : structures.structure()) {
        std::string wp_labels{};
        for (const auto &atomic_orbital : structure.unperturbed_band_structure().atomic_orbital()) {
            wp_labels = wp_labels + "+" + atomic_orbital.wyckoff_position().label();
        }
        std::cerr << fmt::format("{} ({}), WPs {} -> {} ({}): ",
                                 structure.supergroup().label(),
                                 structure.supergroup().number(),
                                 wp_labels,
                                 structure.subgroup().label(),
                                 structure.subgroup().number());
        const auto result = magnon::diagnose2::analyze_perturbation(structure, TIMEOUT_S);
        if (result.is_timeout()) {
            std::cerr << fmt::format(fmt::bg(fmt::color::blue), "Timeout!") << '\n';
        } else if (result.is_negative_diagnosis()) {
            std::cerr << fmt::format(fmt::bg(fmt::color::red), "Negative!!") << '\n';
        } else {
            std::cerr << fmt::format(fmt::bg(fmt::color::green), "Positive!!!") << '\n';
        }

        magnon::diagnose2::SearchResults results{};
        *results.add_search_result() = result;
        std::string output;
        assert(google::protobuf::TextFormat::PrintToString(results, &output));
        out << output << std::endl;
    }
}
