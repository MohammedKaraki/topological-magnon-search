package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_py_binary",
    "magnon_py_library",
)

magnon_cc_binary(
    name = "make_diagram",
    srcs = ["make_diagram.cpp"],
    deps = [
        ":visualizer",
        "//common:proto_text_format",
        "//diagnose2:perturbed_band_structure_proto_cc",
        "//diagnose2:spectrum_data",
        "@boost//:program_options",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "visualizer",
    srcs = ["visualizer.cpp"],
    hdrs = ["visualizer.hpp"],
    data = [
        "//config:visualize_config_base_cfg",
    ],
    deps = [
        "//diagnose:entities",
        "//diagnose:latexify",
        "//diagnose2:spectrum_data",
        "//common:proto_text_format",
        "//config:visualize_config_proto_cc",
        "@fmt",
    ],
)
