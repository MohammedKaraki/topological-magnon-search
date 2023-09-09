load("//internal:py_binary.bzl", _py_binary = "py_binary")
load("//internal:py_library.bzl", _py_library = "py_library")
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")

def magnon_proto_library(name, srcs, deps = [], **kwargs):
    proto_library(
        name = name,
        srcs = srcs,
        deps = deps,
        **kwargs,
    )
    cc_proto_library(
        name = name + "_cc",
        deps = deps + [":" + name],
        **kwargs,
    )
    py_proto_library(
        name = name + "_py",
        srcs = srcs,
        deps = deps,
        **kwargs,
    )


magnon_py_binary = _py_binary
magnon_py_library = _py_library


_ADDITIONAL_COPTS = [
    "-Werror",
]

def magnon_cc_library(name, copts = [], **kwargs):
    copts = _ADDITIONAL_COPTS + copts
    cc_library(
        name = name,
        copts = copts,
        **kwargs,
    )

def magnon_cc_binary(name, copts = [], **kwargs):
    copts = _ADDITIONAL_COPTS + copts
    cc_binary(
        name = name,
        copts = copts,
        **kwargs,
    )
