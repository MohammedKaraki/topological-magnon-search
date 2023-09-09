#include "read_global_config.hpp"
#include "utility/proto_text_format.hpp"

namespace {

const char *const CONFIG_PATH = "config/config.cfg";

}

magnon::config::proto::GlobalConfig read_global_config() {
    magnon::config::proto::GlobalConfig config;
    magnon::proto::read_from_text_file(CONFIG_PATH, config);
    return config;
}
