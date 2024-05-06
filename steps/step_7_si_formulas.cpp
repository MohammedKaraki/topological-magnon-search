#include <cstdlib>
#include <fstream>
#include <iostream>

#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "config/output_dirs.hpp"
#include "diagnose/latexify.hpp"
#include "formula/replace_formulas.hpp"
#include "range/v3/all.hpp"
#include "summary/is_positive.hpp"
#include "summary/msg_summary.pb.h"
#include "utils/proto_text_format.hpp"

const std::map<std::string, std::string> output_dirs = magnon::get_output_dirs();
const std::string msg_summary_dir = output_dirs.at("msg_summary_dir");
const std::string output_base_dir = output_dirs.at("output_base_dir");

struct Args {
    Args(const int argc, const char *const argv[]);
};

using MsgsSummary = magnon::summary::MsgsSummary;
using MsgSummary = MsgsSummary::MsgSummary;
using Msg = magnon::groups::MagneticSpaceGroup;

class SiFormulasTexGenerator {
 public:
    SiFormulasTexGenerator(std::ostream &out) : msgs_summary_{load_summary()}, out_{out} {}

    void generate();

 private:
    static MsgsSummary load_summary();
    static MsgSummary load_summary(const std::string &msg) {
        MsgSummary result{};
        const std::string msg_summary_pathname = fmt::format("{}/{}.pb.txt", msg_summary_dir, msg);
        if (!magnon::utils::proto::read_from_text_file(msg_summary_pathname, result)) {
            throw std::runtime_error(
                fmt::format("Unable to read proto file! Pathname: {}.", msg_summary_pathname));
        }
        return result;
    }

    static std::string human_readable_msg_label(const auto &msg) {
        return fmt::format("{}~({})", msg.label(), msg.number());
    }

    void latexify_si_formulas(const magnon::diagnose2::SpectrumData::Msg &group) {
        out_ << R"(\begin{align})" << '\n';
        for (int i = 0; i < group.si_matrix.rows(); ++i) {
            out_ << fmt::format(R"(z_{{{}}} &= {})",
                                i + 1,
                                magnon::latexify_row(group.si_matrix.row(i), group.irreps))
                 << R"( \bmod )" << group.si_orders[i];
            if (i + 1 < group.si_matrix.rows()) {
                out_ << R"(\\)";
            }
            out_ << '\n';
        }
        out_ << "\\end{align}\n\n";
    }

 private:
    const std::string super_msg_number_;
    const MsgsSummary msgs_summary_;
    std::ostream &out_;
};

int main(const int argc, const char *const argv[]) {
    using namespace magnon;
    const Args args{argc, argv};
    const std::string output_filename = "si_formulas.tex";
    const std::string output_filepath = fmt::format("{}/{}", output_base_dir, output_filename);
    std::ofstream out(output_filepath);
    SiFormulasTexGenerator(out).generate();
    std::cerr << fmt::format("Output: {}\n", output_filepath);
}

void SiFormulasTexGenerator::generate() {
    std::map<std::string, magnon::diagnose2::PerturbedBandStructure> subgroup_to_perturbation{};
    for (const auto &msg :
         {"11.55",   "11.57",   "125.367", "125.373", "126.384", "126.386", "12.63",   "127.396",
          "128.408", "130.432", "132.456", "134.481", "135.492", "136.506", "13.74",   "138.525",
          "138.528", "138.529", "139.535", "140.550", "141.551", "141.554", "141.555", "141.557",
          "142.568", "142.570", "14.80",   "148.20",  "14.82",   "14.84",   "15.85",   "15.89",
          "166.102", "166.97",  "167.108", "176.143", "18.22",   "192.252", "203.26",  "205.33",
          "205.36",  "222.103", "224.113", "227.131", "228.139", "229.143", "230.148", "48.260",
          "50.282",  "52.310",  "52.315",  "52.318",  "53.333",  "53.335",  "53.336",  "54.350",
          "54.352",  "56.369",  "56.372",  "56.373",  "56.374",  "57.386",  "57.391",  "58.402",
          "59.409",  "59.410",  "60.431",  "60.432",  "61.433",  "61.439",  "62.441",  "62.446",
          "62.447",  "62.448",  "62.450",  "62.451",  "62.452",  "63.466",  "63.468",  "65.490",
          "67.509",  "69.526",  "70.530",  "73.553",  "74.558",  "74.559",  "85.59",   "85.64",
          "86.73",   "88.81",   "88.86"}) {
        const auto msg_summary = load_summary(msg);
        if (!is_diagnosed_positive(msg_summary)) {
            return;
        }

        for (const auto &wps_summary : msg_summary.wps_summary()) {
            if (!is_diagnosed_positive(wps_summary)) {
                // Skipping trivial WPs result.
                continue;
            }

            for (const auto &pert_summary : wps_summary.perturbation_summary()) {
                if (!is_diagnosed_positive(pert_summary)) {
                    // Skipping trivial perturbation.
                    continue;
                }
                const auto &pert = pert_summary.perturbation();
                const std::string subgroup_number = pert.subgroup().number();
                if (!subgroup_to_perturbation.contains(subgroup_number)) {
                    subgroup_to_perturbation[subgroup_number] = pert;
                }
            }
        }
    }

    out_ << "\\section{SI formulas for subgroups}\n\n";
    for (const auto &[subgroup_number, pert] : subgroup_to_perturbation) {
        out_ << fmt::format("\\subsection{{Subgroup ${}$}}\n",
                            human_readable_msg_label(pert.subgroup()));
        latexify_si_formulas(magnon::diagnose2::SpectrumData{
            magnon::formula::maybe_with_alternative_si_formulas(pert)}
                                 .sub_msg);
    }
}

MsgsSummary SiFormulasTexGenerator::load_summary() {
    MsgsSummary result{};
    const std::string msgs_summary_pathname =
        fmt::format("{}/msgs_summary.pb.txt", output_base_dir);
    if (!magnon::utils::proto::read_from_text_file(msgs_summary_pathname, result)) {
        throw std::runtime_error(
            fmt::format("Unable to read proto file! Pathname: {}.", msgs_summary_pathname));
    }
    return result;
}

Args::Args(const int argc, const char *const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{};
    // clang-format off
    desc.add_options()
        ("help", "Print help message");
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
