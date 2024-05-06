package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_library",
    "magnon_cc_test",
    "magnon_proto_library",
    "magnon_py_library",
    "magnon_py_test",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_proto_library(
    name = "search_result_proto",
    srcs = ["search_result.proto"],
    deps = [
        "//groups:atomic_orbital_proto",
        "//utils:matrix_proto",
    ],
)

magnon_proto_library(
    name = "perturbed_band_structure_proto",
    srcs = ["perturbed_band_structure.proto"],
    deps = [
        "//groups:atomic_orbital_proto",
        "//groups:group_subgroup_relation_proto",
        "//groups:irrep_proto",
        "//groups:magnetic_space_group_proto",
        "//utils:matrix_proto",
    ],
)

magnon_py_library(
    name = "process_tables",
    srcs = ["process_tables.py"],
    deps = [
        ":perturbed_band_structure_proto_py",
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
    data = [
        "//diagnose2/test_data",
    ],
    deps = [
        ":process_tables",
    ],
)

magnon_cc_library(
    name = "analyze_perturbation",
    srcs = ["analyze_perturbation.cpp"],
    hdrs = ["analyze_perturbation.hpp"],
    deps = [
        ":perturbed_band_structure_proto_cc",
        ":search_result_proto_cc",
        ":si_summary",
        ":spectrum_data",
        "@fmt",
    ],
)

magnon_cc_test(
    name = "analyze_perturbation_test",
    srcs = ["analyze_perturbation_test.cpp"],
    data = [
        "//diagnose2/test_data",
    ],
    deps = [
        ":analyze_perturbation",
        "//utils:proto_text_format",
        "@gtest//:gtest_main",
    ],
)

magnon_cc_library(
    name = "si_summary",
    hdrs = ["si_summary.hpp"],
    deps = [
        ":spectrum_data",
    ],
)

magnon_cc_library(
    name = "spectrum_data",
    srcs = ["spectrum_data.cpp"],
    hdrs = ["spectrum_data.hpp"],
    deps = [
        ":perturbed_band_structure_proto_cc",
        ":utility",
        "//utils:comparable",
        "//utils:matrix_converter",
        "@eigen",
        "@fmt",
        "@range-v3",
    ],
)

magnon_cc_library(
    name = "utility",
    hdrs = ["utility.hpp"],
)
