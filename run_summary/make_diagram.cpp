#include <cstdlib>
#include <set>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "diagnose2/perturbed_band_structure.pb.h"
#include "diagnose2/spectrum_data.hpp"
#include "run_summary/visualizer.hpp"
#include "utils/proto_text_format.hpp"

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string input_filename{};
    std::string output_dir{};
};

int main(const int argc, const char *const argv[]) {
    using namespace magnon;

    const Args args{argc, argv};
    const auto perturbations = [&]() {
        diagnose2::PerturbedBandStructures result{};
        utils::proto::read_from_text_file(args.input_filename, result);
        return result;
    }();

    std::multiset<std::string> filename_bases{};
    for (const diagnose2::PerturbedBandStructure &perturbation : perturbations.structure()) {
        if (perturbation.subgroup().is_trivial_symmetry_indicator_group()) {
            continue;
        }

        const diagnose2::SpectrumData data(perturbation);
        const diagnose2::Superband superband(data.pos_neg_magnonirreps.first, data);
        const diagnose2::Subband subband = superband.make_subband();

        const std::string wps = [&]() {
            std::string result{};
            for (const auto &orbital : perturbation.unperturbed_band_structure().atomic_orbital()) {
                result += orbital.wyckoff_position().label();
            }
            return result;
        }();
        const std::string filename_base = fmt::format(
            "{}_{}_{}", perturbation.supergroup().number(), perturbation.subgroup().number(), wps);
        filename_bases.insert(filename_base);
        const int filename_base_count = filename_bases.count(filename_base);
        const std::string output_filename =
            fmt::format("{}/{}_{}_fig.tex", args.output_dir, filename_base, filename_base_count);

        Visualizer({0, 1, 2, 3, 4, 1, 3, 0}, superband, subband, data, {}, {})
            .dump(output_filename);
    }
}

Args::Args(const int argc, const char *const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{};
    // clang-format off
    desc.add_options()
        ("help", "Print help message.")
        ("input_file", po::value(&input_filename)->required(), "Perturbations filename")
        ("output_dir", po::value(&output_dir)->required(), "Directory for output TeX files");
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
