load("//:defs.bzl", "magnon_cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "compare",
    hdrs = ["compare.hpp"],
)

magnon_cc_library(
    name = "proto",
    hdrs = ["proto.hpp"],
    srcs = ["proto.cpp"],
    deps = [
        "@com_google_protobuf//:protobuf_lite",
    ]
)
