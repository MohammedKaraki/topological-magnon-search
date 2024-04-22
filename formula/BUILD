load(
    "//build:defs.bzl",
    "magnon_cc_library",
    "magnon_cc_test",
    "magnon_cc_binary",
    "magnon_proto_library",
    "magnon_py_library",
    "magnon_py_test",
)

magnon_proto_library(
    name = "formula_proto",
    srcs = ["formula.proto"],
)

magnon_cc_library(
    name = "load_formulas",
    srcs = ["load_formulas.cpp"],
    hdrs = ["load_formulas.hpp"],
    deps=[":formula_proto_cc","//groups:magnetic_space_group_proto_cc",],
)

magnon_cc_library(
    name = "parse",
    srcs = ["parse.cpp"],
    hdrs = ["parse.hpp"],
    deps=["@boost//:spirit",],
)

magnon_cc_test(
    name = "parse_test",
    srcs = ["parse_test.cpp"],
    deps=[":parse",
        "@gtest//:gtest_main",
    ],
)
