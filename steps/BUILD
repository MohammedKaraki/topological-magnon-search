package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_proto_library",
    "magnon_py_binary",
    "magnon_py_library",
)

magnon_py_binary(
    name = "step_1_create_incomplete_msgs_summary",
    data = ["//data:materials.json"],
    main = "step_1_create_incomplete_msgs_summary.py",
    deps = [
        "//config:output_dirs_python",
        "//summary:msg_summary_proto_py",
    ],
)

magnon_cc_binary(
    name = "step_2_make_msg_summary",
    srcs = ["step_2_make_msg_summary.cpp"],
    data = ["//data:summary_data"],
    deps = [
        "//config:output_dirs",
        "//diagnose2:analyze_perturbation",
        "//formula:replace_formulas",
        "//summary:msg_summary_proto_cc",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
    ],
)

magnon_cc_binary(
    name = "step_3_make_msg_diagrams",
    srcs = ["step_3_make_msg_diagrams.cpp"],
    data = ["//data:summary_data"],
    deps = [
        "//config:output_dirs",
        "//diagnose2:perturbed_band_structure_proto_cc",
        "//diagnose2:spectrum_data",
        "//formula:replace_formulas",
        "//summary:is_positive",
        "//summary:msg_summary_proto_cc",
        "//summary:visualizer",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
    ],
)

magnon_cc_binary(
    name = "step_4_make_msg_tex_file",
    srcs = ["step_4_make_msg_tex_file.cpp"],
    data = ["//data:summary_data"],
    deps = [
        "//config:output_dirs",
        "//summary:is_positive",
        "//summary:msg_summary_proto_cc",
        "//summary:visualizer",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
    ],
)

magnon_py_binary(
    name = "step_5_make_msg_tables",
    srcs = ["step_5_make_msg_tables.py"],
    data = ["//data:summary_data"],
    main = "step_5_make_msg_tables.py",
    deps = [
        "//config:output_dirs_python",
        "//diagnose2:perturbed_band_structure_proto_py",
        "//latexify:latexify_result",
        "//summary:msg_summary_proto_py",
    ],
)

sh_binary(
    name = "steps_2_3_4_5",
    srcs = ["steps_2_3_4_5.sh"],
    data = [
        ":step_2_make_msg_summary",
        ":step_3_make_msg_diagrams",
        ":step_4_make_msg_tex_file",
        ":step_5_make_msg_tables",
    ],
    deps = [
        "@bazel_tools//tools/bash/runfiles",
    ],
)

magnon_cc_binary(
    name = "step_6_compile_msg_figures",
    srcs = ["step_6_compile_msg_figures.cpp"],
    data = ["//data:summary_data"],
    deps = [
        "//config:output_dirs",
        "//summary:is_positive",
        "//summary:msg_summary_proto_cc",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
        "@range-v3",
    ],
)

sh_binary(
    name = "steps_3_6",
    srcs = ["steps_3_6.sh"],
    data = [
        ":step_3_make_msg_diagrams",
        ":step_6_compile_msg_figures",
    ],
    deps = [
        "@bazel_tools//tools/bash/runfiles",
    ],
)
