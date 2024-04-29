package(default_visibility = ["//visibility:public"])

filegroup(
    name = "msg_info_table",
    srcs = ["msg_number_label_si_genpos.txt"],
)

filegroup(
    name = "lattice_type_table",
    srcs = ["lattice_type_space_group_number.txt"],
)

filegroup(
    name = "standard_magnetic_space_groups",
    srcs = ["standard_magnetic_space_groups.txtpb"],
)

filegroup(
    name = "materials",
    srcs = ["materials.json"],
)

filegroup(
    name = "material",
    srcs = ["material.json"],
)

filegroup(
    name = "summary_data",
    srcs = [":msgs_summary.pb.txt"] + glob(["perturbations/*.pb.txt"]),
)
