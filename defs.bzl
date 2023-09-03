load("//internal:py_binary.bzl", _py_binary = "py_binary")
load("//internal:py_library.bzl", _py_library = "py_library")
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

magnon_py_binary = _py_binary
magnon_py_library = _py_library


_ADDITIONAL_COPTS = [
    "-Werror",
]

def magnon_cc_library(name, copts = [], **kwargs):
    cc_library(
        name = name,
        copts = copts,
        **kwargs,
    )

def magnon_cc_binary(name, copts = [], **kwargs):
    cc_binary(
        name = name,
        copts = copts,
        **kwargs,
    )
