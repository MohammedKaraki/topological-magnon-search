#include <fstream>

#include "boost/program_options.hpp"
#include "fmt/color.h"
#include "fmt/core.h"

#include "common/proto_text_format.hpp"
#include "diagnose2/analyze_perturbation.hpp"
#include "diagnose2/search_result.pb.h"
#include "google/protobuf/text_format.h"

namespace po = boost::program_options;

constexpr double TIMEOUT_S = 1.0e+10;
int main(const int argc, const char *const argv[]) {
    po::options_description desc{};
    desc.add_options()("input", po::value<std::string>(), "Perturbed struction filename")(
        "output", po::value<std::string>(), "Search result output filename");
    po::variables_map vm{};
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    assert(vm.count("input") == 1);
    assert(vm.count("output") == 1);

    const std::string input_filename = vm["input"].as<std::string>();
    const std::string output_filename = vm["output"].as<std::string>();

    magnon::diagnose2::PerturbedBandStructures perturbed_structures{};
    magnon::common::proto::read_from_text_file(input_filename, perturbed_structures);
    magnon::diagnose2::SearchResults results{};

    for (const auto &perturbed_structure : perturbed_structures.structure()) {
        std::string wp_labels{};
        for (const auto &atomic_orbital :
             perturbed_structure.unperturbed_band_structure().atomic_orbital()) {
            wp_labels = wp_labels + "+" + atomic_orbital.wyckoff_position().label();
        }
        std::cerr << fmt::format("{} ({}), WPs {} -> {} ({}): ",
                                 perturbed_structure.supergroup().label(),
                                 perturbed_structure.supergroup().number(),
                                 wp_labels,
                                 perturbed_structure.subgroup().label(),
                                 perturbed_structure.subgroup().number());
        const auto result = magnon::diagnose2::analyze_perturbation(perturbed_structure, TIMEOUT_S);
        if (result.is_timeout()) {
            std::cerr << fmt::format(fmt::bg(fmt::color::blue), "Timeout!") << '\n';
        } else if (result.is_negative_diagnosis()) {
            std::cerr << fmt::format(fmt::bg(fmt::color::red), "Negative!!") << '\n';
        } else {
            std::cerr << fmt::format(fmt::bg(fmt::color::green), "Positive!!!") << '\n';
        }

        *results.add_search_result() = result;
    }
    std::ofstream out(output_filename);
    out << magnon::common::proto::to_text_format(results);
}
