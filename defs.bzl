load("//internal:py_binary.bzl", "py_binary")
load("//internal:py_library.bzl", "py_library")
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

magnon_py_binary = py_binary
magnon_py_library = py_library


_ADDITIONAL_COPTS = [
    "-Werror",
]

def magnon_cc_library(**kwargs):
    name = kwargs["name"]
    kwargs.pop('name')
    copts = _ADDITIONAL_COPTS + kwargs.get('copts', [])
    if 'copts' in kwargs:
        kwargs.pop('copts')

    cc_library(
        name = name,
        copts = copts,
        **kwargs,
    )

def magnon_cc_binary(**kwargs):
    name = kwargs["name"]
    kwargs.pop('name')
    copts = _ADDITIONAL_COPTS + kwargs.get('copts', [])
    if 'copts' in kwargs:
        kwargs.pop('copts')

    cc_binary(
        name = name,
        copts = copts,
        **kwargs,
    )
