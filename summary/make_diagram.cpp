#include <cstdlib>
#include <iostream>
#include <set>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "diagnose2/perturbed_band_structure.pb.h"
#include "diagnose2/spectrum_data.hpp"
#include "formula/replace_formulas.hpp"
#include "summary/kpath.hpp"
#include "summary/msg_summary.pb.h"
#include "summary/visualizer.hpp"
#include "utils/proto_text_format.hpp"
#include "config/read_global_config.hpp"

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string msg{};
    std::string output_dir{};
};

const std::string output_dir = magnon::read_global_config().output_dir();
const std::string msg_summary_dir = output_dir + "/msg_summary";
const std::string figures_dir = output_dir + "/figures";

int main(const int argc, const char *const argv[]) {
    using namespace magnon;

    const Args args{argc, argv};
    const std::string msg_summary_filepath = fmt::format("{}/{}.pb.txt", msg_summary_dir, args.msg);
    const auto msg_summary = [&]() {
        summary::MsgsSummary::MsgSummary result{};
        utils::proto::read_from_text_file(msg_summary_filepath, result);
        for (auto &wps_summary : *result.mutable_wps_summary()) {
            for (auto &pert_summary : *wps_summary.mutable_perturbation_summary()) {
                formula::maybe_replace_formula(
                    *pert_summary.mutable_perturbation()->mutable_subgroup());
            }
        }
        return result;
    }();

    std::multiset<std::string> filename_bases{};
    for (const auto &wps_summary : msg_summary.wps_summary()) {
        for (auto &pert_summary : wps_summary.perturbation_summary()) {
            const auto &perturbation = pert_summary.perturbation();
            if (perturbation.subgroup().is_trivial_symmetry_indicator_group()) {
                continue;
            }
            if (pert_summary.search_result().is_negative_diagnosis()) {
                continue;
            }

            const diagnose2::SpectrumData data(perturbation);
            const diagnose2::Superband superband(data.pos_neg_magnonirreps.first, data);
            const diagnose2::Subband subband = superband.make_subband();

            const std::string wps = [&]() {
                std::string result{};
                for (const auto &orbital :
                     perturbation.unperturbed_band_structure().atomic_orbital()) {
                    if (!result.empty()) {
                        result += '+';
                    }
                    result += orbital.wyckoff_position().label();
                }
                return result;
            }();

            const std::string filename = fmt::format("{}_{}_{}_fig.tex",
                                                     perturbation.supergroup().number(),
                                                     perturbation.subgroup().number(),
                                                     wps);
            filename_bases.insert(filename);
            // const int filename_base_count = filename_bases.count(filename);
            const std::string figure_filepath = fmt::format("{}/{}", figures_dir, filename);
            constexpr bool ALL_EDGES = true;
            std::vector kpath_indices = make_kpath_indices(data.sub_msg, !ALL_EDGES);
            complement_kpath_indices(kpath_indices, data.sub_msg);
            Visualizer(kpath_indices, superband, subband, data, {}, {}).dump(figure_filepath);
            std::cerr << fmt::format("Output: {}\n", figure_filepath);
        }
    }
}

Args::Args(const int argc, const char *const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{};
    // clang-format off
    desc.add_options()
        ("help", "Print help message.")
        ("msg", po::value(&msg)->required(), "MSG number");
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
