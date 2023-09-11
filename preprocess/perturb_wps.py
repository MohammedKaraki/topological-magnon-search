from magnon.preprocess.all_subgroups import gstrs_and_presc_of_subgroups
from collections import defaultdict
from magnon.preprocess.msg import Msg
from magnon.preprocess.super_and_sub_msgs import SuperAndSubMsgs
from magnon.preprocess.mbandrep import fetch_wp_point_group_and_br
from magnon.preprocess.magnon_irreps import sxsy_irreps_from_pg
import json
from magnon.preprocess.br import LittleIrrep
from magnon.preprocess.mbandpaths import antiunit_related_irreps


def logfile_path():
    return "/tmp/perturb_wps.log"


def json_output_dir():
    return "/tmp/json_output"


def read_args():
    import sys

    try:
        msg_number = sys.argv[1]
        wp_labels = sorted(sys.argv[2:-1])
        subgroup_id = int(sys.argv[-1])
        if len(wp_labels) == 0:
            raise ValueError("Invalid input arguments.")

    except:
        raise ValueError("Invalid input arguments.")

    return msg_number, wp_labels, subgroup_id


def sxsy_irreps_from_msg_and_wp(msg_number, wp_label):
    point_group, brs = fetch_wp_point_group_and_br(msg_number, wp_label)
    irrep1, irrep2 = sxsy_irreps_from_pg(point_group)
    return [irrep1, irrep2], brs[irrep1].irreps + brs[irrep2].irreps


def posirrep_posbr_negirrep_negbr(msg_number, wp_label):
    point_group, brs = fetch_wp_point_group_and_br(msg_number, wp_label)
    irrep1, irrep2 = sxsy_irreps_from_pg(point_group)
    return irrep1, brs[irrep1].irreps, irrep2, brs[irrep2].irreps


def k1_to_k2_to_irrep_to_lineirreps(msg):
    result = {}

    for kvec1 in msg.kvectors:
        k1 = kvec1.symbol
        assert k1 not in result
        result[k1] = {}

        for kvec2 in msg.kvectors:
            k2 = kvec2.symbol
            assert k2 not in result[k1]

            result[k1][k2] = {
                irrep.label: [
                    "{}({})".format(line_irrep.label, line_irrep.dim)
                    for line_irrep in line_irreps
                ]
                for irrep, line_irreps in msg.common_comp_rels_dict(k1, k2).items()
            }

    return result


