load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

_ADDITIONAL_COPTS = [
    # -Werror is placed here to limit its effect to our targets only.
    # This is because the code of some external libraries we have cause
    # clang to emit warnings, and it is preferable to just ignore them.
    "-Werror",
]

def magnon_proto_library(name, srcs, deps = [], **kwargs):
    proto_library(
        name = name,
        srcs = srcs,
        deps = deps,
        **kwargs
    )

    cc_proto_library(
        name = name + "_cc",
        deps = [":" + name],
        **kwargs
    )

    py_proto_deps = [dep + "_py" for dep in deps]
    py_proto_library(
        name = name + "_py",
        srcs = srcs,
        deps = py_proto_deps,
        **kwargs
    )

def magnon_cc_library(name, copts = [], **kwargs):
    copts = _ADDITIONAL_COPTS + copts
    cc_library(
        name = name,
        copts = copts,
        **kwargs
    )

def magnon_cc_binary(name, copts = [], **kwargs):
    copts = _ADDITIONAL_COPTS + copts
    cc_binary(
        name = name,
        copts = copts,
        **kwargs
    )

def magnon_py_library(**kwargs):
    py_library(**kwargs)

def magnon_py_binary(name, main, srcs = [], **kwargs):
    if main not in srcs:
        srcs = [main] + srcs

    py_binary(
        name = name,
        main = main,
        srcs = srcs,
        **kwargs
    )

def magnon_pybind_library(name, srcs, data = [], deps = [], py_deps = [], **kwargs):
    pybind_extension(
        name = name,
        srcs = srcs,
        deps = deps,
        **kwargs
    )

    magnon_py_library(
        name = name,
        data = [":" + name + ".so"] + data,
        deps = py_deps,
        **kwargs
    )

def magnon_py_test(name, srcs, **kwargs):
    py_test(
        name = name,
        srcs = srcs,
        **kwargs
    )

def _concatenate_srcs_impl(ctx):
    pass

concatenate_srcs = rule(
    implementation = _concatentate_srcs_impl,
    attrs = {
        "files": attr.label_list(allow_files = True),
        "out": attr.output(mandatory = True),
    },
)
