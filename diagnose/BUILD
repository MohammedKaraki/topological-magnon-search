package(default_visibility=["//visibility:public"])

load("//build:defs.bzl",
     "magnon_py_library",
     "magnon_py_binary",
     "magnon_cc_library",
     "magnon_cc_binary",
     )

load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

magnon_cc_binary(
    name = "read_example",
    srcs = ["read_example.cpp"],
    data = [
        "//config:config",
    ],
    deps = [
        "//config:config_proto_cc",
        "//utility:proto_text_format",
    ],
)

magnon_cc_binary(
    name = "diagnose",
    srcs = ["diagnose.cpp"],
    deps = [
        "//config:config_proto_cc",
        ":irreps",
        ":physics_and_chemistry",
        ":sisummary",
        ":latexify",
        ":k_path",
        ":visualize",
        ":utility",
        ":entities",
        ":ostream_utility",
        ":spectrum_data",
        "@fmt",
        "@nlohmann_json//:json",
    ],
)

magnon_cc_library(
    name = "spectrum_data",
    hdrs = ["spectrum_data.hpp"],
    srcs = ["spectrum_data.cpp"],
    deps = [
        "@eigen",
        "@fmt",
        ":utility",
        ":ostream_utility",
        "//utility:comparable",
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
    hdrs = ["ostream_utility.hpp"],
    srcs = ["ostream_utility.cpp"],
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
    hdrs = ["entities.hpp"],
    srcs = ["entities.cpp"],
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
    hdrs = ["visualize.hpp"],
    srcs = ["visualize.cpp"],
    data = [
        "//config:visualize_config_base_cfg",
    ],
    deps = [
        "@fmt",
        ":spectrum_data",
        ":entities",
        ":latexify",
        "//config:visualize_config_proto_cc",
        "//utility:proto_text_format"
    ],
)

magnon_cc_library(
    name = "k_path",
    hdrs = ["k_path.hpp"],
    srcs = ["k_path.cpp"],
    deps = [
        ":spectrum_data",
    ],
)

magnon_cc_library(
    name = "latexify",
    hdrs = ["latexify.hpp"],
    srcs = ["latexify.cpp"],
    deps = [
        "@fmt",
        ":spectrum_data",
        ":physics_and_chemistry",
    ],
)

magnon_cc_library(
    name = "physics_and_chemistry",
    hdrs = ["physics_and_chemistry.hpp"],
    srcs = ["physics_and_chemistry.cpp"],
    deps = [
        ":spectrum_data",
        ":irreps",
    ],
)

magnon_cc_library(
    name = "irreps",
    hdrs = ["irreps.hpp"],
    srcs = ["irreps.cpp"],
    deps = [
        "@fmt",
        "//utility:comparable",
    ],
)

magnon_cc_library(
    name = "sisummary",
    hdrs = ["sisummary.hpp"],
    deps = [
    ],
)


