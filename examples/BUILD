package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_binary", "magnon_py_binary")

magnon_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//common:proto_text_format",
        "//diagnose2:analyze_perturbed_band_structure",
        "@fmt",
    ],
)

magnon_py_binary(
    name = "main_py",
    srcs = ["main.py"],
    data = [
        "//data:materials",
    ],
    main = "main.py",
    deps = [
        "//diagnose2:process_tables",
    ],
)
