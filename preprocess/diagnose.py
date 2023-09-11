from magnon.diagnose.preprocess.all_subgroups import gstrs_and_presc_of_subgroups
from magnon.diagnose.preprocess.lattice_type import find_latticetype
from magnon.diagnose.preprocess.msg import Msg
from magnon.diagnose.preprocess.primvecs import find_primvecsmat
from magnon.diagnose.preprocess.msg_info_table import MSG_INFO_TABLE
from magnon.diagnose.preprocess.identify_group import identify_group
from magnon.diagnose.preprocess.magndata import load_materials, material_doi
from magnon.diagnose.preprocess.magnon_irreps import sxsy_irreps_from_pg
from magnon.diagnose.preprocess.wp import fetch_wp
from magnon.diagnose.preprocess.mbandrep import fetch_wp_point_group_and_br
from collections import defaultdict
from subprocess import check_output
import numpy as np
import json
import re


def make_ksymbol_nicer(k):
    k = k.replace(r"GM", r"\Gamma")
    k = k.replace(r"LA", r"\mathit{LA}")
    k = k.replace(r"VA", r"\mathit{VA}")
    k = k.replace(r"KA", r"\mathit{KA}")
    k = k.replace(r"HA", r"\mathit{HA}")
    k = k.replace(r"NA", r"\mathit{NA}")
    k = k.replace(r"TA", r"\mathit{TA}")
    return k


def siteirrep_and_br(msg_number, wp_label):
    point_group, brs = fetch_wp_point_group_and_br(msg_number, wp_label)
    irrep1, irrep2 = sxsy_irreps_from_pg(point_group)
    siteirrep, br, _, _ = irrep1, brs[irrep1].irreps, irrep2, brs[irrep2].irreps
    br.sort()

    counts = {x: 0 for x in set(br)}
    for x in br:
        counts[x] += 1
    br_new = []
    for x in set(br):
        br_new.append((x.as_str(), counts[x]))
    br_new.sort()
    br_new = [(str(x[1]) if x[1] > 1 else "") + x[0] for x in br_new]
    return siteirrep, br_new


