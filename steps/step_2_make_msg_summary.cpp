#include <cstdlib>
#include <fstream>
#include <iostream>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "config/output_dirs.hpp"
#include "diagnose2/analyze_perturbation.hpp"
#include "formula/replace_formulas.hpp"
#include "range/v3/all.hpp"
#include "summary/msg_summary.pb.h"
#include "utils/proto_text_format.hpp"

const std::map<std::string, std::string> output_dirs = magnon::get_output_dirs();
const std::string msgs_summary_pathname =
    fmt::format("{}/msgs_summary.pb.txt", output_dirs.at("output_base_dir"));
const std::string msg_summary_dir = output_dirs.at("msg_summary_dir");
const std::string perturbations_dir = output_dirs.at("perturbations_dir");
constexpr double SEARCH_TIMEOUT_S = 60.0;

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string msg{};
};

using MsgsSummary = magnon::summary::MsgsSummary;
using MsgSummary = MsgsSummary::MsgSummary;
using Perturbations = magnon::diagnose2::PerturbedBandStructures;

MsgSummary make_msg_summary(MsgSummary unpopulated_summary);

int main(const int argc, const char *const argv[]) {
    using namespace magnon;
    const Args args{argc, argv};

    MsgsSummary msgs_summary{};
    if (!utils::proto::read_from_text_file(msgs_summary_pathname, msgs_summary)) {
        throw std::runtime_error(
            fmt::format("Unable to read proto file! Pathname: {}.", msgs_summary_pathname));
    }
    const auto has_correct_msg = [&](const auto &msg_summary) {
        return msg_summary.msg_number() == args.msg;
    };
    const int match_count = ranges::count_if(msgs_summary.msg_summary(), has_correct_msg);
    if (match_count != 1) {
        throw std::runtime_error(fmt::format(
            "{} matches found for MSG {}! Expected exactly 1 match.", match_count, args.msg));
    }
    const auto msg_summary_it = ranges::find_if(msgs_summary.msg_summary(), has_correct_msg);
    assert(msg_summary_it != msgs_summary.msg_summary().end());

    const MsgSummary msg_summary = make_msg_summary(*msg_summary_it);
    const std::string output_pathname = fmt::format("{}/{}.pb.txt", msg_summary_dir, args.msg);
    std::ofstream(output_pathname) << utils::proto::to_text_format(msg_summary);
    std::cerr << fmt::format("Output: {}\n", output_pathname);
}

MsgSummary make_msg_summary(MsgSummary unpopulated_summary) {
    using namespace magnon;

    for (auto &wps_summary : *unpopulated_summary.mutable_wps_summary()) {
        const std::string wps_encoding =
            wps_summary.wp_label() | ranges::views::join('+') | ranges::to<std::string>;
        const std::string perturbation_filename =
            fmt::format("{}_{}.pb.txt", unpopulated_summary.msg_number(), wps_encoding);
        const std::string perturbations_filepath =
            fmt::format("{}/{}", perturbations_dir, perturbation_filename);
        Perturbations perturbations{};
        if (!utils::proto::read_from_text_file(perturbations_filepath, perturbations)) {
            throw std::runtime_error(
                fmt::format("Unable to read proto file! Pathname: \"{}\".", perturbation_filename));
        }
        for (const auto &perturbation : perturbations.structure()) {
            auto &perturbation_summary = *wps_summary.add_perturbation_summary();
            perturbation_summary.mutable_perturbation()->CopyFrom(perturbation);
            perturbation_summary.mutable_search_result()->CopyFrom(diagnose2::analyze_perturbation(
                formula::maybe_with_alternative_si_formulas(perturbation), SEARCH_TIMEOUT_S));
        }
    }
    return unpopulated_summary;
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
