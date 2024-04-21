package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_py_library",
    "magnon_py_test",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "kvector_stars",
    srcs = ["kvector_stars.py"],
    deps = [
        requirement("numpy"),
        requirement("bs4"),
        requirement("html5lib"),
        "//fetch/utility:cached_requests",
        "//fetch/utility:scrape_utility",
        "//groups:kvector_proto_py",
    ],
)

magnon_py_test(
    name = "kvector_stars_test",
    srcs = ["kvector_stars_test.py"],
    deps = [
        ":kvector_stars",
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
        "//groups:atomic_orbital_proto_py",
        "//groups:irrep_proto_py",
        "//groups:magnetic_space_group_proto_py",
        "//groups:wyckoff_position_proto_py",
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
        "//utils:matrix_converter_py",
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
        "//utils:matrix_converter_py",
    ],
)

magnon_py_library(
    name = "character_table",
    srcs = ["character_table.py"],
    deps = [
        "//utils:matrix_converter_py",
        "//fetch/utility:cached_requests",
        "//fetch/utility:general_position_utility",
        "//fetch/utility:scrape_utility",
        "//groups:character_table_proto_py",
        "//groups:irrep_proto_py",
        requirement("joblib"),
        requirement("bs4"),
        requirement("html5lib"),
    ],
)

magnon_py_test(
    name = "character_table_test",
    srcs = ["character_table_test.py"],
    deps = [
        ":character_table",
        "//utils:matrix_converter_py",
    ],
)

magnon_py_library(
    name = "compatibility_relations",
    srcs = ["compatibility_relations.py"],
    deps = [
        "//fetch/utility:br",
        "//fetch/utility:cached_requests",
        "//groups:compatibility_relations_proto_py",
        "//groups:irrep_proto_py",
        requirement("bs4"),
        requirement("html5lib"),
    ],
)

magnon_py_test(
    name = "compatibility_relations_test",
    srcs = ["compatibility_relations_test.py"],
    deps = [
        ":compatibility_relations",
    ],
)

magnon_py_library(
    name = "antiunitarily_related_irreps",
    srcs = ["antiunitarily_related_irreps.py"],
    deps = [
        "//fetch/utility:br",
        "//fetch/utility:cached_requests",
        "//fetch/utility:kvector",
        "//fetch/utility:scrape_utility",
        "//groups:magnetic_space_group_proto_py",
        requirement("bs4"),
        requirement("html5lib"),
    ],
)

magnon_py_test(
    name = "antiunitarily_related_irreps_test",
    srcs = ["antiunitarily_related_irreps_test.py"],
    deps = [
        ":antiunitarily_related_irreps",
    ],
)

magnon_py_library(
    name = "general_positions",
    srcs = ["general_positions.py"],
    deps = [
        "//fetch/utility:cached_requests",
        "//fetch/utility:general_position_utility",
        "//fetch/utility:scrape_utility",
        "//groups:magnetic_space_group_proto_py",
        requirement("numpy"),
        requirement("bs4"),
        requirement("html5lib"),
    ],
)

magnon_py_test(
    name = "general_positions_test",
    srcs = ["general_positions_test.py"],
    deps = [
        ":general_positions",
    ],
)

magnon_py_library(
    name = "wyckoff_positions",
    srcs = ["wyckoff_positions.py"],
    deps = [
        "//fetch/utility:cached_requests",
        "//fetch/utility:scrape_utility",
        "//groups:magnetic_space_group_proto_py",
        requirement("bs4"),
        requirement("html5lib"),
    ],
)

magnon_py_test(
    name = "wyckoff_positions_test",
    srcs = ["wyckoff_positions_test.py"],
    deps = [
        ":wyckoff_positions",
    ],
)
