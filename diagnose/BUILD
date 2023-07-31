load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_binary(
    name = "diagnose",
    srcs = ["diagnose.cpp"],
    deps = [
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
    ],
)

cc_library(
    name = "spectrum_data",
    hdrs = ["spectrum_data.hpp"],
    srcs = ["spectrum_data.cpp"],
    deps = [
        "@eigen",
        "@fmt",
        ":utility",
        ":ostream_utility",
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

cc_library(
    name = "ostream_utility",
    hdrs = ["ostream_utility.hpp"],
    srcs = ["ostream_utility.cpp"],
    deps = [
        # ":spectrum_data",
    ],
)

cc_library(
    name = "utility",
    hdrs = ["utility.hpp"],
    deps = [
    ],
)

cc_library(
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

cc_library(
    name = "visualize",
    hdrs = ["visualize.hpp"],
    srcs = ["visualize.cpp"],
    deps = [
        "@fmt",
        ":spectrum_data",
        ":entities",
        ":latexify",
    ],
)

cc_library(
    name = "k_path",
    hdrs = ["k_path.hpp"],
    srcs = ["k_path.cpp"],
    deps = [
        ":spectrum_data",
    ],
)

cc_library(
    name = "latexify",
    hdrs = ["latexify.hpp"],
    srcs = ["latexify.cpp"],
    deps = [
        "@fmt",
        ":spectrum_data",
        ":physics_and_chemistry",
    ],
)

cc_library(
    name = "physics_and_chemistry",
    hdrs = ["physics_and_chemistry.hpp"],
    srcs = ["physics_and_chemistry.cpp"],
    deps = [
        ":spectrum_data",
        ":irreps",
    ],
)

cc_library(
    name = "irreps",
    hdrs = ["irreps.hpp"],
    srcs = ["irreps.cpp"],
    deps = [
        "@fmt",
    ],
)

cc_library(
    name = "sisummary",
    hdrs = ["sisummary.hpp"],
    deps = [
    ],
)
