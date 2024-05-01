#include "config/output_dirs.hpp"

#include <cstdlib>

#include "boost/filesystem.hpp"
#include "fmt/core.h"

#include "config/read_global_config.hpp"

namespace magnon {

namespace {

void create_dir_if_not_exist(const std::string &path) {
    if (!boost::filesystem::exists(path)) {
        boost::filesystem::create_directory(path);
    }
    if (!boost::filesystem::exists(path)) {
        throw std::runtime_error(fmt::format("Failed to create directory \"{}\"", path));
    }
}

void create_directories_if_not_exist(const std::map<std::string, std::string> &dir_map) {
    create_dir_if_not_exist(dir_map.at("output_base_dir"));

    for (const auto &[key, dir] : dir_map) {
        create_dir_if_not_exist(dir);
    }
}

// Expand "~" in a pathname into home directory.
//
// Copied from:
// https://stackoverflow.com/questions/4891006/how-to-create-a-folder-in-the-home-directory
//
std::string expand_user(std::string path) {
    if (not path.empty() and path[0] == '~') {
        assert(path.size() == 1 or path[1] == '/');  // or other error handling
        char const *home = getenv("HOME");
        if (home or ((home = getenv("USERPROFILE")))) {
            path.replace(0, 1, home);
        } else {
            char const *hdrive = getenv("HOMEDRIVE"), *hpath = getenv("HOMEPATH");
            assert(hdrive);
            assert(hpath);
            path.replace(0, 1, std::string(hdrive) + hpath);
        }
    }
    return path;
}

}  // namespace

std::map<std::string, std::string> get_output_dirs() {
    const std::string output_base_dir = expand_user(read_global_config().output_base_dir());
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
    result["si_tables_relative_dir"] = "si_tables_tex";
    result["gap_tables_relative_dir"] = "gap_tables_tex";
    result["figures_relative_dir"] = "figures_pdf_tex";
    result["si_tables_dir"] =
        fmt::format("{}/{}", output_base_dir, result["si_tables_relative_dir"]);
    result["gap_tables_dir"] =
        fmt::format("{}/{}", output_base_dir, result["gap_tables_relative_dir"]);
    result["figures_dir"] = fmt::format("{}/{}", output_base_dir, result["figures_relative_dir"]);
    result["perturbations_dir"] = fmt::format("{}/perturbations_pb", output_base_dir);
    result["msg_tex_dir"] = fmt::format("{}/msg_tex", output_base_dir);
    result["msg_summary_dir"] = fmt::format("{}/msg_summary_pb", output_base_dir);

    create_directories_if_not_exist(result);
    return result;
}

}  // namespace magnon
