package(default_visibility = ["//visibility:public"])

load("@pip_deps//:requirements.bzl", "requirement")
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

magnon_py_library(
    name = "magnetic_space_group",
    srcs = ["magnetic_space_group.py"],
    deps = [
        ":msg_info_table",
        "//diagnose2/utility:character_table",
        "//diagnose2/utility:lattice_type",
        "//diagnose2/utility:primitive_vectors",
        "//diagnose2/utility:smith_normal_form",
        "//fetch:compatibility_relations",
        "//fetch:general_positions",
        "//fetch:magnetic_band_representation",
        "//fetch/utility:general_position_utility",
    ],
)

magnon_py_test(
    name = "magnetic_space_group_test",
    srcs = ["magnetic_space_group_test.py"],
    deps = [":magnetic_space_group"],
)

magnon_py_library(
    name = "msg_info_table",
    srcs = ["msg_info_table.py"],
    data = [
        "//data:msg_number_label_si_genpos.txt",
    ],
    deps = [
    ],
)

magnon_py_test(
    name = "msg_info_table_test",
    srcs = ["msg_info_table_test.py"],
    deps = [":msg_info_table"],
)

magnon_py_library(
    name = "smith_normal_form",
    srcs = ["smith_normal_form.py"],
    deps = [
        requirement("oauthlib"),
        requirement("wolframclient"),
        requirement("joblib"),
    ],
)

magnon_py_test(
    name = "smith_normal_form_test",
    srcs = ["smith_normal_form_test.py"],
    deps = [":smith_normal_form"],
)

magnon_py_library(
    name = "character_table",
    srcs = ["character_table.py"],
    deps = [
        "//fetch:character_table",
        "//fetch/utility:general_position_utility",
    ],
)

magnon_py_test(
    name = "character_table_test",
    srcs = ["character_table_test.py"],
    deps = [":character_table"],
)

magnon_py_library(
    name = "primitive_vectors",
    srcs = ["primitive_vectors.py"],
    deps = [
        ":lattice_type",
        "//fetch:general_positions",
        "//fetch/utility:general_position_utility",
        requirement("numpy"),
    ],
)

magnon_py_test(
    name = "primitive_vectors_test",
    srcs = ["primitive_vectors_test.py"],
    deps = [":primitive_vectors"],
)

magnon_py_library(
    name = "lattice_type",
    srcs = ["lattice_type.py"],
    data = [
        "//data:lattice_type_table",
    ],
    deps = [
    ],
)

magnon_py_test(
    name = "lattice_type_test",
    srcs = ["lattice_type_test.py"],
    deps = [":lattice_type"],
)

magnon_py_library(
    name = "group_subgroup_relation",
    srcs = ["group_subgroup_relation.py"],
    deps = [
        "//utils:logger",
        "//diagnose2/utility:magnetic_space_group",
        "//fetch:magnetic_space_group_from_generators",
        "//fetch/utility:br",
        "//fetch/utility:general_position_utility",
    ],
)

magnon_py_test(
    name = "group_subgroup_relation_test",
    srcs = ["group_subgroup_relation_test.py"],
    deps = [":group_subgroup_relation"],
)
