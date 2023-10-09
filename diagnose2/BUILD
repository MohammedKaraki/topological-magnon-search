package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_proto_library",
    "magnon_py_library",
    "magnon_py_test",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_proto_library(
    name = "perturbed_band_structure_proto",
    srcs = ["perturbed_band_structure.proto"],
    deps = [
        "//common:matrix_proto",
        "//groups:atomic_orbital_proto",
        "//groups:group_subgroup_relation_proto",
        "//groups:magnetic_space_group_proto",
    ],
)

magnon_py_library(
    name = "process_tables",
    srcs = ["process_tables.py"],
    deps = [
        "//diagnose2/utility:group_subgroup_relation",
        "//diagnose2/utility:magnetic_space_group",
        "//diagnose2/utility:s_plus_irrep",
        "//fetch:antiunitarily_related_irreps",
        "//fetch:magnetic_band_representation",
        "//fetch/utility:br",
        "//groups:find_subgroups_py",
        "//groups:read_standard_magnetic_space_groups_py",
    ],
)

magnon_py_test(
    name = "process_tables_test",
    srcs = ["process_tables_test.py"],
    deps = [
        ":process_tables",
    ],
)
