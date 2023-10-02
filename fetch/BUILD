package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl",
     "magnon_py_library",
     "magnon_py_binary",
     )
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "magnetic_kvector_type_fetcher",
    srcs = ["magnetic_kvector_type_fetcher.py"],
    deps = [
        requirement("numpy"),
        requirement("bs4"),
        requirement("html5lib"),
        "//groups:magnetic_kvector_type_proto_py",
        "//preprocess:cached_requests",
        "//preprocess:utility",
    ],
)

magnon_py_binary(
    name = "test_magnetic_kvector_type_fetcher",
    main = "test_magnetic_kvector_type_fetcher.py",
    # srcs = ["test_magnetic_kvector_type_fetcher.py"],
    deps = [
        ":magnetic_kvector_type_fetcher",
    ],
)
