#pragma once

#include "google/protobuf/message_lite.h"

namespace magnon::proto {
bool read_from_text_file(const std::string &path, ::google::protobuf::MessageLite &);
}
