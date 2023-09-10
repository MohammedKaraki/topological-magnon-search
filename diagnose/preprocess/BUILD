package(default_visibility=["//visibility:public"])

load("//build:defs.bzl",
     "magnon_py_library",
     "magnon_py_binary",
     "magnon_py_test",
     "magnon_cc_library",
     "magnon_cc_binary",
     )
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
        ":log",
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
        ":log",
        requirement("oauthlib"),
        requirement("wolframclient"),
    ]
)

magnon_py_library(
    name = "primvecs",
    srcs = ["primvecs.py"],
    data = [
        ":log",
        "//data:latticetype_spacegroupnumber"
    ],
)

magnon_py_library(
    name = "all_subgroups",
    srcs = ["all_subgroups.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "msg_info_table",
    srcs = ["msg_info_table.py"],
    data = ["//data:msgnumber_label_si_genpos"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "cached_requests",
    srcs = ["cached_requests.py"],
    data = ["//:cache"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "identify_group",
    srcs = ["identify_group.py"],
    deps = [
        ":log",
        ":cached_requests",
        requirement("html5lib"),
    ],
)

magnon_py_library(
    name = "mbandrep",
    srcs = ["mbandrep.py"],
    deps = [
        ":br",
        ":log",
        requirement("requests"),
        requirement("bs4"),
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
        ":log",
    ],
)

magnon_py_library(
    name = "comp_rels",
    srcs = ["comp_rels.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "mkvec",
    srcs = ["mkvec.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "char_table",
    srcs = ["char_table.py"],
    deps = [
        ":coreps",
        ":mkvec",
        ":log",
        requirement("joblib"),
    ],
)

magnon_py_library(
    name = "band",
    srcs = ["band.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "genpos",
    srcs = ["genpos.py"],
    deps = [
        ":log",
        ":cached_requests",
        ":utility",
        requirement("requests"),
        requirement("numpy"),
        requirement("bs4"),
    ],
)

magnon_py_library(
    name = "br",
    srcs = ["br.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "utility",
    srcs = ["utility.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "kvector",
    srcs = ["kvector.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "log",
    srcs = ["log.py"],
    deps = [
    ],
)

magnon_py_library(
    name = "lattice_type",
    srcs = ["lattice_type.py"],
    deps = [
        ":log"
    ],
)

magnon_py_library(
    name = "super_and_sub_msgs",
    srcs = ["super_and_sub_msgs.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "mbandpaths",
    srcs = ["mbandpaths.py"],
    deps = [
        ":log",
    ],
)

magnon_py_binary(
    name = "visual",
    main = "visual.py",
    data = ["//src/latex:template"],
    deps = [
        ":br",
        ":log",
        requirement("numpy"),
    ],
)

magnon_py_library(
    name = "magnon_irreps",
    srcs = ["magnon_irreps.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "perturb_wp",
    srcs = ["perturb_wp.py"],
    deps = [
        ":super_and_sub_msgs",
        ":magnon_irreps",
        ":mbandpaths",
        ":log",
    ],
)

magnon_py_library(
    name = "wp",
    srcs = ["wp.py"],
    deps = [
        ":log",
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
        ":log",
        ":all_subgroups",
        ":msg",
        ":perturb_wp",
        # requirement("numpy"),
    ],
)

magnon_py_test(
    name = "perturb_relevant_test",
    srcs = ["perturb_relevant_test.py"],
    deps = [
        ":perturb_relevant",
    ],
)

magnon_py_test(
    name = "genpos_test",
    srcs = ["genpos_test.py"],
    deps = [
        ":genpos",
    ],
)
