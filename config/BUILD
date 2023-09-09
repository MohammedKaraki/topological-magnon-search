package(default_visibility = ["//visibility:public"])

load("//:defs.bzl", "magnon_proto_library", "magnon_cc_library")

filegroup(
    name = "config",
    srcs = ["config.cfg"]
)

magnon_proto_library(
    name = "config_proto",
    srcs = ["config.proto"],
)

magnon_cc_library(
    name = "read_global_config",
    hdrs = ["read_global_config.hpp"],
    srcs = ["read_global_config.cpp"],
    data = [
        "//config:config",
    ],
    deps = [
        ":config_proto_cc",
        "//utility:proto_text_format",
    ],
)
