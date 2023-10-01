#include <optional>
#include <string>

namespace magnon {

int execute_algorithm(const std::string &msg_number,
                      const std::string &wp,
                      const std::string &subgroup_number,
                      const std::optional<std::string> &config_filename);

}
