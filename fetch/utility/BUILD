package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_py_library",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "scrape_utility",
    srcs = ["scrape_utility.py"],
    deps = [requirement("bs4"),
    ],
)

magnon_py_library(
    name = "band",
    srcs = ["band.py"],
    deps = [
    "//common:logger",
    ]
)

magnon_py_library(
    name = "kvector",
    srcs = ["kvector.py"],
    deps = [
    "//common:logger",
    ]
)

magnon_py_library(
    name = "br",
    srcs = ["br.py"],
    deps = [
    "//common:logger",
        ":band",
    ]
)
