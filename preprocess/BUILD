package(default_visibility = ["//visibility:public"])

load(
    "//build:defs.bzl",
    "magnon_cc_binary",
    "magnon_cc_library",
    "magnon_py_binary",
    "magnon_py_library",
    "magnon_py_test",
)
load("@pip_deps//:requirements.bzl", "requirement")

magnon_py_binary(
    name = "analyze_relevant_subgroups",
    main = "analyze_relevant_subgroups.py",
    deps = [
        ":all_subgroups",
        ":br",
        ":log",
        ":magnon_irreps",
        ":mbandpaths",
        ":mbandrep",
        ":msg",
        ":perturb_wp",
        ":super_and_sub_msgs",
    ],
)

magnon_py_library(
    name = "msg",
    srcs = ["msg.py"],
    data = ["//subgroups:find_subgroups"],
    deps = [
        ":band",
        ":br",
        ":char_table",
        ":comp_rels",
        ":genpos",
        ":kvector",
        ":lattice_type",
        ":log",
        ":mbandrep",
        ":msg_info_table",
        ":primvecs",
        ":snf",
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
        requirement("joblib"),
    ],
)

magnon_py_library(
    name = "primvecs",
    srcs = ["primvecs.py"],
    data = [
        ":log",
        "//data:latticetype_spacegroupnumber",
    ],
    deps = [
        ":genpos",
        ":lattice_type",
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
    deps = [
        ":log",
        requirement("requests"),
    ],
)

magnon_py_library(
    name = "identify_group",
    srcs = ["identify_group.py"],
    deps = [
        ":cached_requests",
        ":genpos",
        ":log",
        ":utility",
        requirement("html5lib"),
    ],
)

magnon_py_library(
    name = "mbandrep",
    srcs = ["mbandrep.py"],
    deps = [
        ":br",
        ":cached_requests",
        ":kvector",
        ":log",
        ":utility",
        requirement("requests"),
        requirement("bs4"),
    ],
)

magnon_py_library(
    name = "coreps",
    srcs = ["coreps.py"],
    deps = [
        ":cached_requests",
        ":genpos",
        ":log",
        ":mkvec",
        ":utility",
        requirement("bs4"),
        requirement("joblib"),
    ],
)

magnon_py_library(
    name = "comp_rels",
    srcs = ["comp_rels.py"],
    deps = [
        ":br",
        ":cached_requests",
        ":log",
        requirement("bs4"),
    ],
)

magnon_py_library(
    name = "mkvec",
    srcs = ["mkvec.py"],
    deps = [
        ":cached_requests",
        ":log",
        ":utility",
        requirement("bs4"),
        requirement("numpy"),
    ],
)

magnon_py_library(
    name = "char_table",
    srcs = ["char_table.py"],
    deps = [
        ":coreps",
        ":genpos",
        ":log",
        ":mkvec",
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
        ":cached_requests",
        ":log",
        ":utility",
        requirement("numpy"),
        requirement("bs4"),
    ],
)

magnon_py_library(
    name = "br",
    srcs = ["br.py"],
    deps = [
        ":band",
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
        requirement("numpy"),
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
        ":log",
    ],
)

magnon_py_library(
    name = "super_and_sub_msgs",
    srcs = ["super_and_sub_msgs.py"],
    deps = [
        ":br",
        ":genpos",
        ":identify_group",
        ":log",
        ":msg",
    ],
)

magnon_py_library(
    name = "mbandpaths",
    srcs = ["mbandpaths.py"],
    deps = [
        ":br",
        ":cached_requests",
        ":kvector",
        ":log",
        ":utility",
        requirement("bs4"),
    ],
)

magnon_py_binary(
    name = "visual",
    data = ["//template:latex"],
    main = "visual.py",
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
        ":all_subgroups",
        ":br",
        ":log",
        ":magnon_irreps",
        ":mbandpaths",
        ":mbandrep",
        ":msg",
        ":super_and_sub_msgs",
    ],
)

