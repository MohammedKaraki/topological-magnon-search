package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_py_library",
    "magnon_py_test",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "magnetic_kvector_type",
    srcs = ["magnetic_kvector_type.py"],
    deps = [
        requirement("numpy"),
        requirement("bs4"),
        requirement("html5lib"),
        "//fetch/utility:cached_requests",
        "//fetch/utility:scrape_utility",
        "//groups:magnetic_kvector_type_proto_py",
    ],
)

magnon_py_test(
    name = "magnetic_kvector_type_test",
    srcs = ["magnetic_kvector_type_test.py"],
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
        "//fetch/utility:br",
        "//fetch/utility:cached_requests",
        "//fetch/utility:kvector",
        "//fetch/utility:scrape_utility",
        "//groups:magnetic_space_group_proto_py",
    ],
)

magnon_py_test(
    name = "magnetic_band_representation_test",
    srcs = ["magnetic_band_representation_test.py"],
    deps = [
        ":magnetic_band_representation",
    ],
)

magnon_py_library(
    name = "magnetic_space_group_from_generators",
    srcs = ["magnetic_space_group_from_generators.py"],
    deps = [
        "//common:matrix_converter_py",
        "//fetch/utility:cached_requests",
        "//fetch/utility:general_position_utility",
        "//fetch/utility:scrape_utility",
        "//groups:magnetic_space_group_proto_py",
        requirement("bs4"),
        requirement("html5lib"),
        requirement("numpy"),
    ],
)

magnon_py_test(
    name = "magnetic_space_group_from_generators_test",
    srcs = ["magnetic_space_group_from_generators_test.py"],
    deps = [
        ":magnetic_space_group_from_generators",
        requirement("numpy"),
        "//common:matrix_converter_py",
    ],
)
