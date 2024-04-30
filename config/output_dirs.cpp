#include "config/output_dirs.hpp"

#include "boost/filesystem.hpp"
#include "fmt/core.h"

#include "config/read_global_config.hpp"

namespace magnon {

std::map<std::string, std::string> get_output_dirs() {
    const std::string output_base_dir = read_global_config().output_base_dir();
    if (output_base_dir.empty()) {
        throw std::runtime_error("No output directory path specified!");
    }
    if (output_base_dir.back() == '/') {
        throw std::runtime_error(
            "Invalid output base directory path! Output base directory path must not end with "
            "'/'.");
    }

    std::map<std::string, std::string> result;
    result["output_base_dir"] = output_base_dir;
    result["figures_dir"] = fmt::format("{}/figures_pdf_tex", output_base_dir);
    result["si_tables_dir"] = fmt::format("{}/si_tables_tex", output_base_dir);
    result["gap_tables_dir"] = fmt::format("{}/gap_tables_tex", output_base_dir);
    result["perturbations_table_dir"] = fmt::format("{}/perturbations_table_pb", output_base_dir);
    result["msg_dir"] = fmt::format("{}/msg_tex", output_base_dir);
    result["msg_summary_dir"] = fmt::format("{}/msg_summary_pb", output_base_dir);

    return result;
}

}  // namespace magnon
