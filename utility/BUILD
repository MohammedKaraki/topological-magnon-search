package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_library", "magnon_py_library", "magnon_pybind_library")


cc_library(
    name = "compare",
    hdrs = ["compare.hpp"],
)

magnon_cc_library(
    name = "proto_text_format",
    hdrs = ["proto_text_format.hpp"],
    srcs = ["proto_text_format.cpp"],
    deps = [
        "@com_google_protobuf//:protobuf",
    ]
)
