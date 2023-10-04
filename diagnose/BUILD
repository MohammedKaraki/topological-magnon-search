package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_py_binary",
    "magnon_py_library",
)
load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

magnon_cc_binary(
    name = "read_example",
    srcs = ["read_example.cpp"],
    data = [
        "//config",
    ],
    deps = [
        "//common:proto_text_format",
        "//config:config_proto_cc",
    ],
)

magnon_cc_binary(
    name = "diagnose",
    srcs = ["diagnose.cpp"],
    deps = [
        ":diagnosis_algorithm",
        "@boost//:program_options",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "diagnosis_algorithm",
    srcs = ["diagnosis_algorithm.cpp"],
    hdrs = ["diagnosis_algorithm.hpp"],
    deps = [
        ":entities",
        ":irreps",
        ":k_path",
        ":latexify",
        ":ostream_utility",
        ":physics_and_chemistry",
        ":sisummary",
        ":spectrum_data",
        ":utility",
        ":visualize",
        "//config:config_proto_cc",
        "@fmt",
        "@nlohmann_json//:json",
    ],
)

magnon_cc_library(
    name = "spectrum_data",
    srcs = ["spectrum_data.cpp"],
    hdrs = ["spectrum_data.hpp"],
    deps = [
        ":ostream_utility",
        ":utility",
        "//common:comparable",
        "@eigen",
        "@fmt",
        "@nlohmann_json//:json",
    ],
)

cc_test(
    name = "spectrum_data_test",
    srcs = ["spectrum_data_test.cpp"],
    deps = [
        ":spectrum_data",
        "@gtest//:gtest_main",
    ],
)

magnon_cc_library(
    name = "ostream_utility",
    srcs = ["ostream_utility.cpp"],
    hdrs = ["ostream_utility.hpp"],
    deps = [
        # ":spectrum_data",
    ],
)

magnon_cc_library(
    name = "utility",
    hdrs = ["utility.hpp"],
    deps = [
    ],
)

magnon_cc_library(
    name = "entities",
    srcs = ["entities.cpp"],
    hdrs = ["entities.hpp"],
    deps = [
        "@fmt",
        "@eigen",
        # ":ostream_utility",
        ":spectrum_data",
        ":utility",
    ],
)

magnon_cc_library(
    name = "visualize",
    srcs = ["visualize.cpp"],
    hdrs = ["visualize.hpp"],
    data = [
        "//config:visualize_config_base_cfg",
    ],
    deps = [
        ":entities",
        ":latexify",
        ":spectrum_data",
        "//common:proto_text_format",
        "//config:visualize_config_proto_cc",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "k_path",
    srcs = ["k_path.cpp"],
    hdrs = ["k_path.hpp"],
    deps = [
        ":spectrum_data",
    ],
)

magnon_cc_library(
    name = "latexify",
    srcs = ["latexify.cpp"],
    hdrs = ["latexify.hpp"],
    deps = [
        ":physics_and_chemistry",
        ":spectrum_data",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "physics_and_chemistry",
    srcs = ["physics_and_chemistry.cpp"],
    hdrs = ["physics_and_chemistry.hpp"],
    deps = [
        ":irreps",
        ":spectrum_data",
    ],
)

magnon_cc_library(
    name = "irreps",
    srcs = ["irreps.cpp"],
    hdrs = ["irreps.hpp"],
    deps = [
        "//common:comparable",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "sisummary",
    hdrs = ["sisummary.hpp"],
    deps = [
    ],
)
