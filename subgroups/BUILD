package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
)

magnon_cc_binary(
    name = "find_subgroups",
    srcs = ["find_subgroups.cpp"],
    deps = [
        "@eigen",
        "@fmt",
        "@range-v3",
    ],
)
