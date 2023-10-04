package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_library", "magnon_proto_library", "magnon_py_library", "magnon_pybind_library")

cc_library(
    name = "comparable",
    hdrs = ["comparable.hpp"],
)

magnon_cc_library(
    name = "proto_text_format",
    srcs = ["proto_text_format.cpp"],
    hdrs = ["proto_text_format.hpp"],
    deps = [
        "@com_google_protobuf//:protobuf",
        "@fmt",
    ],
)

magnon_proto_library(
    name = "matrix_proto",
    srcs = ["matrix.proto"],
)
