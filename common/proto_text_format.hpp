#pragma once

#include "google/protobuf/message.h"

namespace magnon::common::proto {

bool read_from_text_file(const std::string &path, ::google::protobuf::Message &message);

std::string to_text_format(const ::google::protobuf::Message &message);

}  // namespace magnon::common::proto
