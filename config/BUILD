package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_library",
    "magnon_proto_library",
    "magnon_py_binary",
    "magnon_py_library",
    "magnon_pybind_library",
)

filegroup(
    name = "config",
    srcs = ["config.cfg"],
)

filegroup(
    name = "visualize_config_base_cfg",
    srcs = ["visualize_config_base.cfg"],
)

magnon_proto_library(
    name = "config_proto",
    srcs = ["config.proto"],
)

magnon_proto_library(
    name = "visualize_config_proto",
    srcs = ["visualize_config.proto"],
)

magnon_cc_library(
    name = "read_global_config",
    srcs = ["read_global_config.cpp"],
    hdrs = ["read_global_config.hpp"],
    data = [
        "//config",
    ],
    deps = [
        ":config_proto_cc",
        "//utility:proto_text_format",
    ],
)

magnon_pybind_library(
    name = "read_global_config_python",
    srcs = ["read_global_config_python.cpp"],
    py_deps = [
        ":config_proto_py",
    ],
    deps = [
        ":read_global_config",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)