magnon_py_library(
    name = "wp",
    srcs = ["wp.py"],
    deps = [
        ":cached_requests",
        ":log",
        ":utility",
        requirement("bs4"),
        requirement("numpy"),
    ],
)

magnon_py_binary(
    name = "perturb_relevant",
    main = "perturb_relevant.py",
    deps = [
        ":all_subgroups",
        ":br",
        ":log",
        ":magnon_irreps",
        ":mbandpaths",
        ":mbandrep",
        ":msg",
        ":perturb_wp",
        ":super_and_sub_msgs",
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

magnon_py_test(
    name = "all_subgroups_test",
    srcs = ["all_subgroups_test.py"],
    deps = [
        ":all_subgroups",
    ],
)

magnon_py_test(
    name = "band_test",
    srcs = ["band_test.py"],
    deps = [
        ":band",
    ],
)

magnon_py_test(
    name = "br_test",
    srcs = ["br_test.py"],
    deps = [
        ":br",
    ],
)

magnon_py_test(
    name = "cached_requests_test",
    srcs = ["cached_requests_test.py"],
    deps = [
        ":cached_requests",
    ],
)

magnon_py_test(
    name = "char_table_test",
    srcs = ["char_table_test.py"],
    deps = [
        ":char_table",
    ],
)

magnon_py_test(
    name = "comp_rels_test",
    srcs = ["comp_rels_test.py"],
    deps = [
        ":comp_rels",
    ],
)

magnon_py_test(
    name = "coreps_test",
    srcs = ["coreps_test.py"],
    deps = [
        ":coreps",
    ],
)

magnon_py_test(
    name = "diagnose_test",
    srcs = ["diagnose_test.py"],
    deps = [
        ":diagnose",
    ],
)

magnon_py_test(
    name = "doi2bib_test",
    srcs = ["doi2bib_test.py"],
    deps = [
        ":doi2bib",
    ],
)

magnon_py_test(
    name = "identify_group_test",
    srcs = ["identify_group_test.py"],
    deps = [
        ":identify_group",
    ],
)

magnon_py_test(
    name = "kpoint_test",
    srcs = ["kpoint_test.py"],
    deps = [
        ":kpoint",
    ],
)

magnon_py_test(
    name = "kvector_test",
    srcs = ["kvector_test.py"],
    deps = [
        ":kvector",
    ],
)

magnon_py_test(
    name = "lattice_type_test",
    srcs = ["lattice_type_test.py"],
    deps = [
        ":lattice_type",
    ],
)

magnon_py_test(
    name = "log_test",
    srcs = ["log_test.py"],
    deps = [
        ":log",
    ],
)

magnon_py_test(
    name = "magndata_test",
    srcs = ["magndata_test.py"],
    deps = [
        ":magndata",
    ],
)

magnon_py_test(
    name = "magnon_irreps_test",
    srcs = ["magnon_irreps_test.py"],
    deps = [
        ":magnon_irreps",
    ],
)

magnon_py_test(
    name = "main_test",
    srcs = ["main_test.py"],
    deps = [
        ":main",
    ],
)

magnon_py_test(
    name = "mapk_test",
    srcs = ["mapk_test.py"],
    deps = [
        ":mapk",
    ],
)

magnon_py_test(
    name = "mathml2latex_test",
    srcs = ["mathml2latex_test.py"],
    deps = [
        ":mathml2latex",
    ],
)

magnon_py_test(
    name = "mbandpaths_test",
    srcs = ["mbandpaths_test.py"],
    deps = [
        ":mbandpaths",
    ],
)

magnon_py_test(
    name = "mbandrep_test",
    srcs = ["mbandrep_test.py"],
    deps = [
        ":mbandrep",
    ],
)

magnon_py_test(
    name = "mkvec_test",
    srcs = ["mkvec_test.py"],
    deps = [
        ":mkvec",
    ],
)

magnon_py_test(
    name = "msg_info_table_test",
    srcs = ["msg_info_table_test.py"],
    deps = [
        ":msg_info_table",
    ],
)

magnon_py_test(
    name = "msg_test",
    srcs = ["msg_test.py"],
    deps = [
        ":msg",
    ],
)

magnon_py_test(
    name = "perturb_wp_test",
    srcs = ["perturb_wp_test.py"],
    deps = [
        ":perturb_wp",
    ],
)

magnon_py_test(
    name = "perturb_wps_test",
    srcs = ["perturb_wps_test.py"],
    deps = [
        ":perturb_wps",
    ],
)

magnon_py_test(
    name = "primvecs_test",
    srcs = ["primvecs_test.py"],
    deps = [
        ":primvecs",
    ],
)

magnon_py_test(
    name = "snf_test",
    srcs = ["snf_test.py"],
    deps = [
        ":snf",
    ],
)

magnon_py_test(
    name = "summarize_bands_test",
    srcs = ["summarize_bands_test.py"],
    deps = [
        ":summarize_bands",
    ],
)

magnon_py_test(
    name = "super_and_sub_msgs_test",
    srcs = ["super_and_sub_msgs_test.py"],
    deps = [
        ":super_and_sub_msgs",
    ],
)

magnon_py_test(
    name = "tests_test",
    srcs = ["tests_test.py"],
    deps = [
        ":tests",
    ],
)

magnon_py_test(
    name = "utility_test",
    srcs = ["utility_test.py"],
    deps = [
        ":utility",
    ],
)

magnon_py_test(
    name = "visual_test",
    srcs = ["visual_test.py"],
    deps = [
        ":visual",
    ],
)

magnon_py_test(
    name = "wp_test",
    srcs = ["wp_test.py"],
    deps = [
        ":wp",
    ],
)

magnon_py_library(
    name = "diagnose",
    srcs = ["diagnose.py"],
    deps = [
        ":all_subgroups",
        ":identify_group",
        ":lattice_type",
        ":log",
        ":magndata",
        ":magnon_irreps",
        ":mbandrep",
        ":msg",
        ":msg_info_table",
        ":primvecs",
        ":wp",
    ],
)

magnon_py_library(
    name = "kpoint",
    srcs = ["kpoint.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "magndata",
    srcs = ["magndata.py"],
    deps = [
        ":cached_requests",
        ":log",
        ":utility",
        requirement("bs4"),
    ],
)

magnon_py_library(
    name = "mapk",
    srcs = ["mapk.py"],
    deps = [
        ":genpos",
        ":lattice_type",
        ":log",
        ":mbandrep",
        ":mkvec",
    ],
)

magnon_py_library(
    name = "doi2bib",
    srcs = ["doi2bib.py"],
    deps = [
        ":cached_requests",
        ":log",
        ":magndata",
    ],
)

magnon_py_library(
    name = "main",
    srcs = ["main.py"],
    deps = [
        ":all_subgroups",
        ":identify_group",
        ":log",
        ":msg",
        ":msg_info_table",
    ],
)

magnon_py_library(
    name = "mathml2latex",
    srcs = ["mathml2latex.py"],
    deps = [
        ":log",
    ],
)

magnon_py_library(
    name = "perturb_wps",
    srcs = ["perturb_wps.py"],
    deps = [
        ":all_subgroups",
        ":br",
        ":log",
        ":magnon_irreps",
        ":mbandpaths",
        ":mbandrep",
        ":msg",
        ":super_and_sub_msgs",
    ],
)

magnon_py_library(
    name = "summarize_bands",
    srcs = ["summarize_bands.py"],
    deps = [
        ":log",
        ":magndata",
        ":msg",
        ":primvecs",
    ],
)

magnon_py_library(
    name = "tests",
    srcs = ["tests.py"],
    deps = [
        ":log",
    ],
)
