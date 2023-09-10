#include "utility/proto_text_format.hpp"

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "google/protobuf/text_format.h"

namespace magnon::proto {

// Returns true if successfully read the file content into a proto message.
bool read_from_text_file(const std::string &path, ::google::protobuf::Message &message) {
    auto text_file = std::ifstream(path);
    if (!text_file.is_open()) {
        throw std::runtime_error("Cannot open file.");
    }
    const std::string text_file_content(std::istreambuf_iterator<char>(text_file),
                                        std::istreambuf_iterator<char>{});
    return ::google::protobuf::TextFormat::ParseFromString(text_file_content, &message);
}

}  // namespace magnon::proto
