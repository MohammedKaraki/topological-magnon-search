package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_library",
    "magnon_cc_test",
    "magnon_proto_library",
    "magnon_pybind_library",
)

magnon_proto_library(
    name = "irrep_proto",
    srcs = ["irrep.proto"],
    deps = [
        ":kvector_proto",
        ":wyckoff_position_proto",
    ],
)

magnon_proto_library(
    name = "wyckoff_position_proto",
    srcs = ["wyckoff_position.proto"],
)

magnon_proto_library(
    name = "atomic_orbital_proto",
    srcs = ["atomic_orbital.proto"],
    deps = [
        ":irrep_proto",
        ":wyckoff_position_proto",
    ],
)

magnon_proto_library(
    name = "magnetic_space_group_proto",
    srcs = ["magnetic_space_group.proto"],
    deps = [
        ":atomic_orbital_proto",
        ":compatibility_relations_proto",
        ":irrep_proto",
        ":kvector_proto",
        ":wyckoff_position_proto",
        "//utils:matrix_proto",
    ],
)

magnon_proto_library(
    name = "group_subgroup_relation_proto",
    srcs = ["group_subgroup_relation.proto"],
    deps = [
        ":compatibility_relations_proto",
        ":irrep_proto",
        ":kvector_proto",
        ":magnetic_space_group_proto",
        "//utils:matrix_proto",
    ],
)

magnon_proto_library(
    name = "kvector_proto",
    srcs = ["kvector.proto"],
    deps = [
    ],
)

magnon_proto_library(
    name = "character_table_proto",
    srcs = ["character_table.proto"],
    deps = [
        ":irrep_proto",
        "//utils:matrix_proto",
    ],
)

magnon_proto_library(
    name = "compatibility_relations_proto",
    srcs = ["compatibility_relations.proto"],
    deps = [
        ":irrep_proto",
        ":kvector_proto",
    ],
)

magnon_cc_library(
    name = "read_standard_magnetic_space_groups",
    srcs = ["read_standard_magnetic_space_groups.cpp"],
    hdrs = ["read_standard_magnetic_space_groups.hpp"],
    data = [
        "//data:standard_magnetic_space_groups",
    ],
    deps = [
        ":magnetic_space_group_proto_cc",
        "//utils:proto_text_format",
    ],
)

magnon_cc_library(
    name = "find_subgroups",
    srcs = ["find_subgroups.cpp"],
    hdrs = ["find_subgroups.hpp"],
    deps = [
        ":group_subgroup_relation_proto_cc",
        ":read_standard_magnetic_space_groups",
        "@eigen",
        "@fmt",
        "@range_v3//:range-v3",
    ],
)

magnon_cc_test(
    name = "find_subgroups_test",
    srcs = ["find_subgroups_test.cpp"],
    deps = [
        ":find_subgroups",
        "@gtest//:gtest_main",
    ],
)

magnon_pybind_library(
    name = "find_subgroups_py",
    srcs = ["find_subgroups_py.cpp"],
    py_deps = [
        ":group_subgroup_relation_proto_py",
    ],
    deps = [
        ":find_subgroups",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)

magnon_pybind_library(
    name = "read_standard_magnetic_space_groups_py",
    srcs = ["read_standard_magnetic_space_groups_py.cpp"],
    py_deps = [
        ":magnetic_space_group_proto_py",
    ],
    deps = [
        ":read_standard_magnetic_space_groups",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)
