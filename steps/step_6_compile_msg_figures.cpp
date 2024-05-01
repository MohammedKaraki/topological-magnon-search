#include <cstdlib>
#include <iostream>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "config/output_dirs.hpp"
#include "summary/msg_summary.pb.h"
#include "utils/proto_text_format.hpp"

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string msg{};
};

const std::map<std::string, std::string> output_dirs = magnon::get_output_dirs();
const std::string msg_summary_dir = output_dirs.at("msg_summary_dir");
const std::string figures_dir = output_dirs.at("figures_dir");

int main(const int argc, const char *const argv[]) {
    using namespace magnon;

    const Args args{argc, argv};
    const std::string msg_summary_filepath = fmt::format("{}/{}.pb.txt", msg_summary_dir, args.msg);
    const auto msg_summary = [&]() {
        summary::MsgsSummary::MsgSummary result{};
        utils::proto::read_from_text_file(msg_summary_filepath, result);
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
            const std::string command =
                fmt::format("cd {} && pdflatex {} 1>/dev/null", figures_dir, filename);
            std::cerr << fmt::format("Result: {}, Command: {}\n", system(command.c_str()), command);
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
