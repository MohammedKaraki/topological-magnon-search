package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_cc_binary", "magnon_py_binary")

magnon_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//config:read_global_config",
        "@boost//:filesystem",
        "@fmt",
    ],
)

magnon_py_binary(
    name = "main_py",
    srcs = ["main.py"],
    main = "main.py",
    deps = [
        "//fetch:magnetic_space_group_from_generators",
        "//groups:find_subgroups_py",
        "//groups:read_standard_magnetic_space_groups_py",
    ],
)
