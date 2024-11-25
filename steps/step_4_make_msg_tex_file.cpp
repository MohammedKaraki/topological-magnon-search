#include <cstdlib>
#include <fstream>
#include <iostream>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "config/output_dirs.hpp"
#include "range/v3/all.hpp"
#include "summary/is_positive.hpp"
#include "summary/msg_summary.pb.h"
#include "utils/proto_text_format.hpp"

const std::map<std::string, std::string> output_dirs = magnon::get_output_dirs();
const std::string msg_summary_dir = output_dirs.at("msg_summary_dir");
const std::string msg_tex_dir = output_dirs.at("msg_tex_dir");
const std::string si_tables_relative_dir = output_dirs.at("si_tables_relative_dir");
const std::string gap_tables_relative_dir = output_dirs.at("gap_tables_relative_dir");
const std::string figures_relative_dir = output_dirs.at("figures_relative_dir");

struct Args {
    Args(const int argc, const char *const argv[]);

    std::string msg{};
};

using MsgsSummary = magnon::summary::MsgsSummary;
using MsgSummary = MsgsSummary::MsgSummary;
using Msg = magnon::groups::MagneticSpaceGroup;

std::string filter_alnum(const std::string &presc) {
    std::string result;
    for (const char c : presc) {
        if (std::isalnum(c)) {
            result += c;
        }
    }
    return result;
}

class MsgTexGenerator {
 public:
    MsgTexGenerator(const std::string &msg, std::ostream &out)
        : super_msg_number_{msg}, msg_summary_{load_summary(msg)}, out_{out} {}

    void generate();

