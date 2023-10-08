package(default_visibility = ["//visibility:public"])

load("//build:defs.bzl", "magnon_py_binary")

magnon_py_binary(
    name = "process_standard_msgs_to_proto",
    data = [
        "//data:msg_info_table",
    ],
    main = "process_standard_msgs_to_proto.py",
    deps = [
        "//groups:magnetic_space_group_proto_py",
    ],
)
