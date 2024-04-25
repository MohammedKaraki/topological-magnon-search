package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_library",
    "magnon_cc_test",
    "magnon_proto_library",
    "magnon_py_binary",
    "magnon_py_library",
    "magnon_pybind_library",
)
load("@pip_deps//:requirements.bzl", "requirement")

cc_library(
    name = "comparable",
    hdrs = ["comparable.hpp"],
)

magnon_py_library(
    name = "logger",
    srcs = ["logger.py"],
)

magnon_cc_library(
    name = "proto_text_format",
    srcs = ["proto_text_format.cpp"],
    hdrs = ["proto_text_format.hpp"],
    deps = [
        "@com_google_protobuf//:protobuf",
        "@fmt",
    ],
)

magnon_proto_library(
    name = "complex_number_proto",
    srcs = ["complex_number.proto"],
)

magnon_proto_library(
    name = "matrix_proto",
    srcs = ["matrix.proto"],
    deps = [
        ":complex_number_proto",
    ],
)

magnon_cc_library(
    name = "complex_converter",
    srcs = ["complex_converter.cpp"],
    hdrs = ["complex_converter.hpp"],
    deps = [
        ":complex_number_proto_cc",
    ],
)

magnon_cc_test(
    name = "complex_converter_test",
    srcs = ["complex_converter_test.cpp"],
    deps = [
        ":complex_converter",
        "@gtest//:gtest_main",
    ],
)

magnon_cc_library(
    name = "matrix_converter",
    srcs = ["matrix_converter.cpp"],
    hdrs = ["matrix_converter.hpp"],
    deps = [
        ":complex_converter",
        ":matrix_proto_cc",
        "@eigen",
    ],
)

magnon_cc_test(
    name = "matrix_converter_test",
    srcs = ["matrix_converter_test.cpp"],
    deps = [
        ":matrix_converter",
        "@gtest//:gtest_main",
    ],
)

magnon_pybind_library(
    name = "matrix_converter_py",
    srcs = ["matrix_converter_py.cpp"],
    py_deps = [
        ":matrix_proto_py",
    ],
    deps = [
        ":matrix_converter",
        "@pybind11_protobuf//pybind11_protobuf:native_proto_caster",
    ],
)

magnon_cc_library(
    name = "smith_normal_form",
    srcs = ["smith_normal_form.cpp"],
    hdrs = ["smith_normal_form.hpp"],
    deps = [
        "@eigen",
    ],
)

magnon_cc_test(
    name = "smith_normal_form_test",
    srcs = ["smith_normal_form_test.cpp"],
    deps = [
        ":smith_normal_form",
        "@gtest//:gtest_main",
    ],
)

sh_binary(
    name = "evince_tex",
    srcs = ["evince_tex.sh"],
)
