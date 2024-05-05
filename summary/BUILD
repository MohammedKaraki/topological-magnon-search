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
        "//config:visualization_config",
    ],
    deps = [
        "//config:visualization_config_proto_cc",
        "//diagnose:entities",
        "//diagnose:latexify",
        "//diagnose2:spectrum_data",
        "//summary:kpath",
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

magnon_cc_library(
    name = "is_positive",
    srcs = ["is_positive.cpp"],
    hdrs = ["is_positive.hpp"],
    deps = [
        ":msg_summary_proto_cc",
        "//diagnose2:search_result_proto_cc",
    ],
)
