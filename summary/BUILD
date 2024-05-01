package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_proto_library",
    "magnon_py_binary",
    "magnon_py_library",
)

magnon_cc_library(
    name = "visualizer",
    srcs = ["visualizer.cpp"],
    hdrs = ["visualizer.hpp"],
    data = [
        "//config:visualize_config_base_cfg",
    ],
    deps = [
        "//config:visualize_config_proto_cc",
        "//diagnose:entities",
        "//diagnose:latexify",
        "//diagnose2:spectrum_data",
        "//utils:proto_text_format",
        "@fmt",
    ],
)

magnon_cc_library(
    name = "kpath",
    srcs = ["kpath.cpp"],
    hdrs = ["kpath.hpp"],
    deps = [
        "//diagnose2:spectrum_data",
    ],
)

magnon_proto_library(
    name = "msg_summary_proto",
    srcs = ["msg_summary.proto"],
    deps = [
        "//diagnose2:perturbed_band_structure_proto",
        "//diagnose2:search_result_proto",
    ],
)