def main():
    msg_number, wp_labels, subgroup_id = read_args()

    output = {"wp": "+".join(wp_labels)}

    msg = Msg(msg_number)

    magnon_site_irreps, magnon_band_irreps = [[], []], []
    for wp_label in wp_labels:
        l_magnon_site_irreps, l_magnon_band_irreps = sxsy_irreps_from_msg_and_wp(
            msg_number, wp_label
        )
        magnon_site_irreps[0].append(l_magnon_site_irreps[0])
        magnon_site_irreps[1].append(l_magnon_site_irreps[1])
        magnon_band_irreps.extend(l_magnon_band_irreps)
    magnon_site_irreps[0] = "AND".join(magnon_site_irreps[0])
    magnon_site_irreps[1] = "AND".join(magnon_site_irreps[1])
    magnon_band_irreps.sort()

    output["super_irrep12wp_decomps_of_sxsy"] = msg.decompose_irreps_into_irrep12wps(
        magnon_band_irreps
    )
    output["super_irrep1wp_to_irreps"] = msg.make_irrep1wp_to_irreps()
    output["magnon_site_irreps"] = magnon_site_irreps

    posbrirrep, posbr, negbrirrep, negbr = [], [], [], []
    for wp_label in wp_labels:
        l_posbrirrep, l_posbr, l_negbrirrep, l_negbr = posirrep_posbr_negirrep_negbr(
            msg_number, wp_label
        )
        posbrirrep.append(l_posbrirrep)
        posbr.extend(l_posbr)
        negbrirrep.append(l_negbrirrep)
        negbr.extend(l_negbr)
    posbrirrep = "AND".join(posbrirrep)
    posbr.sort()
    negbrirrep = "AND".join(negbrirrep)
    negbr.sort()

    output["posbrsiteirrep"] = posbrirrep
    output["posbrirreps"] = [x.label for x in posbr]
    output["negbrsiteirrep"] = negbrirrep
    output["negbrirreps"] = [x.label for x in negbr]

    gstrs, presc = gstrs_and_presc_of_subgroups(msg)[subgroup_id]
    msgs = SuperAndSubMsgs(msg, gstrs.split(";"))

    if len(msgs.sub_msg.si_orders) == 0:
        print(
            "SI of subgroup ({}) is trivial. Quitting...".format(msgs.sub_msg.si_orders)
        )
        exit(1)

    subksymbol_to_g_and_superksymbol = {}
    for subkvec in msgs.sub_msg.kvectors:
        gs, superkvec = msgs.subkvec_to_gs_and_superkvec(subkvec)
        g = gs[0]
        assert subkvec.symbol not in subksymbol_to_g_and_superksymbol
        subksymbol_to_g_and_superksymbol[subkvec.symbol] = (str(g), superkvec.symbol)
    output["subk_to_g_and_superk"] = subksymbol_to_g_and_superksymbol

    output["presc"] = presc

    superksymbols_having_maximal_subks = set([])
    for subksymbol, g_and_superksymbol in subksymbol_to_g_and_superksymbol.items():
        superksymbols_having_maximal_subks.add(g_and_superksymbol[1])

    # output["band_super_irreps"] = [x.label
    #                                for x in magnon_irreps_from_msg_and_wp(
    #                                    msg_number, wp_label)
    #                                if x.ksymbol
    #                                in superksymbols_having_maximal_subks
    #                                ]

    output["super_msg_label"] = msgs.super_msg.label
    output["super_msg_number"] = msgs.super_msg.number
    output["sub_msg_label"] = msgs.sub_msg.label
    output["sub_msg_number"] = msgs.sub_msg.number

    output["super_msg_irreps"] = [
        LittleIrrep(x).label for x in msgs.super_msg.irrep_labels
    ]
    output["superirrep_to_dim"] = {
        irrep.label: irrep.dim
        for irrep in [LittleIrrep(x) for x in msgs.super_msg.irrep_labels]
    }
    output["superirrep_to_k"] = {
        irrep.label: irrep.ksymbol
        for irrep in [LittleIrrep(x) for x in msgs.super_msg.irrep_labels]
    }
    output["sub_msg_irreps"] = [LittleIrrep(x).label for x in msgs.sub_msg.irrep_labels]
    output["subirrep_to_dim"] = {
        irrep.label: irrep.dim
        for irrep in [LittleIrrep(x) for x in msgs.sub_msg.irrep_labels]
    }
    output["subirrep_to_k"] = {
        irrep.label: irrep.ksymbol
        for irrep in [LittleIrrep(x) for x in msgs.sub_msg.irrep_labels]
    }

    output["si_orders"] = [int(x) for x in msgs.sub_msg.si_orders]
    output["si_matrix"] = [[int(x) for x in row] for row in msgs.sub_msg.si_matrix]
    output["comp_rels_matrix"] = [
        [int(x) for x in row] for row in msgs.sub_msg.comp_rels
    ]

    output["super_msg_ks"] = [x.symbol for x in msgs.super_msg.kvectors]
    output["super_msg_kcoords"] = [
        ",".join((str(coord) for coord in x.coords)) for x in msgs.super_msg.kvectors
    ]
    output["sub_msg_ks"] = [x.symbol for x in msgs.sub_msg.kvectors]
    output["sub_msg_kcoords"] = [
        ",".join((str(coord) for coord in x.coords)) for x in msgs.sub_msg.kvectors
    ]

    output["super_k1_to_k2_to_irrep_to_lineirreps"] = k1_to_k2_to_irrep_to_lineirreps(
        msgs.super_msg
    )
    output["sub_k1_to_k2_to_irrep_to_lineirreps"] = k1_to_k2_to_irrep_to_lineirreps(
        msgs.sub_msg
    )

    superirrep_to_all_subirreps = defaultdict(list)
    for superkvec, m in msgs.superkvec_to_superirrep_to_subirreps.items():
        for superirrep, superirreps in m.items():
            superirrep_to_all_subirreps[superirrep.label].extend(
                [x.label for x in superirreps]
            )
    output["superirrep_to_all_subirreps"] = superirrep_to_all_subirreps

    output["subk_to_superirrep_to_subirreps"] = {
        subkvec.symbol: {
            superirrep.label: [subirrep.label for subirrep in subirreps]
            for superirrep, subirreps in superirrep_to_subirreps.items()
        }
        for subkvec, superirrep_to_subirreps in msgs.subkvec_to_superirrep_to_subirreps.items()
    }

    output["k1_k2_irrep1irrep2pairs_tuples_of_supermsg"] = antiunit_related_irreps(
        msgs.super_msg.number
    )
    output["k1_k2_irrep1irrep2pairs_tuples_of_submsg"] = antiunit_related_irreps(
        msgs.sub_msg.number
    )

    from fractions import Fraction

    output["super_to_sub"] = list(
        list(str(Fraction(y).limit_denominator(1000)) for y in x)
        for x in msgs.super_to_sub
    )
    print(output["super_to_sub"])
    out_filename = r"{}-{}-{}.json".format(msg_number, "+".join(wp_labels), subgroup_id)

    with open(json_output_dir() + "/" + out_filename, "w") as f:
        json.dump(output, f, indent=4)

    print("Finished successfully. Quitting ...")


if __name__ == "__main__":
    import log

    logger = log.create_root_logger(filename=logfile_path())

    main()
