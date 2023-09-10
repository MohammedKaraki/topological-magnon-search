#pragma once

#include "config/config.pb.h"

// Returns configs used globally in the project's multiple binary targets.
magnon::config::GlobalConfig read_global_config();
