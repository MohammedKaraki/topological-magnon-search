package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_binary", "magnon_py_binary")

magnon_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//common:proto_text_format",
        "//diagnose2:analyze_perturbation",
        "@fmt",
    ],
)

magnon_cc_binary(
    name = "analyze_perturbation",
    srcs = ["analyze_perturbation.cpp"],
    data = [
        "//diagnose2/test_data",
    ],
    deps = [
        "//common:proto_text_format",
        "//diagnose2:analyze_perturbation",
        "//diagnose2:perturbed_band_structure_proto_cc",
        "@fmt",
    ],
)

magnon_py_binary(
    name = "summarize_results",
    srcs = ["summarize_results.py"],
    main = "summarize_results.py",
    deps = [
        "//latexify:latexify_result",
    ],
)

magnon_py_binary(
    name = "process_tables_for_all_materials",
    srcs = ["process_tables_for_all_materials.py"],
    data = [
        "//data:materials",
    ],
    main = "process_tables_for_all_materials.py",
    deps = [
        "//diagnose2:process_tables",
    ],
)

magnon_py_binary(
    name = "process_tables",
    srcs = ["process_tables.py"],
    main = "process_tables.py",
    deps = [
        "//diagnose2:process_tables",
    ],
)
