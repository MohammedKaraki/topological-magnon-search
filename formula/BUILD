load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_cc_test",
    "magnon_proto_library",
    "magnon_py_library",
    "magnon_py_test",
)

magnon_proto_library(
    name = "formula_proto",
    srcs = ["formula.proto"],
)

magnon_cc_library(
    name = "replace_formulas",
    srcs = ["replace_formulas.cpp"],
    hdrs = ["replace_formulas.hpp"],
    data = [":alternative_formulas.pb.txt"],
    visibility = ["//visibility:public"],
    deps = [
        ":formula_proto_cc",
        ":parse",
        "//diagnose2:perturbed_band_structure_proto_cc",
        "//groups:magnetic_space_group_proto_cc",
        "//utils:matrix_converter",
        "//utils:proto_text_format",
        "@eigen",
    ],
)

magnon_cc_library(
    name = "parse",
    srcs = ["parse.cpp"],
    hdrs = ["parse.hpp"],
    deps = ["@boost//:spirit"],
)

magnon_cc_test(
    name = "parse_test",
    srcs = ["parse_test.cpp"],
    deps = [
        ":parse",
        "@gtest//:gtest_main",
    ],
)
