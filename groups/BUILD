package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_proto_library")

magnon_proto_library(
    name = "magnetic_space_group_proto",
    srcs = ["magnetic_space_group.proto"],
)

magnon_proto_library(
    name = "magnetic_kvector_type_proto",
    srcs = ["magnetic_kvector_type.proto"],
    deps = [
        ":magnetic_space_group_proto",
    ],
)
