package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_proto_library",
"magnon_cc_library",
"magnon_cc_test",
     )
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

magnon_proto_library(
    name = "character_table_proto",
    srcs = ["character_table.proto"],
    deps = [
        ":atomic_orbital_proto",
        "//common:matrix_proto",
    ],
)

magnon_proto_library(
    name = "compatibility_relations_proto",
    srcs = ["compatibility_relations.proto"],
    deps = [
        ":atomic_orbital_proto",
    ],
)

magnon_cc_library(
    name = "find_subgroups",
    hdrs = ["find_subgroups.hpp"],
     srcs= ["find_subgroups.cpp"],
    deps = [
        ":induced_magnetic_space_group_proto_cc",
        "@eigen",
        "@fmt",
        "@range_v3//:range-v3",
    ],
)

magnon_cc_test(
    name = "find_subgroups_test",
    srcs = ["find_subgroups_test.hpp"],
    deps = [
        ":find_subgroups",
        "@gtest//:gtest_main",
    ],
)
