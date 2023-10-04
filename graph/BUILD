load(
    "//build:defs.bzl",
    "magnon_cc_binary",
)

magnon_cc_binary(
    name = "compute_connectivities",
    srcs = ["compute_connectivities.cpp"],
    deps = [
        "@boost//:algorithm",
        "@boost//:spirit",
        "@eigen",
        "@fmt",
        "@robin_hood",
    ],
)
