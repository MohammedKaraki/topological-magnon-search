package(default_visibility = ["//visibility:public"])

load("//:defs.bzl", "magnon_cc_binary", "magnon_py_binary")

magnon_cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    deps = [
        "//config:read_global_config",
        "@fmt",
        "@boost//:filesystem",
    ],
)

magnon_py_binary(
    name = "main_py",
    main = "main.py",
    deps = [
        "//config:config_proto_py",
        "//config:read_global_config",
        "@fmt",
        "@boost//:filesystem",
    ],
)
