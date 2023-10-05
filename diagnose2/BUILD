package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_proto_library",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_proto_library(
    name = "perturbed_band_structure_proto",
    srcs = ["perturbed_band_structure.proto"],
    deps = [
        "//common:matrix_proto",
        "//groups:induced_magnetic_space_group_proto",
        "//groups:magnetic_space_group_proto",
    ],
)
