#include <cstdlib>
#include <fstream>

#include "boost/program_options.hpp"
#include "fmt/color.h"
#include "fmt/core.h"

#include "diagnose2/analyze_perturbation.hpp"
#include "diagnose2/search_result.pb.h"
#include "formula/replace_formulas.hpp"
#include "google/protobuf/text_format.h"
#include "utils/proto_text_format.hpp"

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string input_filename{};
    std::string output_filename{};
};

constexpr double TIMEOUT_S = 1.0e+10;
int main(const int argc, const char *const argv[]) {
    using namespace magnon;
    const Args args{argc, argv};
    diagnose2::PerturbedBandStructures perturbed_structures{};
    utils::proto::read_from_text_file(args.input_filename, perturbed_structures);
    diagnose2::SearchResults results{};

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
        const auto result = diagnose2::analyze_perturbation(
            formula::maybe_with_alternative_si_formulas(perturbed_structure), TIMEOUT_S);
        if (result.is_timeout()) {
            std::cerr << fmt::format(fmt::bg(fmt::color::blue), "Timeout!") << '\n';
        } else if (result.is_negative_diagnosis()) {
            std::cerr << fmt::format(fmt::bg(fmt::color::red), "Negative!!") << '\n';
        } else {
            std::cerr << fmt::format(fmt::bg(fmt::color::green), "Positive!!!") << '\n';
        }

        *results.add_search_result() = result;
    }
    std::ofstream out(args.output_filename);
    out << utils::proto::to_text_format(results);
}

Args::Args(const int argc, const char *const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{};
    // clang-format off
    desc.add_options()
        ("help", "Print help message.")
        ("input_file", po::value(&input_filename)->required(), "Perturbations filename")
        ("output_file", po::value(&output_filename)->required(), "Search result output filename");
    // clang-format on

    try {
        po::variables_map vm{};
        po::store(po::parse_command_line(argc, argv, desc), vm);
        if (vm.count("help")) {
            std::cout << desc;
            std::exit(EXIT_SUCCESS);
        }
        po::notify(vm);
    } catch (const po::error &e) {
        std::cerr << "Error: " << e.what() << '\n';
        std::exit(EXIT_SUCCESS);
    }
}
