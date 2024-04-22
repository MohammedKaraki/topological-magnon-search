import sys
import json
from collections import defaultdict
from fractions import Fraction

from magnon.diagnose2.perturbed_band_structure_pb2 import (
    PerturbedBandStructures,
    PerturbedBandStructure,
)
from magnon.utils.matrix_converter_py import (
    matrixxi_from_proto,
    matrixxi_to_proto,
    matrix4d_to_proto,
)

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


def _process_tables_for_subgroup(
    msg_number, wp_labels, subgroup_gstrs, presc, is_trivial_subgroup
):
    result = PerturbedBandStructure()
    assert isinstance(wp_labels, list)
    assert isinstance(wp_labels[0], str)
    msg = Msg(msg_number)

    msgs = GroupSubgroupRelation(msg, subgroup_gstrs)

    def copy_to_proto():
        result.supergroup.number = msgs.super_msg.number
        result.supergroup.label = msgs.super_msg.label
        result.subgroup.number = msgs.sub_msg.number
        result.subgroup.label = msgs.sub_msg.label

    copy_to_proto()
    del copy_to_proto

    def copy_to_proto():
        result.group_subgroup_relation.supergroup_from_subgroup_standard_basis.CopyFrom(
            matrix4d_to_proto(
                list(
                    list(str(float(Fraction(y).limit_denominator(1000))) for y in x)
                    for x in msgs.super_to_sub
                )
            )
        )

    copy_to_proto()
    del copy_to_proto

    if is_trivial_subgroup:
        result.subgroup.is_trivial_symmetry_indicator_group = True
        return result

    def copy_to_proto():
        result.subgroup.symmetry_indicator_matrix.CopyFrom(
            matrixxi_to_proto([[int(x) for x in row] for row in msgs.sub_msg.si_matrix])
        )
        result.subgroup.compatibility_relations_matrix.CopyFrom(
            matrixxi_to_proto([[int(x) for x in row] for row in msgs.sub_msg.comp_rels])
        )
        for si_order in msgs.sub_msg.si_orders:
            result.subgroup.symmetry_indicator_order.append(int(si_order))

        for index, irrep_str in enumerate(msgs.sub_msg.irrep_labels):
            label = LittleIrrep(irrep_str).label
            result.subgroup.irrep_label_to_matrix_column_index[label] = index

    copy_to_proto()
    del copy_to_proto

    for wp_label in wp_labels:
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

        def copy_to_proto():
            orbital = result.unperturbed_band_structure.atomic_orbital.add()
            orbital.wyckoff_position.label = wp_label
            orbital.wyckoff_position.site_symmetry_group_label = point_group_label

            for irrep in magnon_br.kspace_little_irrep:
                result.unperturbed_band_structure.supergroup_little_irrep.add(
                    label=irrep.label, dimension=irrep.dimension
                )

        copy_to_proto()
        del copy_to_proto

    subksymbol_to_g_and_superksymbol = {}
    for subkvec in msgs.sub_msg.kvectors:
        gs, superkvec = msgs.subkvec_to_gs_and_superkvec(subkvec)
        g = gs[0]
        assert subkvec.symbol not in subksymbol_to_g_and_superksymbol
        subksymbol_to_g_and_superksymbol[subkvec.symbol] = (str(g), superkvec.symbol)

    def copy_to_proto():
        for p in presc:
            result.group_subgroup_relation.perturbation_prescription.append(p)

    copy_to_proto()
    del copy_to_proto

    def copy_to_proto():
        def convert_to_proto(irrep_labels, group_proto):
            for x in irrep_labels:
                irrep = LittleIrrep(x)
                irrep_proto = group_proto.little_irrep.add()
                irrep_proto.label = irrep.label
                irrep_proto.dimension = irrep.dim
                irrep_proto.kstar.label = irrep.ksymbol

        convert_to_proto(msgs.super_msg.irrep_labels, result.supergroup)
        convert_to_proto(msgs.sub_msg.irrep_labels, result.subgroup)

    copy_to_proto()
    del copy_to_proto

    def copy_to_proto():
        def process_kvectors(group, group_proto):
            for kvec in group.kvectors:
                kvec_coords = ",".join((str(coord) for coord in kvec.coords))
                kvector_proto = group_proto.kvector.add()
                kvector_proto.star.label = kvec.symbol
                kvector_proto.coordinates = kvec_coords

                for cr in fetch_compatibility_relations(group.number, kvec.symbol):
                    group_proto.compatibility_relation.append(cr)

        process_kvectors(msgs.sub_msg, result.subgroup)
        process_kvectors(msgs.super_msg, result.supergroup)

    copy_to_proto()
    del copy_to_proto

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
                        subgroup_irrep.label = subirrep.label
                        subgroup_irrep.dimension = subirrep.dim

    copy_to_proto()
    del copy_to_proto

    def copy_to_proto():
        result.supergroup.antiunitarily_related_irrep_pairs.CopyFrom(
            fetch_antiunitarily_related_irreps(msgs.super_msg.number)
        )
        result.subgroup.antiunitarily_related_irrep_pairs.CopyFrom(
            fetch_antiunitarily_related_irreps(msgs.sub_msg.number)
        )

    copy_to_proto()
    del copy_to_proto

    return result


def process_tables(msg_number, wp_labels, debug_index=None):
    if debug_index is not None:
        return [
            _process_tables_for_subgroup(
                msg_number,
                wp_labels,
                *gstrs_and_presc_of_subgroups(msg_number)[debug_index],
                False
            )
        ]

    structures = PerturbedBandStructures()
    print(msg_number, wp_labels)
    for i, (gstrs, presc) in enumerate(gstrs_and_presc_of_subgroups(msg_number)):
        identified_number = fetch_msg_from_generators(gstrs).number
        subgroup = Msg(identified_number)
        print("Subgroup ", subgroup.label, " ... ", end="")
        is_trivial_subgroup = len(subgroup.si_orders) == 0
        structures.structure.append(
            _process_tables_for_subgroup(
                msg_number, wp_labels, gstrs, presc, is_trivial_subgroup
            )
        )

        def format_mat(m):
            def format_vec(v):
                return "{{ {} }}".format(", ".join(str(x) for x in v))

            return "{" + ",\n".join([format_vec(v) for v in m]) + "}"

        if is_trivial_subgroup:
            print(i + 1, "trivial")
        else:
            print(i + 1, "<<<<<<viable>>>>>>>!")
            # print()
            # print(format_mat(matrixxi_from_proto(structures.structure[-1].subgroup.symmetry_indicator_matrix)))
            # print()
            # print(format_mat(matrixxi_from_proto(structures.structure[-1].subgroup.compatibility_relations_matrix)))
            # print("{ " + ", ".join(['"' + li.label + '"' for li in
            #                         structures.structure[-1].subgroup.little_irrep]) + " }")

    return structures
