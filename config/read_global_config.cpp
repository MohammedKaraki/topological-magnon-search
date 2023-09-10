#include "read_global_config.hpp"

#include "utility/proto_text_format.hpp"

namespace magnon {

namespace {

const char *const CONFIG_PATH = "config/config.cfg";

}

magnon::config::GlobalConfig read_global_config() {
    magnon::config::GlobalConfig config;
    magnon::proto::read_from_text_file(CONFIG_PATH, config);
    return config;
}

}