 private:
    static MsgSummary load_summary(const std::string &pathname);
    template <class T>
    static std::string make_wps_encoding(const T &wps) {
        return wps | ranges::views::join('+') | ranges::to<std::string>;
    }
    std::string make_fig_filepath(const MsgSummary::WpsSummary &wps_summary,
                                  const Msg &subgroup,
                                  const std::string pert_presc,
                                  const std::string extension) {
        return fmt::format("{}/{}_{}_{}_{}_fig.{}",
                           figures_relative_dir,
                           super_msg_number_,
                           subgroup.number(),
                           filter_alnum(pert_presc),
                           make_wps_encoding(wps_summary.wp_label()),
                           extension);
    }
    std::string make_gap_table_pathname(const MsgSummary::WpsSummary &wps_summary,
                                        const Msg &subgroup,
                                        const std::string pert_presc) {
        return fmt::format("{}/{}_{}_{}_{}_table.tex",
                           gap_tables_relative_dir,
                           super_msg_number_,
                           subgroup.number(),
                           filter_alnum(pert_presc),
                           make_wps_encoding(wps_summary.wp_label()));
    }
    std::string make_si_table_pathname(const MsgSummary::WpsSummary &wps_summary,
                                       const Msg &subgroup,
                                       const std::string pert_presc) {
        return fmt::format("{}/{}_{}_{}_{}_table.tex",
                           si_tables_relative_dir,
                           super_msg_number_,
                           subgroup.number(),
                           filter_alnum(pert_presc),
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
    const MsgSummary msg_summary_;
    std::ostream &out_;
};

int main(const int argc, const char *const argv[]) {
    using namespace magnon;
    const Args args{argc, argv};
    const std::string output_filename = fmt::format("{}.tex", args.msg);
    const std::string output_filepath = fmt::format("{}/{}", msg_tex_dir, output_filename);
    std::ofstream out(output_filepath);
    MsgTexGenerator(args.msg, out).generate();
    std::cerr << fmt::format("Output: {}\n", output_filepath);
}

void MsgTexGenerator::generate() {

    if (!is_diagnosed_positive(msg_summary_)) {
        std::cerr << fmt::format("Skipping MSG {}: is_diagnosed_positive() evaluates to false!\n",
                                 human_readable_msg_label(supergroup()));
        return;
    }

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
            "\\textbf{{Nontrivial-SI Subgroups:}} {}.\\\\\n"
            "\n"
            "\\textbf{{Trivial-SI Subgroups:}} {}.\\\\\n"
            "\n",
            subgroups | nontrivial_view | join_view | ranges::to<std::string>,
            subgroups | trivial_view | join_view | ranges::to<std::string>);
    }
    for (const auto &wps_summary : msg_summary_.wps_summary()) {
        if (!is_diagnosed_positive(wps_summary)) {
            // Skipping trivial WPs result.
            continue;
        }

        out_ << fmt::format("\\subsection{{WP: ${}$}}\n",
                            make_wps_encoding(wps_summary.wp_label()));
        const std::string materials =
            wps_summary.example_material() | ranges::views::transform([](const auto &material) {
                std::string result = material.formula();
                if (material.has_temperature_k()) {
                    result = fmt::format("{}~({} K)", result, material.temperature_k());
                }
                if (material.reference_size() > 0) {
                    // #TODO: Handle more than one reference.
                    assert(material.reference_size() == 1);
                    const std::string with_escaped_underscore =
                        material.reference(0) | ranges::views::split('_') |
                        ranges::views::join(std::string("\\_")) | ranges::to<std::string>;
                    result = fmt::format(
                        "{{{0}}}\\footnote{{BCS web page: \\texttt{{\\href{{{1}}} {{{1}}}}}}}",
                        result,
                        with_escaped_underscore);
                }
                return result;
            }) |
            ranges::views::join(std::string(", ")) | ranges::to<std::string>;
        out_ << fmt::format("\\textbf{{BCS Materials:}} {}.\\\\\n", materials);

        for (const auto &pert_summary : wps_summary.perturbation_summary()) {
            if (!is_diagnosed_positive(pert_summary)) {
                // Skipping trivial perturbation.
                continue;
            }
            const auto &pert = pert_summary.perturbation();
            out_ << fmt::format("\\subsubsection{{Topological bands in subgroup ${}$}}\n",
                                human_readable_msg_label(pert.subgroup()));

            out_ << fmt::format(
                "\\textbf{{Perturbation{}:}}\n"
                "\\begin{{itemize}}\n"
                "{}.\n"
                "\\end{{itemize}}\n",
                pert.group_subgroup_relation().perturbation_prescription_size() > 1 ? "s" : "",
                pert.group_subgroup_relation().perturbation_prescription() |
                    ranges::views::transform([](const std::string &presc) {
                        const std::string replacement_1 =
                            presc | ranges::views::split(std::string("perp")) |
                            ranges::views::join(std::string("$\\perp$")) | ranges::to<std::string>;
                        const std::string replacement_2 =
                            replacement_1 | ranges::views::split(std::string("parallel")) |
                            ranges::views::join(std::string("$\\parallel$")) |
                            ranges::to<std::string>;
                        return fmt::format("\\item {}", replacement_2);
                    }) |
                    ranges::views::join(std::string(",\n")) | ranges::to<std::string>);

            const std::string presc = pert.group_subgroup_relation().perturbation_prescription(0);
            out_ << fmt::format(
                "\\begin{{figure}}[H]\n"
                "\\centering\n"
                "\\includegraphics[scale=0.6]{{{0}}}\n"
                "\\caption{{Topological magnon bands in subgroup ${1}$ for magnetic moments on "
                "Wyckoff position{2} ${3}$ of supergroup ${4}$.\\label{{fig_{5}_{6}_{7}_{3}}}}}\n"
                "\\end{{figure}}\n",
                make_fig_filepath(wps_summary, pert.subgroup(), presc, "pdf"),
                human_readable_msg_label(pert.subgroup()),
                wps_summary.wp_label_size() > 1 ? "s" : "",
                make_wps_encoding(wps_summary.wp_label()),
                human_readable_msg_label(pert.supergroup()),
                std::string(pert.supergroup().number()),
                std::string(pert.subgroup().number()),
                filter_alnum(presc));
            out_ << fmt::format("\\input{{{}}}\n",
                                make_gap_table_pathname(wps_summary, pert.subgroup(), presc));
            out_ << fmt::format("\\input{{{}}}\n",
                                make_si_table_pathname(wps_summary, pert.subgroup(), presc));
        }
    }
}

MsgSummary MsgTexGenerator::load_summary(const std::string &msg) {
    MsgSummary result{};
    const std::string msg_summary_pathname = fmt::format("{}/{}.pb.txt", msg_summary_dir, msg);
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
