package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_library",
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

magnon_cc_library(
    name = "analyze_perturbed_band_structure",
    srcs = ["analyze_perturbed_band_structure.cpp"],
    hdrs = ["analyze_perturbed_band_structure.hpp"],
    deps = [
        ":perturbed_band_structure_proto_cc",
        # ":irreps",
        # ":k_path",
        # ":latexify",
        # ":ostream_utility",
        # ":physics_and_chemistry",
        ":si_summary",
        ":spectrum_data",
        # ":utility",
        # ":visualize",
        # "//config:config_proto_cc",
        "@fmt",
        # "@nlohmann_json//:json",
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
        "//common:comparable",
        "@eigen",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "utility",
    hdrs = ["utility.hpp"],
)
