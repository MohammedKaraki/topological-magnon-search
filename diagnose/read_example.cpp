#include <iostream>

#include "config/config.pb.h"
#include "utility/proto_text_format.hpp"

int main() {
    magnon::config::GlobalConfig config{};
    magnon::proto::read_from_text_file("config/config.cfg", config);
    // google::protobuf::TextFormat::ParseFromString(R"(cache_dir: "ASDasda")", &config);
    // std::cout << int(config.has_cache_dir()) << "\n";
    std::cout << config.cache_dir() << "\n";
}
