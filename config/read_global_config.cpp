#include "read_global_config.hpp"

#include "utils/proto_text_format.hpp"

namespace magnon {

namespace {

const char *const CONFIG_PATH = "config/config.cfg";

}

magnon::config::GlobalConfig read_global_config() {
    magnon::config::GlobalConfig config;
    magnon::utils::proto::read_from_text_file(CONFIG_PATH, config);
    return config;
}

}  // namespace magnon
