#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "fmt/format.h"

#include "google/protobuf/io/tokenizer.h"
#include "google/protobuf/text_format.h"
#include "utility/proto_text_format.hpp"

namespace magnon::proto {

namespace details {

using google::protobuf::io::ColumnNumber;

class StrictErrorCollector : public google::protobuf::io::ErrorCollector {
 public:
    virtual void RecordError(int line, ColumnNumber column, absl::string_view message) {
        throw std::runtime_error(fmt::format(
            "Error during protobuf message parsing:\n{}:{}:{}\n", line, column, message));
    }
};

}  // namespace details

// Returns true if successfully read the file content into a proto message.
bool read_from_text_file(const std::string &path, ::google::protobuf::Message &message) {
    auto text_file = std::ifstream(path);
    if (!text_file.is_open()) {
        throw std::runtime_error("Cannot open file.");
    }
    const std::string text_file_content(std::istreambuf_iterator<char>(text_file),
                                        std::istreambuf_iterator<char>{});
    ::google::protobuf::TextFormat::Parser parser{};
    details::StrictErrorCollector error_collector{};
    parser.RecordErrorsTo(&error_collector);
    return parser.ParseFromString(text_file_content, &message);
}

}  // namespace magnon::proto
