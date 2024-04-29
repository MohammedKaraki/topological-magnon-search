#pragma clang diagnostic ignored "-Wunused"
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "diagnose2/analyze_perturbation.hpp"
#include "formula/replace_formulas.hpp"
#include "range/v3/all.hpp"
#include "run_summary/msg_summary.pb.h"
#include "utils/proto_text_format.hpp"

constexpr char MSGS_SUMMARY_PATHNAME[] = "data/msgs_summary.pb.txt";
constexpr char PERTURBATIONS_DIR[] = "data/perturbations";

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string msg{};
    std::string output_dir{};
};

using MsgsSummary = magnon::summary::MsgsSummary;
using MsgSummary = MsgsSummary::MsgSummary;
using Perturbations = magnon::diagnose2::PerturbedBandStructures;

MsgSummary make_msg_summary(MsgSummary unpopulated_summary);

int main(const int argc, const char *const argv[]) {
    using namespace magnon;
    const Args args{argc, argv};

    MsgsSummary msgs_summary{};
    if (!utils::proto::read_from_text_file(MSGS_SUMMARY_PATHNAME, msgs_summary)) {
        throw std::runtime_error(
            fmt::format("Unable to read proto file! Pathname: {}.", MSGS_SUMMARY_PATHNAME));
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
    const std::string output_pathname = fmt::format("{}/{}.pb.txt", args.output_dir, args.msg);
    std::ofstream(output_pathname) << utils::proto::to_text_format(msg_summary);
}

MsgSummary make_msg_summary(MsgSummary unpopulated_summary) {
    using namespace magnon;

    for (auto &wps_summary : *unpopulated_summary.mutable_wps_summary()) {
        const std::string wps_encoding =
            wps_summary.wp_label() | ranges::views::join('+') | ranges::to<std::string>;
        const std::string perturbation_filename =
            fmt::format("{}_{}.pb.txt", unpopulated_summary.msg_number(), wps_encoding);
        const std::string perturbations_filepath =
            fmt::format("{}/{}", PERTURBATIONS_DIR, perturbation_filename);
        Perturbations perturbations{};
        if (!utils::proto::read_from_text_file(perturbations_filepath, perturbations)) {
            throw std::runtime_error(
                fmt::format("Unable to read proto file! Pathname: \"{}\".", perturbation_filename));
        }
        for (const auto &perturbation : perturbations.structure()) {
            auto &perturbation_summary = *wps_summary.add_perturbation_summary();
            perturbation_summary.mutable_perturbation()->CopyFrom(perturbation);
            perturbation_summary.mutable_search_result()->CopyFrom(diagnose2::analyze_perturbation(
                formula::maybe_with_alternative_si_formulas(perturbation)));
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
        ("msg", po::value(&msg)->required(), "MSG number")
        ("output_dir", po::value(&output_dir)->required(), "Directory for output msg summary");
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
