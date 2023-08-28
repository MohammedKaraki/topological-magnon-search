import log


def logfile_path():
    from pathlib import Path

    return str((Path(__file__).parent / "../../logs/tests.log").resolve())


logger = log.create_root_logger(filename=logfile_path())


def test_mkvec():
    from mkvec import mkvec_parsed

    ks = mkvec_parsed("15.89")
    for k in ks:
        print(k)


def test_identify_group():
    from identify_group import identify_group

    group_gens = (
        r"""+x,+y,+z,+1
-x,-y,-z,+1
-z,-y,-x,-1
+z,+y,+x,-1
x+1/2,y,z,-1
x,y+1/2,z,-1
x,y,z+1/2,-1"""
    ).split("\n")
    for g in group_gens:
        print(g)
    group = identify_group(group_gens)
    print(group)


def test_wyckoff_brs():
    from mbandrep import fetch_br

    print(fetch_br("143.1", "3d")["A"])


def print_vec(vec):
    print("\t".join(str(x) for x in vec))


def print_mat(mat):
    for v in mat:
        print_vec(v)


def test_coreps():
    import numpy as np
    from coreps import char_table_info
    from genpos import unitary_gstr_to_mat4x4, mat4x4_to_unitary_gstr, UnitaryGenpos

    from fractions import Fraction

    def helper(group_number, ksymbol, to_standard=None):
        irrep_labels, unitary_gs, char_matrix = char_table_info(group_number, ksymbol)

        print(irrep_labels)
        print(unitary_gs)
        if to_standard is not None:
            print(
                [
                    UnitaryGenpos(to_standard @ g.mat4x4 @ np.linalg.inv(to_standard))
                    for g in unitary_gs
                ]
            )
        print(char_matrix)

    # result = coreps("228.139", "GM")
    # result = coreps("228.139", "L")
    # result = coreps("2.4", "R")
    helper("134.481", "X")
    helper(
        "13.72",
        "A",
        np.array(
            [
                [1.0, 0.0, -1.0, 0.0],
                [0.0, 0.0, 1.0, 0.0],
                [0.0, -1.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0],
            ]
        ),
    )
    helper(
        "13.72",
        "B",
        np.array(
            [
                [1.0, 0.0, -1.0, 0.0],
                [0.0, 0.0, 1.0, 0.0],
                [0.0, -1.0, 0.0, 0.0],
                [0.0, 0.0, 0.0, 1.0],
            ]
        ),
    )


def test_Msg():
    from genpos import UnitaryGenpos
    from kvector import KVector
    from msg import Msg
    import numpy as np

    def print_msg(msg):
        print("\n====================\n")
        print(msg.number)
        print(msg.unitary_gs)
        print(msg.kvectors)
        print(msg.contains_unitary_g(UnitaryGenpos.from_gstr("x,y,z,+1")))
        print(msg.contains_unitary_g(UnitaryGenpos.from_gstr("-x,y,-z+1/2,+1")))

    # print_msg(Msg("1.1"))
    # print_msg(Msg("2.4"))
    # print_msg(Msg("143.1"))
    # print_msg(Msg("228.139"))
    # print_msg(Msg("167.103"))
    print_msg(Msg("15.90"))
    print_msg(Msg("12.63"))


def test_all_subgroups():
    from all_subgroups import gstrs_and_presc_of_subgroups
    from msg import Msg
    from super_and_sub_msgs import SuperAndSubMsgs

    # msg = Msg("62.448")
    # msg = Msg("167.103")
    # msg = Msg("62.448")
    msg = Msg("205.33")

    for gstrs, presc in gstrs_and_presc_of_subgroups(msg):
        supersub = SuperAndSubMsgs(msg, gstrs.split(";"))
        # print(supersub.super_msg)
        print(supersub.sub_msg)
        # print(gstrs)
        # print(supersub.super_to_sub)


