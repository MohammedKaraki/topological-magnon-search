import sys
import json
from collections import defaultdict

from magnon.diagnose2.perturbed_band_structure_pb2 import PerturbedBandStructure
from magnon.common.matrix_converter_py import matrixxi_to_proto, matrix4d_to_proto

from magnon.fetch.magnetic_space_group_from_generators import fetch_msg_from_generators
from magnon.fetch.magnetic_band_representation import (
    fetch_atomic_band_representations_for_wyckoff_position,
)


from magnon.fetch.antiunitarily_related_irreps import fetch_antiunitarily_related_irreps
from magnon.fetch.compatibility_relations import fetch_compatibility_relations


def antiunit_related_irreps(msg_number):
    # only temporary until refactoring is complete
    # only temporary until refactoring is complete
    # only temporary until refactoring is complete
    from magnon.fetch.antiunitarily_related_irreps import _antiunit_related_irreps_impl

    return _antiunit_related_irreps_impl(msg_number)


from magnon.diagnose2.utility.magnetic_space_group import Msg
from magnon.diagnose2.utility.group_subgroup_relation import GroupSubgroupRelation
from magnon.fetch.utility.br import LittleIrrep
from magnon.diagnose2.utility.s_plus_irrep import s_plus_irrep_for_point_group


from magnon.groups.find_subgroups_py import find_subgroups
from magnon.groups.read_standard_magnetic_space_groups_py import (
    read_standard_msgs_from_disk,
)


def gstrs_and_presc_of_subgroups(msg_number):
    # only temporary until refactoring is complete
    # only temporary until refactoring is complete
    # only temporary until refactoring is complete
    standard_msgs = read_standard_msgs_from_disk()
    subgroups_info = find_subgroups(msg_number, standard_msgs)

    def get_gstrs(subgroup_info):
        return [
            gp.coordinates_form
            for gp in subgroup_info.unbroken_standard_general_positions.general_position
        ]

    def get_prescs(subgroup_info):
        return [p for p in subgroup_info.perturbation_prescription]

    return [(get_gstrs(sgi), get_prescs(sgi)) for sgi in subgroups_info]


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


