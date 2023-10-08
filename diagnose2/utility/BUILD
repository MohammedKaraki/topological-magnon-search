package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_py_library",
    "magnon_py_test",
)

magnon_py_library(
    name = "s_plus_irrep",
    srcs = ["s_plus_irrep.py"],
)

magnon_py_test(
    name = "s_plus_irrep_test",
    srcs = ["s_plus_irrep_test.py"],
    deps = [":s_plus_irrep"],
)
