package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_py_library",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "cached_requests",
    srcs = ["cached_requests.py"],
    deps = [
        "//utils:logger",
        "//config:read_global_config_python",
        requirement("requests"),
    ],
)

magnon_py_library(
    name = "scrape_utility",
    srcs = ["scrape_utility.py"],
    deps = [
        requirement("bs4"),
    ],
)

magnon_py_library(
    name = "band",
    srcs = ["band.py"],
    deps = [
        "//utils:logger",
    ],
)

magnon_py_library(
    name = "kvector",
    srcs = ["kvector.py"],
    deps = [
        "//utils:logger",
        requirement("numpy"),
    ],
)

magnon_py_library(
    name = "br",
    srcs = ["br.py"],
    deps = [
        ":band",
        "//utils:logger",
    ],
)

magnon_py_library(
    name = "general_position_utility",
    srcs = ["general_position_utility.py"],
    deps = [
        requirement("numpy"),
    ],
)
