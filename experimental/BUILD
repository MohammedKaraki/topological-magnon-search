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
        "//config:config_proto_py",
        "//config:read_global_config_python",
    ],
)

magnon_cc_binary(
    name = "graph",
    srcs = ["graph.cpp"],
    deps = [
        "@boost//:graph",
        "@fmt",
    ],
)