def process_tables(msg_number, wp_label, subgroup_id):
    result = PerturbedBandStructure()

    subgroup_id = int(subgroup_id)
    output = {"wp": wp_label}

    def copy_to_proto():
        result.atomic_orbital.add()
        result.atomic_orbital[0].wyckoff_position.label = wp_label

    copy_to_proto()
    del copy_to_proto

    msg = Msg(msg_number)

    brs_at_wp = fetch_atomic_band_representations_for_wyckoff_position(
        msg_number, wp_label
    )
    point_group_label = brs_at_wp[
        0
    ].atomic_orbital.wyckoff_position.site_symmetry_group_label
    s_plus_irrep = s_plus_irrep_for_point_group(point_group_label)

    magnon_br_filtered = [
        br
        for br in brs_at_wp
        if br.atomic_orbital.site_symmetry_irrep.label == s_plus_irrep
    ]
    assert len(magnon_br_filtered) == 1
    magnon_br = magnon_br_filtered[0]

    output["magnon_site_irreps"] = [s_plus_irrep, "LEFT_EMPTY"]
    result.atomic_orbital[0].site_symmetry_irrep.label = s_plus_irrep
    output["posbrsiteirrep"] = s_plus_irrep
    output["posbrirreps"] = [irrep.label for irrep in magnon_br.kspace_little_irrep]

    def copy_to_proto():
        for irrep in magnon_br.kspace_little_irrep:
            result.unperturbed_band_structure.supergroup_little_irrep.add(
                label=irrep.label, dimension=irrep.dimension
            )

    copy_to_proto()
    del copy_to_proto

    gstrs, presc = gstrs_and_presc_of_subgroups(msg_number)[subgroup_id]
    msgs = GroupSubgroupRelation(msg, gstrs)

    if len(msgs.sub_msg.si_orders) == 0:
        print(
            "SI of subgroup ({}) is trivial. Quitting...".format(
                msgs.sub_msg.si_orders
            ),
            file=sys.stderr,
        )
        exit(1)

    subksymbol_to_g_and_superksymbol = {}
    for subkvec in msgs.sub_msg.kvectors:
        gs, superkvec = msgs.subkvec_to_gs_and_superkvec(subkvec)
        g = gs[0]
        assert subkvec.symbol not in subksymbol_to_g_and_superksymbol
        subksymbol_to_g_and_superksymbol[subkvec.symbol] = (str(g), superkvec.symbol)
    output["subk_to_g_and_superk"] = subksymbol_to_g_and_superksymbol

    output["presc"] = ";".join(presc)

    def copy_to_proto():
        for p in presc:
            result.group_subgroup_relation.perturbation_prescription.append(p)

    copy_to_proto()
    del copy_to_proto

    superksymbols_having_maximal_subks = set([])
    for subksymbol, g_and_superksymbol in subksymbol_to_g_and_superksymbol.items():
        superksymbols_having_maximal_subks.add(g_and_superksymbol[1])

    output["super_msg_label"] = msgs.super_msg.label
    output["super_msg_number"] = msgs.super_msg.number
    output["sub_msg_label"] = msgs.sub_msg.label
    output["sub_msg_number"] = msgs.sub_msg.number

    def copy_to_proto():
        result.supergroup.number = msgs.super_msg.number
        result.supergroup.label = msgs.super_msg.label
        result.subgroup.number = msgs.sub_msg.number
        result.subgroup.label = msgs.sub_msg.label

    copy_to_proto()
    del copy_to_proto

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

    def copy_to_proto():
        result.subgroup.symmetry_indicator_matrix.CopyFrom(
            matrixxi_to_proto(output["si_matrix"])
        )
        result.subgroup.compatibility_relations_matrix.CopyFrom(
            matrixxi_to_proto(output["comp_rels_matrix"])
        )
        for si_order in output["si_orders"]:
            result.subgroup.symmetry_indicator_order.append(si_order)

    copy_to_proto()
    del copy_to_proto

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

    def copy_to_proto():
        for subkvec in msgs.sub_msg.kvectors:
            for cr in fetch_compatibility_relations(
                msgs.sub_msg.number, subkvec.symbol
            ):
                result.subgroup.compatibility_relation.append(cr)

        for superkvec in msgs.super_msg.kvectors:
            for cr in fetch_compatibility_relations(
                msgs.super_msg.number, superkvec.symbol
            ):
                result.supergroup.compatibility_relation.append(cr)

    copy_to_proto()
    del copy_to_proto

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

    def copy_to_proto():
        map_superksymbol_to_map_subksymbol_to_pair_g_and_map_superirrep_to_subirreps = (
            {}
        )
        map0 = (
            map_superksymbol_to_map_subksymbol_to_pair_g_and_map_superirrep_to_subirreps
        )

        for (
            subkvec,
            superirrep_to_subirreps,
        ) in msgs.subkvec_to_superirrep_to_subirreps.items():
            g, superksymbol = subksymbol_to_g_and_superksymbol[subkvec.symbol]
            if superksymbol not in map0:
                map0[superksymbol] = {}
            map0[superksymbol][subkvec.symbol] = (g, superirrep_to_subirreps)

        for superksymbol, map1 in map0.items():
            irrep_relation = result.group_subgroup_relation.irrep_relation.add()
            irrep_relation.supergroup_kvector.star.label = superksymbol
            for subksymbol, (g, map2) in map1.items():
                subgroup_kvector_decomps = (
                    irrep_relation.subgroup_kvector_decompositions.add()
                )
                subgroup_kvector_decomps.subgroup_kvector.star.label = subksymbol
                subgroup_kvector_decomps.action_on_supergroup_kvector.seitz_form = g
                for superirrep, subirreps in map2.items():
                    decomp = subgroup_kvector_decomps.decomposition.add()
                    decomp.supergroup_irrep.label = superirrep.label
                    decomp.supergroup_irrep.dimension = superirrep.dim
                    for subirrep in subirreps:
                        subgroup_irrep = decomp.subgroup_irrep.add()
                        subgroup_irrep.label = superirrep.label
                        subgroup_irrep.dimension = superirrep.dim

    copy_to_proto()
    del copy_to_proto

    output["k1_k2_irrep1irrep2pairs_tuples_of_supermsg"] = antiunit_related_irreps(
        msgs.super_msg.number
    )
    output["k1_k2_irrep1irrep2pairs_tuples_of_submsg"] = antiunit_related_irreps(
        msgs.sub_msg.number
    )

    def copy_to_proto():
        result.supergroup.antiunitarily_related_irrep_pairs.CopyFrom(
            fetch_antiunitarily_related_irreps(msgs.super_msg.number)
        )
        result.subgroup.antiunitarily_related_irrep_pairs.CopyFrom(
            fetch_antiunitarily_related_irreps(msgs.sub_msg.number)
        )

    copy_to_proto()
    del copy_to_proto

    from fractions import Fraction

    output["super_to_sub"] = list(
        list(str(Fraction(y).limit_denominator(1000)) for y in x)
        for x in msgs.super_to_sub
    )
    print(output["super_to_sub"], file=sys.stderr)

    def copy_to_proto():
        result.group_subgroup_relation.supergroup_from_subgroup_standard_basis.CopyFrom(
            matrix4d_to_proto(output["super_to_sub"])
        )

    copy_to_proto()
    del copy_to_proto

    out_filename = r"{}-{}-{}.json".format(msg_number, wp_label, subgroup_id)

    print("Finished successfully. Quitting ...", file=sys.stderr)

    return result
