package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_py_binary", "magnon_py_library")

filegroup(
    name = "latex_templates",
    srcs = [
        "document_template.tex",
        "longtable_template.tex",
    ],
)

magnon_py_library(
    name = "latexify_result",
    srcs = ["latexify_result.py"],
    data = [":latex_templates"],
    deps = [
        "//diagnose2:search_result_proto_py",
    ],
)
