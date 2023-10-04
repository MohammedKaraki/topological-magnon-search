#pragma once

#include "google/protobuf/message.h"

namespace magnon::common::proto {

bool read_from_text_file(const std::string &path, ::google::protobuf::Message &message);

}