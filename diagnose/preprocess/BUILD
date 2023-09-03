package(default_visibility=["//visibility:public"])

load("//:defs.bzl", "magnon_py_library", "magnon_py_binary", "magnon_cc_library", "magnon_cc_binary")
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_library(
    name = "msg",
    srcs = ["msg.py"], data = ["//diagnose:find_subgroups"],
    deps = [
        ":genpos",
        ":band",
        ":comp_rels",
        ":char_table",
        ":lattice_type",
        ":utility",
        ":kvector",
        ":snf",
        ":primvecs",
        ":msg_info_table",
        ":identify_group",
        ":mbandrep",
        requirement("numpy"),
    ],
)

magnon_py_library(
    name = "snf",
    srcs = ["snf.py"],
    deps = [
        requirement("oauthlib"),
    ]
)

magnon_py_library(
    name = "primvecs",
    srcs = ["primvecs.py"],
    data = [
        "//data:latticetype_spacegroupnumber"
    ],
)

magnon_py_library(
    name = "all_subgroups",
    srcs = ["all_subgroups.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "msg_info_table",
    srcs = ["msg_info_table.py"],
    data = ["//data:msgnumber_label_si_genpos"],
    deps = [
    ],
)

magnon_py_library(
    name = "cached_requests",
    srcs = ["cached_requests.py"],
    data = ["//:cache"],
    deps = [
    ],
)

magnon_py_library(
    name = "identify_group",
    srcs = ["identify_group.py"],
    deps = [
        requirement("html5lib"),
        ":cached_requests",
    ],
)

magnon_py_library(
    name = "mbandrep",
    srcs = ["mbandrep.py"],
    deps = [
        ":br",
        requirement("requests"),
    ],
)

# magnon_py_library(
#     name = "all_subgroups",
#     srcs = ["all_subgroups.py"],
#     deps = [
#     ],
# )

magnon_py_library(
    name = "coreps",
    srcs = ["coreps.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "comp_rels",
    srcs = ["comp_rels.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "mkvec",
    srcs = ["mkvec.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "char_table",
    srcs = ["char_table.py"],
    deps = [
        ":coreps",
        ":mkvec",
    ],
)

magnon_py_library(
    name = "band",
    srcs = ["band.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "genpos",
    srcs = ["genpos.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "br",
    srcs = ["br.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "utility",
    srcs = ["utility.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "kvector",
    srcs = ["kvector.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "lattice_type",
    srcs = ["lattice_type.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "super_and_sub_msgs",
    srcs = ["super_and_sub_msgs.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "mbandpaths",
    srcs = ["mbandpaths.py"],
    deps = [
    ],
)

magnon_py_binary(
    name = "visual",
    main = "visual.py",
    data = ["//src/latex:template"],
    deps = [
        ":br",
        requirement("numpy"),
    ],
)

magnon_py_library(
    name = "magnon_irreps",
    srcs = ["magnon_irreps.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "perturb_wp",
    srcs = ["perturb_wp.py"],
    deps = [
        ":super_and_sub_msgs",
        ":magnon_irreps",
        ":mbandpaths",
    ],
)

magnon_py_library(
    name = "wp",
    srcs = ["wp.py"],
    deps = [
        ":cached_requests",
    ],
)

magnon_py_binary(
    name = "perturb_relevant",
    main = "perturb_relevant.py",
    data = [
        "//:logs",
    ],
    deps = [
        ":all_subgroups",
        ":msg",
        ":perturb_wp",
        # requirement("numpy"),
    ],
)
