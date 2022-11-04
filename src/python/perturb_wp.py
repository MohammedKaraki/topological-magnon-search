from all_subgroups import gstrs_and_presc_of_subgroups
from collections import defaultdict
from msg import Msg
from super_and_sub_msgs import SuperAndSubMsgs
from mbandrep import fetch_wp_point_group_and_br
from magnon_irreps import magnon_irreps_from_pg
import json
from br import LittleIrrep


def logfile_path():
    from pathlib import Path
    return str((Path(__file__).parent / "../../logs/tests.log").resolve())


def read_args():
    import sys

    try:
        _, msg_number, wp_label, subgroup_id = sys.argv
        subgroup_id = int(subgroup_id)
    except:
        raise ValueError("Invalid input arguments.")

    return msg_number, wp_label, subgroup_id


def magnon_irreps_from_msg_and_wp(msg_number, wp_label):
    point_group, brs = fetch_wp_point_group_and_br(msg_number, wp_label)
    irrep1, irrep2 = magnon_irreps_from_pg(point_group)
    logger.error("Magnon irreps obtained only "
                   "from half of particle-hole bands")
    return brs[irrep1].irreps


def k1_to_k2_to_irrep_to_lineirreps(msg):
    result = {}

    for kvec1 in msg.kvectors:
        k1 = kvec1.symbol
        assert k1 not in  result
        result[k1] = {}

        for kvec2 in msg.kvectors:
            if kvec2 == kvec1:
                continue

            k2 = kvec2.symbol
            assert k2 not in result[k1]

            result[k1][k2] = {irrep.label:
                              [line_irrep.label for line_irrep in line_irreps]
                              for irrep, line_irreps
                              in msg.common_comp_rels_dict(k1, k2).items()
                              }

    return result


def main():
    msg_number, wp_label, subgroup_id = read_args()

    output = {}

    msg = Msg(msg_number)

    gstrs, presc = gstrs_and_presc_of_subgroups(msg)[subgroup_id]
    msgs = SuperAndSubMsgs(msg, gstrs.split(';'))
    output["band_super_irreps"] = [x.label
                                         for x in magnon_irreps_from_msg_and_wp(
                                             msg_number, wp_label)
                                         ]
    output["super_msg_label"] = msgs.super_msg.label
    output["super_msg_number"] = msgs.super_msg.number
    output["sub_msg_label"] = msgs.sub_msg.label
    output["sub_msg_number"] = msgs.sub_msg.number

    output["super_msg_irreps"] = [LittleIrrep(x).label
                                  for x in msgs.super_msg.irrep_labels]
    output["superirrep_to_dim"] = {
        irrep.label: irrep.dim
        for irrep in [LittleIrrep(x) for x in msgs.super_msg.irrep_labels]
        }
    output["sub_msg_irreps"] = [LittleIrrep(x).label
                                for x in msgs.sub_msg.irrep_labels]
    output["subirrep_to_dim"] = {
        irrep.label: irrep.dim
        for irrep in [LittleIrrep(x) for x in msgs.sub_msg.irrep_labels]
        }

    output["si_orders"] = [int(x) for x in msgs.sub_msg.si_orders]
    output["si_matrix"] = [[int(x) for x in row]
                                   for row in msgs.sub_msg.si_matrix]
    output["comp_rels_matrix"] = [[int(x) for x in row]
                                  for row in msgs.sub_msg.comp_rels]

    output["super_msg_ks"] = [x.symbol
                                    for x in msgs.super_msg.kvectors]
    output["sub_msg_ks"] = [x.symbol
                                  for x in msgs.sub_msg.kvectors]
    subksymbol_to_g_and_superksymbol = {}
    for subkvec in msgs.sub_msg.kvectors:
        gs, superkvec = msgs.subkvec_to_gs_and_superkvec(subkvec)
        g = gs[0]
        assert subkvec.symbol not in subksymbol_to_g_and_superksymbol
        subksymbol_to_g_and_superksymbol[subkvec.symbol] = (str(g),
                                                            superkvec.symbol)
    output["subk_to_g_and_superk"] = subksymbol_to_g_and_superksymbol

    output["super_k1_to_k2_to_irrep_to_lineirreps"] = \
        k1_to_k2_to_irrep_to_lineirreps(msgs.super_msg)
    output["sub_k1_to_k2_to_irrep_to_lineirreps"] = \
        k1_to_k2_to_irrep_to_lineirreps(msgs.sub_msg)

    superirrep_to_all_subirreps = defaultdict(list)
    for superkvec, m in msgs.superkvec_to_superirrep_to_subirreps.items():
        for superirrep, superirreps in m.items():
            superirrep_to_all_subirreps[superirrep.label].extend(
                [x.label for x in superirreps]
                )
    output["superirrep_to_all_subirreps"] = superirrep_to_all_subirreps


    output["subk_to_superirrep_to_subirreps"] = \
        {subkvec.symbol: {superirrep.label: [subirrep.label
                                             for subirrep
                                             in subirreps
                                             ]
                          for superirrep, subirreps
                          in superirrep_to_subirreps.items()
                          }
         for subkvec, superirrep_to_subirreps
         in msgs.subkvec_to_superirrep_to_subirreps.items()
         }
    print(json.dumps(output))


if __name__ == "__main__":
    import log
    logger = log.create_root_logger(filename=logfile_path())

    main()
