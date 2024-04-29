package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_proto_library",
    "magnon_py_binary",
    "magnon_py_library",
)

magnon_cc_binary(
    name = "make_diagram",
    srcs = ["make_diagram.cpp"],
    data = ["//data:summary_data"],
    deps = [
        ":kpath",
        ":msg_summary_proto_cc",
        ":visualizer",
        "//diagnose2:perturbed_band_structure_proto_cc",
        "//diagnose2:spectrum_data",
        "//formula:replace_formulas",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
    ],
)

magnon_cc_binary(
    name = "make_msg_summary",
    srcs = ["make_msg_summary.cpp"],
    data = ["//data:summary_data"],
    deps = [
        ":msg_summary_proto_cc",
        "//diagnose2:analyze_perturbation",
        "//formula:replace_formulas",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
    ],
)

magnon_cc_binary(
    name = "make_msg_tex_file",
    srcs = ["make_msg_tex_file.cpp"],
    data = ["//data:summary_data"],
    deps = [
        ":msg_summary_proto_cc",
        ":visualizer",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
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

magnon_py_binary(
    name = "materials_to_msgs_summary",
    data = ["//data:materials.json"],
    main = "materials_to_msgs_summary.py",
    deps = [
        ":msg_summary_proto_py",
    ],
)

magnon_py_binary(
    name = "make_tables",
    srcs = ["make_tables.py"],
    data = ["//data:summary_data"],
    main = "make_tables.py",
    deps = [
        "//diagnose2:perturbed_band_structure_proto_py",
        "//latexify:latexify_result",
        "//run_summary:msg_summary_proto_py",
    ],
)
