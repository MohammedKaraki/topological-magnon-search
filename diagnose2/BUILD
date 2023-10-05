package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_py_binary",
    "magnon_py_library",
    "magnon_py_test",
    "magnon_proto_library",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "analyze_wp_subgroup_pair",
    srcs = ["analyze_wp_subgroup_pair.py"],
    deps = [
    ],
)

magnon_py_test(
    name = "analyze_wp_subgroup_pair_test",
    srcs = ["analyze_wp_subgroup_pair_test.py"],
    deps = [
        ":analyze_wp_subgroup_pair",
    ],
)

magnon_proto_library(
    name = "perturbed_band_structure_proto",
    srcs = ["perturbed_band_structure.proto"],
    deps = [
        "//common:matrix_proto",
        "//groups:magnetic_space_group_proto",
        "//groups:induced_magnetic_space_group_proto",
    ],
)
