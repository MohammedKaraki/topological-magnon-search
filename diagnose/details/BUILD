package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

proto_library(
    name = "config_proto",
    srcs = ["config.proto"],
)

cc_proto_library(
    name = "config_proto_cc",
    deps = [":config_proto"],
)

py_proto_library(
    name = "config_proto_python",
    srcs = ["config.proto"],
)
