#pragma once

#include "google/protobuf/message_lite.h"

namespace TopoMagnon::proto {
bool read_from_text_file(const std::string& path,
                         ::google::protobuf::MessageLite&);
}
