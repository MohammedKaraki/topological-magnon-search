package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_py_binary",
    "magnon_py_library",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "magnetic_kvector_type",
    srcs = ["magnetic_kvector_type.py"],
    deps = [
        requirement("numpy"),
        requirement("bs4"),
        requirement("html5lib"),
        "//groups:magnetic_kvector_type_proto_py",
        "//fetch/utility:cached_requests",
        "//fetch/utility:scrape_utility",
    ],
)

magnon_py_binary(
    name = "test_magnetic_kvector_type",
    main = "test_magnetic_kvector_type.py",
    deps = [
        ":magnetic_kvector_type",
    ],
)

magnon_py_library(
    name = "magnetic_band_representation",
    srcs = ["magnetic_band_representation.py"],
    deps = [
        requirement("numpy"),
        requirement("bs4"),
        requirement("html5lib"),
        "//fetch/utility:cached_requests",
        "//fetch/utility:scrape_utility",
        "//fetch/utility:kvector",
        "//fetch/utility:br",
    ],
)

magnon_py_binary(
    name = "test_magnetic_band_representation",
    main = "test_magnetic_band_representation.py",
    deps = [
        ":magnetic_band_representation",
    ],
)

