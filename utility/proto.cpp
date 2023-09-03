#include <fstream>
#include <string>
#include <iterator>

#include "google/protobuf/text_format.h"
#include "utility/proto.hpp"

namespace magnon::proto {

bool read_from_text_file(const std::string &path, ::google::protobuf::Message &message) {
    auto text_file = std::ifstream(path);
    assert(text_file.is_open());
    const std::string text_file_content(std::istreambuf_iterator<char>(text_file),
                                        std::istreambuf_iterator<char>{});
    std::cerr << text_file_content << "\n";
    return ::google::protobuf::TextFormat::ParseFromString(text_file_content, &message);
}

}  // namespace magnon::proto
