#include "boost/filesystem/path.hpp"
#include "fmt/core.h"

#include "config/config.pb.h"
#include "config/read_global_config.hpp"

int main() {
    const auto config = magnon::read_global_config();
    fmt::print("Cache directory: {}\n", config.cache_dir());
    boost::filesystem::path p{config.cache_dir()};
}