def test_super_and_sub_msgs():
    from msg import Msg
    from super_and_sub_msgs import SuperAndSubMsgs
    from identify_group import identify_group
    import numpy as np
    from br import LittleIrrep

    #     msg = Msg("167.103")
    #     supersub = SuperAndSubMsgs(super_msg=msg,
    #                            sub_msg_gstrs=
    # "-x+1/3,-y+2/3,-z+2/3,+1;-x+2/3,-y+1/3,-z+1/3,+1;-x+y+1/3,-x+2/3,z+2/3,+1;-x+y+2/3,-x+1/3,z+1/3,+1;-x+y,-x,z,+1;-x,-y,-z,+1;-y+1/3,x-y+2/3,z+2/3,+1;-y+2/3,x-y+1/3,z+1/3,+1;-y,x-y,z,+1;x+1/3,y+2/3,z+2/3,+1;x+2/3,y+1/3,z+1/3,+1;x,y,z,+1;x-y+1/3,x+2/3,-z+2/3,+1;x-y+2/3,x+1/3,-z+1/3,+1;x-y,x,-z,+1;y+1/3,-x+y+2/3,-z+2/3,+1;y+2/3,-x+y+1/3,-z+1/3,+1;y,-x+y,-z,+1".split(';')
    # )
    # msg = Msg("62.448")
    msg = Msg("205.33")
    supersub = SuperAndSubMsgs(
        super_msg=msg,
        # sub_msg_gstrs="-x+1/2,y+1/2,z+1/2,-1;-x,-y,-z,+1;x+1/2,-y+1/2,-z+1/2,-1;x,y,z,+1".split(';')
        sub_msg_gstrs="-x,-y,-z,+1".split(";"),
    )
    # print(supersub.super_msg)
    # print(supersub.super_msg.kvectors)
    # print(supersub.sub_msg)
    # print(supersub.sub_msg.kvectors)
    # print(supersub.super_to_sub)

    for subkvec in supersub.subkvec_to_superirrep_to_subirreps:
        print(subkvec)
        for superirrep in supersub.subkvec_to_superirrep_to_subirreps[subkvec]:
            print(superirrep)
            print(supersub.subkvec_to_superirrep_to_subirreps[subkvec][superirrep])

    print()
    for superkvec in supersub.superkvec_to_superirrep_to_subirreps:
        print(superkvec)
        for superirrep in supersub.superkvec_to_superirrep_to_subirreps[superkvec]:
            print(superirrep)
            print(supersub.superkvec_to_superirrep_to_subirreps[superkvec][superirrep])


def test_irrep_decomp():
    from msg import Msg
    from super_and_sub_msgs import SuperAndSubMsgs
    from kvector import KVector

    msg = Msg("134.481")
    # msg = Msg("2.4")
    supersub = SuperAndSubMsgs(super_msg=msg, sub_msg_gstrs="-x,-y,-z,+1".split(";"))

    subkvec_to_superirrep_to_subirreps = {}

    for decomp in supersub._irrep_decomp(KVector("R:(1/2,1/2,1/2)")):
        print(decomp)


def test_irrep_tr_partners():
    from kvector import KVector
    from msg import Msg

    # msg = Msg("228.134")
    # msg = Msg("162.77")
    msg = Msg("167.103")

    print(msg.tr_partners)
    # print(msg.tr_partners)

    # for wyckoff in msg.ebrs:
    #     print(wyckoff)
    #     for wyckoffirrep in msg.ebrs[wyckoff]:
    #         print(wyckoff, " - ", wyckoffirrep, ":")
    #         print(msg.ebrs[wyckoff][wyckoffirrep])
    #
    #     print()

    # print(msg.get_br('1a', 'T'))
    # for ksymbol, irreps in msg.get_br('1a', 'T').as_ksymbol_dict().items():
    #     print(ksymbol, irreps)


