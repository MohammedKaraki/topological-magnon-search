#pragma clang diagnostic ignored "-Wunused"
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "range/v3/all.hpp"
#include "summary/msg_summary.pb.h"
#include "utils/proto_text_format.hpp"

constexpr char MSG_SUMMARY_DIR[] = "data/msg_summary";

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string msg{};
    std::string output_dir{};
};

using MsgsSummary = magnon::summary::MsgsSummary;
using MsgSummary = MsgsSummary::MsgSummary;
using Msg = magnon::groups::MagneticSpaceGroup;

class MsgTexGenerator {
 public:
    MsgTexGenerator(const std::string &msg, const std::string &output_dir, std::ostream &out)
        : super_msg_number_{msg},
          output_dir_{output_dir},
          msg_summary_{load_summary(msg)},
          out_{out} {}

    void generate();

 private:
    static MsgSummary load_summary(const std::string &pathname);
    template <class T>
    static std::string make_wps_encoding(const T &wps) {
        return wps | ranges::views::join('+') | ranges::to<std::string>;
    }
    std::string make_fig_filepath(const MsgSummary::WpsSummary &wps_summary,
                                  const Msg &subgroup,
                                  const std::string extension) {
        // TODO: consider duplicate subgroup numbers.
        return fmt::format("figures/{}_{}_{}_fig.{}",
                           super_msg_number_,
                           subgroup.number(),
                           make_wps_encoding(wps_summary.wp_label()),
                           extension);
    }
    std::string make_gap_table_pathname(const MsgSummary::WpsSummary &wps_summary,
                                        const Msg &subgroup) {
        // TODO: consider duplicate subgroup numbers.
        return fmt::format("gap_tables/{}_{}_{}_table.tex",
                           super_msg_number_,
                           subgroup.number(),
                           make_wps_encoding(wps_summary.wp_label()));
    }
    std::string make_si_table_pathname(const MsgSummary::WpsSummary &wps_summary,
                                       const Msg &subgroup) {
        // TODO: consider duplicate subgroup numbers.
        return fmt::format("si_tables/{}_{}_{}_table.tex",
                           super_msg_number_,
                           subgroup.number(),
                           make_wps_encoding(wps_summary.wp_label()));
    }
    const auto &supergroup() const {
        return msg_summary_.wps_summary(0).perturbation_summary(0).perturbation().supergroup();
    }
    auto get_subgroups() const {
        return msg_summary_.wps_summary(0).perturbation_summary() |
               ranges::views::transform([](const auto &perturbation_summary) {
                   return &perturbation_summary.perturbation().subgroup();
               }) |
               ranges::to_vector;
    }
    static std::string human_readable_msg_label(const auto &msg) {
        return fmt::format("{}~({})", msg.label(), msg.number());
    }

 private:
    const std::string super_msg_number_;
    const std::string output_dir_;
    const MsgSummary msg_summary_;
    std::ostream &out_;
};

int main(const int argc, const char *const argv[]) {
    using namespace magnon;
    const Args args{argc, argv};
    const std::string output_filename = fmt::format("{}.tex", args.msg);
    const std::string output_filepath = fmt::format("{}/{}", args.output_dir, output_filename);
    std::ofstream out(output_filepath);
    MsgTexGenerator(args.msg, args.output_dir, out).generate();
    MsgTexGenerator(args.msg, args.output_dir, std::cout).generate();
}

void MsgTexGenerator::generate() {
    { out_ << fmt::format("\\section{{MSG ${}$}}\n", human_readable_msg_label(supergroup())); }

    {
        const std::vector subgroups = get_subgroups();
        const auto nontrivial_view = ranges::views::filter(
            [](const Msg *msg) { return !msg->is_trivial_symmetry_indicator_group(); });
        const auto trivial_view = ranges::views::filter(
            [](const Msg *msg) { return msg->is_trivial_symmetry_indicator_group(); });
        const auto join_view = ranges::views::transform([](const Msg *msg) {
                                   return "$" + human_readable_msg_label(*msg) + "$";
                               }) |
                               ranges::views::join(std::string(", "));
        out_ << fmt::format(
            "Nontrivial-SI Subgroups: {}.\\\\\n"
            "Trivial-SI Subgroups: {}.\\\\\n",
            subgroups | nontrivial_view | join_view | ranges::to<std::string>,
            subgroups | trivial_view | join_view | ranges::to<std::string>);
    }
    for (const auto &wps_summary : msg_summary_.wps_summary()) {
        out_ << fmt::format("\\subsection{{WP: ${}$}}\n",
                            make_wps_encoding(wps_summary.wp_label()));
        const std::string materials =
            wps_summary.example_material() | ranges::views::transform([](const auto &material) {
                std::string result = material.formula();
                if (material.has_temperature_k()) {
                    result = fmt::format("{}~({} K)", result, material.temperature_k());
                }
                return result;
            }) |
            ranges::views::join(std::string(", ")) | ranges::to<std::string>;
        out_ << fmt::format("BCS Materials: {}\n", materials);

        for (const auto &pert_summary : wps_summary.perturbation_summary()) {
            const auto &pert = pert_summary.perturbation();
            const auto &result = pert_summary.search_result();
            if (pert.subgroup().is_trivial_symmetry_indicator_group()) {
                continue;
            }
            result.has_is_negative_diagnosis();
            if (result.is_negative_diagnosis()) {
                continue;
            }

            out_ << fmt::format("\\subsubsection{{Topological bands in subgroup ${}$}}\n",
                                human_readable_msg_label(pert.subgroup()));
            out_ << fmt::format("\\includegraphics[scale=0.6]{{{}}}\n",
                                make_fig_filepath(wps_summary, pert.subgroup(), "pdf"));
            out_ << fmt::format("\\input{{{}}}\n",
                                make_gap_table_pathname(wps_summary, pert.subgroup()));
            out_ << fmt::format("\\input{{{}}}\n",
                                make_si_table_pathname(wps_summary, pert.subgroup()));
        }
    }
}

MsgSummary MsgTexGenerator::load_summary(const std::string &msg) {
    MsgSummary result{};
    const std::string msg_summary_pathname = fmt::format("{}/{}.pb.txt", MSG_SUMMARY_DIR, msg);
    if (!magnon::utils::proto::read_from_text_file(msg_summary_pathname, result)) {
        throw std::runtime_error(
            fmt::format("Unable to read proto file! Pathname: {}.", msg_summary_pathname));
    }
    return result;
}

Args::Args(const int argc, const char *const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{};
    // clang-format off
    desc.add_options()
        ("help", "Print help message")
        ("msg", po::value(&msg)->required(), "MSG number")
        ("output_dir", po::value(&output_dir)->required(), "Directory for output msg tex file");
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
