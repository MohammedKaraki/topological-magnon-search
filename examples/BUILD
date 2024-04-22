package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_binary", "magnon_py_binary")

magnon_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//diagnose2:analyze_perturbation",
        "//utils:proto_text_format",
        "@fmt",
    ],
)

magnon_cc_binary(
    name = "analyze_perturbation_for_material",
    srcs = ["analyze_perturbation_for_material.cpp"],
    deps = [
        "//diagnose2:analyze_perturbation",
        "//utils:proto_text_format",
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
        "//diagnose2:analyze_perturbation",
        "//diagnose2:perturbed_band_structure_proto_cc",
        "//utils:proto_text_format",
        "@fmt",
    ],
)

magnon_py_binary(
    name = "summarize_results_in_sheet",
    srcs = ["summarize_results_in_sheet.py"],
    data = ["//data:materials"],
    main = "summarize_results_in_sheet.py",
    deps = [
        "//diagnose2:perturbed_band_structure_proto_py",
        "//latexify:latexify_result",
    ],
)

magnon_py_binary(
    name = "summarize_result_for_material",
    srcs = ["summarize_result_for_material.py"],
    data = ["//data:material"],
    main = "summarize_result_for_material.py",
    deps = [
        "//diagnose2:perturbed_band_structure_proto_py",
        "//latexify:latexify_result",
    ],
)

magnon_py_binary(
    name = "summarize_results",
    srcs = ["summarize_results.py"],
    data = ["//data:materials"],
    main = "summarize_results.py",
    deps = [
        "//diagnose2:perturbed_band_structure_proto_py",
        "//latexify:latexify_result",
    ],
)

magnon_py_binary(
    name = "process_tables_for_material",
    srcs = ["process_tables_for_material.py"],
    data = [
        "//data:material",
    ],
    main = "process_tables_for_material.py",
    deps = [
        "//diagnose2:process_tables",
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
