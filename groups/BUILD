package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_proto_library")

magnon_proto_library(
    name = "atomic_orbital_proto",
    srcs = ["atomic_orbital.proto"],
)

magnon_proto_library(
    name = "magnetic_space_group_proto",
    srcs = ["magnetic_space_group.proto"],
    deps = [
        ":atomic_orbital_proto",
        "//common:matrix_proto",
    ],
)

magnon_proto_library(
    name = "induced_magnetic_space_group_proto",
    srcs = ["induced_magnetic_space_group.proto"],
    deps = [
        ":magnetic_space_group_proto",
        "//common:matrix_proto",
    ],
)

magnon_proto_library(
    name = "magnetic_kvector_type_proto",
    srcs = ["magnetic_kvector_type.proto"],
    deps = [
        ":magnetic_space_group_proto",
    ],
)
