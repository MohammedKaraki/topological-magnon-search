load("//build:defs.bzl", "magnon_cc_library")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

package(default_visibility = ["//visibility:public"])

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

pybind_extension(
    name = "proto_text_format_python",
    srcs = ["proto_text_format_python.cpp"],
    deps = [],
)