def partition_strlist(l, partsize):
    padsize = (1 + (len(l) - 1) // partsize) * partsize - len(l)
    return np.array(l + [""] * padsize).reshape((-1, partsize)).tolist()


def latexify_formula(f):
    return "{" + f.replace(r"_{", r"$_{").replace(r"}", r"}$") + "}"


def latexify_multirow(cells):
    for cell in cells:
        assert isinstance(cell, list)
    numrows = max(len(cell) for cell in cells)

    allpieces = []
    for i in range(numrows):
        pieces = []
        for cell in cells:
            pieces.append(
                r"\multirow{{{0}}}{{*}}{{{1}}}".format(
                    numrows - len(cell) + 1, cell[i] if i < len(cell) else ""
                )
            )
        allpieces.append(" & ".join(pieces) + r"\\*")
    return "\n".join(allpieces)


def msg_and_wp_to_materials(msg_number, wp):
    result = []

    all_materials = load_materials()
    for mat in all_materials:
        cur_msg_number = mat["msg"]
        wps = mat["wp"]
        if len(wps) > 1:
            continue
        cur_wp = wps[0][0]

        if (msg_number, wp) == (cur_msg_number, cur_wp):
            result.append(mat)

    result.sort(
        key=lambda mat: -1 if "-" in mat["tc"] else float(mat["tc"]), reverse=True
    )
    return result


def msg_and_wp_to_latex_materials(msg_number, wp):
    result = []

    materials = msg_and_wp_to_materials(msg_number, wp)
    for mat in materials:
        result.append(
            r"""{}{}~\cite{{{}}}""".format(
                latexify_formula(mat["formula"]),
                "" if "-" in mat["tc"] else r"""\,({}\,K)""".format(mat["tc"]),
                material_doi(mat),
            )
        )

    return result


def oldmain():
    good_msg_wps = [
        ("11.55", "4a"),
        ("126.386", "4d"),
        ("130.432", "4c"),
        ("134.481", "4d"),
        ("13.74", "4a"),
        ("13.74", "8f"),
        ("138.528", "4b"),
        ("139.535", "8f"),
        ("140.550", "4a"),
        ("140.550", "8g"),
        ("141.555", "8d"),
        ("141.557", "8d"),
        ("14.80", "4a"),
        ("14.80", "8e"),
        ("14.84", "4c"),
        ("15.89", "4c"),
        ("15.89", "4d"),
        ("166.97", "9e"),
        ("167.108", "18e"),
        ("205.33", "4a"),
        ("205.36", "8b"),
        ("227.131", "16d"),
        ("230.148", "24c"),
        ("52.315", "4b"),
        ("60.431", "4a"),
        ("62.441", "4b"),
        ("62.446", "4b"),
        ("62.446", "4c"),
        ("62.447", "4c"),
        ("62.448", "4a"),
        ("62.448", "4b"),
        ("74.558", "4b"),
        ("88.86", "16c"),
    ]
    materials = load_materials()
    x = 0
    msgwp_to_materials = defaultdict(list)
    for mat in materials:
        msg = mat["msg"]
        wps = mat["wp"]
        if len(wps) == 1:
            msgwp_to_materials[(msg, wps[0][0])].append(mat)
    msgwp_materials_list = [[key, val] for key, val in msgwp_to_materials.items()]
    msgwp_materials_list.sort(key=lambda x: len(x[1]), reverse=True)

    output = []
    output.append(
        r"""
\begin{document}
\begin{longtable}{cc}
\caption{caption}\\
\toprule
\multirow{1}{*}{MSG} &  \multirow{2}{*}{Example Materials} \\
\multirow{1}{*}{WP} &\multirow{2}{*}{} \\
\midrule
\endfirsthead
\multicolumn{2}{@{}l}{\ldots continued}\\
\toprule
\multirow{1}{*}{MSG} &  \multirow{2}{*}{Example Materials} \\
\multirow{1}{*}{WP} &\multirow{2}{*}{} \\
\midrule
\endhead
\multicolumn{2}{r}{Continued on next page}
\endfoot
\bottomrule
\endlastfoot"""
    )
    for msgwp_materials in msgwp_materials_list[::]:
        msg, wp = msgwp_materials[0]
        msg_label = Msg(msg).label
        msgwp = ["${0}~({1})$".format(msg_label, msg), "${0}$".format(wp)]
        mats = msgwp_materials[1]
        mats.sort(
            key=lambda x: (-1000 if "-" in x["tc"] else float(x["tc"])), reverse=True
        )
        mats = [
            r"{0}{1}~\cite{{{2}}}".format(
                latexify_formula(mat["formula"]),
                "(" + mat["tc"] + ")" if "-" not in mat["tc"] else "",
                material_doi(mat),
            )
            for mat in mats
        ]
        if (msg, wp) in good_msg_wps:
            output.append(
                latexify_multirow(
                    [msgwp, [" ".join(x) for x in partition_strlist(mats, 2)]]
                )
            )
            output.append(r"\midrule")
    del output[-1]
    output.append(
        r"""
\end{longtable}
\end{document}
"""
    )

    print("\n".join(output))


def read_args():
    import sys

    try:
        _, msg_number, wp = sys.argv
    except:
        raise ValueError("Invalid input arguments.")

    return str(msg_number), str(wp)


def dump_section(
    msg,
    wp,
    pos_dicts,
    neg_dicts,
    all_subgroup_count,
    nontrivialsi_subgroup_count,
    typei_subgroup_count,
):
    section_filename = (
        "/home/mohammed/Dropbox/research/thesis/ch4-sections/{}-{}.tex".format(
            msg.number, wp
        )
    )
    output = []

    wp_coords = [r"\{" + str(x) + r"\}" for x in fetch_wp(msg.number, wp)]
    if len(wp_coords) == 1:
        latex_wp = "\\\\\n".join(wp_coords)
    else:
        # assert len(wp_coords) % 2 == 0, wp_coords
        tmp = partition_strlist(wp_coords, 2)
        tmp = [[x for x in y if x] for y in tmp]
        # assert len(tmp) * 2 == len(wp_coords)
        latex_wp = "\\\\\n".join(r",\qquad ".join(x) for x in tmp)

    output.append(r"""\clearpage""" "\n")
    output.append(
        r"""\section{{Magnetic moments on WP """
        r"""\texorpdfstring{{${0}$}}{{{0}}} of MSG """
        r"""\texorpdfstring{{${1}~({2})$}}{{{2}}}"""
        r"""\label{{sec:{2}-{0}}}}}"""
        "\n".format(wp, msg.label, msg.number)
    )
    output.append(
        r"""\subsection{{Magnon band representation, material examples """
        r"""and result summary"""
        r"""\label{{subsec:summary-{0}-{1}}}}}"""
        "\n".format(msg.number, wp)
    )

    output.append(
        r"""The coordinates and moments of the ${}$ Wyckoff position are
\begin{{gather*}}
    {}
\end{{gather*}}""".format(
            wp, latex_wp
        )
    )

    siteirrep, br = siteirrep_and_br(msg.number, wp)
    for i in range(5, len(br) - 1, 5):
        br[i] = br[i] + r"\\" + "\n" + r"&\quad"

    output.append(
        r"""and the magnon band representation induced from this WP is
\begin{{align*}}
{{({0})}}_{{{1}}}\uparrow {2}~({3})&={4}
\end{{align*}}
""".format(
            siteirrep,
            wp,
            msg.label,
            msg.number,
            make_ksymbol_nicer(r"\oplus ".join(br)),
        )
    )

    lat_mats = msg_and_wp_to_latex_materials(msg.number, wp)
    if len(lat_mats) == 1:
        output.append(
            """We find one magnetic material, {}, in the BCS database
        with magnetic moments which belong to exactly one copy of this
                      WP.\n\n""".format(
                ", ".join(lat_mats)
            )
        )
    else:
        assert len(lat_mats) > 1
        output.append(
            """In the BCS database,
        we find {} magnetic materials with magnetic moments belonging to
                      exactly one copy of this WP: """.format(
                len(lat_mats)
            )
            + ", ".join(lat_mats[:-1])
            + " and "
            + lat_mats[-1]
            + ".\n\n"
        )

    assert all_subgroup_count > 1
    assert nontrivialsi_subgroup_count > 0
    assert typei_subgroup_count <= nontrivialsi_subgroup_count
    if nontrivialsi_subgroup_count == 1:
        output.append(
            r"""Out of the {0} subgroups of ${1}$ that can be reached by
                      external fields and/or mechanical strain, we find that only one
                      subgroup has a nontrivial SI group. As described
                      below, we additionally find that any perturbation leading to this subgroup
                      results in type-I topological magnons.""".format(
                all_subgroup_count,
                msg.label,
            )
        )
    else:
        output.append(
            r"""Out of the {0} subgroups of ${1}$ that can be reached by
                  external fields and/or mechanical strain, we find that {2} subgroups have nontrivial SI
                  groups. Among the latter, we identify {3} {4} that {5}
                  characterized by type-I topological magnons, as described
                  below.""".format(
                all_subgroup_count,
                msg.label,
                nontrivialsi_subgroup_count,
                "one" if typei_subgroup_count == 1 else typei_subgroup_count,
                "subgroup" if typei_subgroup_count == 1 else "subgroups",
                "is" if typei_subgroup_count == 1 else "are",
            )
        )
    for pos_dict in pos_dicts:
        output.append(
            r"""\input{{ch4-subsections/{0}}}"""
            "\n".format(pos_dict["subsection_filename"])
        )

    section_content = "\n".join(output)
    with open(section_filename, "w") as f:
        f.write(section_content)
    return section_content


def input_all_sections():
    materials = load_materials()
    x = 0
    msgwp_to_materials = defaultdict(list)
    for mat in materials:
        msg = mat["msg"]
        wps = mat["wp"]
        if len(wps) == 1:
            msgwp_to_materials[(msg, wps[0][0])].append(mat)
    msgwp_materials_list = [[key, val] for key, val in msgwp_to_materials.items()]
    msgwp_materials_list.sort(key=lambda x: len(x[1]), reverse=True)
    for msgwp_mats in msgwp_materials_list:
        msg_number, wp = msgwp_mats[0]
        print(
            r"""\IfFileExists{{ch4-sections/{0}-{1}.tex}}"""
            r"""{{\input{{ch4-sections/{0}-{1}.tex}}}}{{\ignorespaces}}""".format(
                msg_number, wp
            )
        )


def main():
    msg_number, wp = read_args()

    positive_dicts = []
    negative_dicts = []

    try:
        conffilebase = {
            ("62.448", "4b"): "../cpp/conf-62.448-4b",
            ("205.33", "4a"): "../cpp/conf2",
            ("205.36", "8b"): "../cpp/conf3",
            ("126.386", "4d"): "../cpp/conf4",
            ("140.550", "4a"): "../cpp/conf2",
            ("15.89", "4d"): "../cpp/conf2",
            ("138.528", "4b"): "../cpp/conftwoone",
            ("130.432", "4c"): "../cpp/conftwoone",
            ("74.558", "4b"): "../cpp/conftwoone",
            ("15.89", "4c"): "../cpp/conftwoone",
            ("230.148", "24c"): "../cpp/toohigh",
            ("13.74", "8f"): "../cpp/tophighbottomlow",
            ("13.74", "8f"): "../cpp/tophighbottomlow",
            ("11.55", "4a"): "../cpp/fouroneup_fourtwodown",
            ("128", "4a"): "../cpp/fouroneup_fourtwodown",
        }[(msg_number, wp)]
    except:
        conffilebase = "../cpp/conf1"

    msg = Msg(msg_number)
    all_subgroup_count = 0
    nontrivialsi_subgroup_count = 0
    typei_subgroup_count = 0
    id = 0
    for gstrs, _ in gstrs_and_presc_of_subgroups(msg):
        all_subgroup_count += 1
        identified_number = identify_group(gstrs.split(";"))["group_number"]
        si_str = MSG_INFO_TABLE[identified_number][1]
        if si_str != "(1)":
            nontrivialsi_subgroup_count += 1
            conffileext = ""
            if (msg_number, wp) == ("126.386", "4d"):
                conffileext = "b" if id == 42 else "a"

            print(id, identified_number, si_str)
            # if ((msg_number, wp, id) == ("230.148", "24c", 11)
            #         or (msg_number, wp, id) == ("201.21", "24g", 11)):
            #     print('\033[93m', "WARNING: "
            #           "SKIPPPPPPPPPPED: {} {}'\033[0m'\n".format(msg_number,
            #                                                      wp))
            #     id += 1
            #     continue
            if (msg_number, wp, id) == ("138.528", "4b", 7):
                conffilebase, conffileext = "../cpp/confthin", ""
            print(id, msg_number, wp)
            result = check_output(
                [
                    "../cpp/diagnose.exe",
                    msg_number,
                    wp,
                    str(id),
                    conffilebase + conffileext,
                ]
            ).decode()
            print(result)
            # new_dict = json.loads(result.replace("\\", "\\\\"))
            # if new_dict['type-i']:
            #     typei_subgroup_count += 1
            #     positive_dicts.append(new_dict)
            # else:
            #     negative_dicts.append(new_dict)

        id += 1
    #
    # if len(positive_dicts) > 0:
    #     print(dump_section(msg, wp, positive_dicts, negative_dicts,
    #                        all_subgroup_count,
    #                        nontrivialsi_subgroup_count,
    #                        typei_subgroup_count))


def perturbe_relevant():
    msgwp_to_materials = defaultdict(list)

    materials = load_materials()
    for mat in materials:
        msg_number = mat["msg"]
        wps = mat["wp"]
        if len(wps) == 1:
            msgwp_to_materials[(msg_number, wps[0][0])].append(mat)
    msgwp_materials_list = [[key, val] for key, val in msgwp_to_materials.items()]
    msgwp_materials_list.sort(key=lambda x: len(x[1]), reverse=True)

    for i, (x1, x2) in enumerate(msgwp_materials_list):
        # if i != 276:
        #     continue
        # command = ["python3", "perturb_relevant.py", *x1]
        # print(i, command)
        # result = check_output(command).decode()
        print(
            "python3 diagnose.py ",
            *x1,
            " && #",
            i,
            find_latticetype(x1[0]),
            np.linalg.det(find_primvecsmat(x1[0])),
            x1[1][:-1],
            len(x2)
        )


if __name__ == "__main__":
    # print()
    # main()
    # oldmain()
    perturbe_relevant()
