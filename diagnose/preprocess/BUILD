package(default_visibility=["//visibility:public"])

load("@pip_deps//:requirements.bzl", "requirement")

py_library(
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

py_library(
    name = "snf",
    srcs = ["snf.py"],
    deps = [
        requirement("oauthlib"),
    ]
)

py_library(
    name = "primvecs",
    srcs = ["primvecs.py"],
    data = [
        "//data:latticetype_spacegroupnumber"
    ],
)

py_library(
    name = "all_subgroups",
    srcs = ["all_subgroups.py"],
    deps = [
    ],
)

py_library(
    name = "msg_info_table",
    srcs = ["msg_info_table.py"],
    data = ["//data:msgnumber_label_si_genpos"],
    deps = [
    ],
)

py_library(
    name = "cached_requests",
    srcs = ["cached_requests.py"],
    data = ["//:cache"],
    deps = [
    ],
)

py_library(
    name = "identify_group",
    srcs = ["identify_group.py"],
    deps = [
        requirement("html5lib"),
        ":cached_requests",
    ],
)

py_library(
    name = "mbandrep",
    srcs = ["mbandrep.py"],
    deps = [
        ":br",
        requirement("requests"),
    ],
)

# py_library(
#     name = "all_subgroups",
#     srcs = ["all_subgroups.py"],
#     deps = [
#     ],
# )

py_library(
    name = "coreps",
    srcs = ["coreps.py"],
    deps = [
    ],
)

py_library(
    name = "comp_rels",
    srcs = ["comp_rels.py"],
    deps = [
    ],
)

py_library(
    name = "mkvec",
    srcs = ["mkvec.py"],
    deps = [
    ],
)

py_library(
    name = "char_table",
    srcs = ["char_table.py"],
    deps = [
        ":coreps",
        ":mkvec",
    ],
)

py_library(
    name = "band",
    srcs = ["band.py"],
    deps = [
    ],
)

py_library(
    name = "genpos",
    srcs = ["genpos.py"],
    deps = [
    ],
)

py_library(
    name = "br",
    srcs = ["br.py"],
    deps = [
    ],
)

py_library(
    name = "utility",
    srcs = ["utility.py"],
    deps = [
    ],
)

py_library(
    name = "kvector",
    srcs = ["kvector.py"],
    deps = [
    ],
)

py_library(
    name = "lattice_type",
    srcs = ["lattice_type.py"],
    deps = [
    ],
)

py_library(
    name = "super_and_sub_msgs",
    srcs = ["super_and_sub_msgs.py"],
    deps = [
    ],
)

py_library(
    name = "mbandpaths",
    srcs = ["mbandpaths.py"],
    deps = [
    ],
)

py_binary(
    name = "visual",
    srcs = ["visual.py"],
    data = ["//src/latex:template"],
    deps = [
        ":br",
        requirement("numpy"),
    ],
)

py_library(
    name = "magnon_irreps",
    srcs = ["magnon_irreps.py"],
    deps = [
    ],
)

py_library(
    name = "perturb_wp",
    srcs = ["perturb_wp.py"],
    deps = [
        ":super_and_sub_msgs",
        ":magnon_irreps",
        ":mbandpaths",
    ],
)

py_binary(
    name = "perturb_relevant",
    srcs = ["perturb_relevant.py"],
    data = [
        "//:logs",
    ],
    deps = [
        "@bazel_tools//tools/python/runfiles:runfiles",
        ":all_subgroups",
        ":msg",
        ":perturb_wp",
        # requirement("numpy"),
    ],
)
