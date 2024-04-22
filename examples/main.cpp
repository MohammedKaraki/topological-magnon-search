#include "fmt/color.h"
#include "fmt/core.h"

#include "diagnose2/analyze_perturbation.hpp"
#include "diagnose2/search_result.pb.h"
#include "google/protobuf/text_format.h"
#include "utils/proto_text_format.hpp"

constexpr double TIMEOUT_S = 1.0;

int main(int argc, const char **argv) {
    assert(argc == 2);
    magnon::diagnose2::PerturbedBandStructures structures{};
    magnon::utils::proto::read_from_text_file(argv[1], structures);

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
        std::cout << output << std::endl;
    }
}
