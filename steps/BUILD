package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_proto_library",
    "magnon_py_binary",
    "magnon_py_library",
)


magnon_py_binary(
    name = "step_1_create_incomplete_msgs_summary",
    main = "step_1_create_incomplete_msgs_summary.py",
    data = ["//data:materials.json"],
    deps = [
        "//config:output_dirs_python",
        "//summary:msg_summary_proto_py",
    ],
)