def test_comp_rels():
    from msg import Msg

    msg2 = Msg("129.419")
    for lhs, rhs in msg2.common_comp_rels_dict("GM", "Z").items():
        print(lhs, rhs)


def test_sis():
    from msg import Msg

    msg = Msg("39.199")
    print(msg.si_orders)
    print(msg.si_matrix)
    print(msg.irrep_labels)
    (msg.comp_rels)
    print(msg.symmetry_indicator)
    print(msg.bs_basis)

    # for bs in msg.bs_basis.T:
    #     print([" + ".join([label]*count)
    #            for count, label in zip(bs, msg.irrep_labels)
    #            if count != 0])


def test_diagnose():
    from band import Band
    from msg import Msg
    from itertools import accumulate
    from random import shuffle

    msg = Msg("62.441")

    # all_brs = [
    #     msg.get_br(wp.label, wpirrep.label)
    #     for wp in msg.ebrs for wpirrep in msg.ebrs[wp]
    #     ]
    #
    # band1 = sum(all_brs[0:3]).as_band()
    # band2 = sum(all_brs[3:5]).as_band()
    # band3 = sum(all_brs[1:5]).as_band()
    # band1.shuffle(1)
    # band2.shuffle(8)
    # band3.shuffle(8)
    # band = band1 + band2 + band3

    # band = sum([msg.get_br('4b', 'A_{g}')]).as_band()
    # subbands = band.sub_bands
    # for subband in subbands:
    #     print(subband)
    #     print(msg.band_si(subband), msg.band_compatibility(subband))

    print(msg.band_bs(band))

    print(msg.band_si(band), msg.band_compatibility(band))

    for subband in msg.find_isolated_subbands(band):
        print(subband)

    print(msg.all_gstrs)


def test_antiunitary_gs():
    from msg import Msg
    from genpos import (
        is_antiunitary_gstr,
        antiunitary_gstr_to_mat4x4,
        unitary_gstr_to_mat4x4,
    )
    import numpy as np

    msg = Msg("12.62")
    antiunitary_mat3x3s = []
    for gstr in msg.all_gstrs:
        if is_antiunitary_gstr(gstr):
            antiunitary_mat3x3s.append(
                antiunitary_gstr_to_mat4x4(gstr).astype(float)[:3, :3]
            )

    def kcoords_to_primkcoords(kcoords):
        return np.mod(msg.primvecsmat.T @ kcoords.astype(float), 1.0)

    def antiunitary_star(given_kvec):
        result = {given_kvec}

        for kvec in msg.kvectors:
            for antiunitary_mat3x3 in antiunitary_mat3x3s:
                antig_times_given_kvec_coords = (
                    (-1)
                    * np.linalg.inv(antiunitary_mat3x3.T)
                    @ given_kvec.coords.astype(float)
                )

                if np.allclose(
                    kcoords_to_primkcoords(antig_times_given_kvec_coords),
                    kcoords_to_primkcoords(kvec.coords),
                ):
                    result.add(kvec)
                    break

        return result

    for kvec in msg.kvectors:
        print(kvec, antiunitary_star(kvec))


def test_complex_ebrs():
    from msg import Msg
    import numpy as np

    msg = Msg("167.103")
    wp_and_irrep_to_paired_vec = msg.make_wp_and_irrep_to_paired_vec()
    all_vecs = list(wp_and_irrep_to_paired_vec.values())
    print(msg.decompose_vec_into_complex_ebr_titles(all_vecs[1] + all_vecs[2]))


def main():
    # test_mkvec()
    # test_identify_group()
    # test_wyckoff_brs()
    # test_coreps()
    # test_Msg()
    # test_super_and_sub_msgs()
    # test_all_subgroups()
    # test_irrep_decomp()
    # test_comp_rels()
    # test_irrep_tr_partners()
    test_sis()
    # test_diagnose()
    # test_complex_ebrs()

    # test_antiunitary_gs()


if __name__ == "__main__":
    main()
