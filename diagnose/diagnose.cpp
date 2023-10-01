#include <iostream>
#include <string>

#include "boost/optional.hpp"
#include "boost/program_options.hpp"
#include "fmt/core.h"

#include "diagnosis_algorithm.hpp"

namespace po = boost::program_options;


int main(int argc, const char **argv) {
    std::string msg_number, wp, subgroup_index;
    boost::optional<std::string> figure_config_filename{};

    po::options_description command_line_options("Allowed options");
    // clang-format off
    command_line_options.add_options()
        ("help,h", "Print a help message")
        ("msg_number",
         po::value(&msg_number)->required()->value_name("MSG_NUMBER"),
         "The magnetic space group (MSG) number")
        ("wp",
         po::value(&wp)->required()->value_name("WP"),
         "The Wyckoff position of the magnetic atoms")
        ("subgroup_index",
         po::value(&subgroup_index)->required()->value_name("SUBGROUP_INDEX"),
         "The 0-based index of the subgroup")
        ("figure_config_file",
         po::value(&figure_config_filename)->value_name("FIGURE_CONFIG_FILE"),
         "A config file specifying output figure details")
        ;
    // clang-format on

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(command_line_options).run(), vm);
        if (vm.count("help")) {
            std::cout << command_line_options << '\n';
            return 0;
        }
        po::notify(vm);
        magnon::execute_algorithm(msg_number,
                                  wp,
                                  subgroup_index,
                                  figure_config_filename // Convert boost::optional to std::optional
                                      ? std::make_optional(figure_config_filename.value())
                                      : std::nullopt);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}
