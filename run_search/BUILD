package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_binary", "magnon_py_binary")

magnon_py_binary(
    name = "create_perturbations_data",
    srcs = ["create_perturbations_data.py"],
    main = "create_perturbations_data.py",
    deps = [
        "//diagnose2:process_tables",
    ],
)

magnon_cc_binary(
    name = "search_perturbations_data",
    srcs = ["search_perturbations_data.cpp"],
    deps = [
        "//diagnose2:analyze_perturbation",
        "//utils:proto_text_format",
        "@boost//:program_options",
        "@fmt",
    ],
)
