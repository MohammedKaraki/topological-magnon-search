import log

def logfile_path():
    from pathlib import Path
    return str((Path(__file__).parent / "../../logs/tests.log").resolve())

logger = log.create_root_logger(filename=logfile_path())


def test_mkvec():
    from mkvec import mkvec_parsed
    ks = mkvec_parsed('143.1')
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
        ).split('\n')
    for g in group_gens:
        print(g)
    group = identify_group(group_gens)
    print(group)


def test_wyckoff_brs():
    from mbandrep import fetch_wyckoff_br
    print(fetch_wyckoff_br('143.1', '3d', 'A'))



def print_vec(vec):
    print('\t'.join(str(x) for x in vec))


def print_mat(mat):
    for v in mat:
        print_vec(v)


def test_coreps():
    import numpy as np
    from coreps import char_table_info
    from genpos import \
        unitary_gstr_to_mat4x4, mat4x4_to_unitary_gstr, UnitaryGenpos

    from fractions import Fraction


    def helper(group_number, ksymbol, to_standard=None):
        irrep_labels, unitary_gs, char_matrix = \
            char_table_info(group_number, ksymbol)

        print(irrep_labels)
        print(unitary_gs)
        if to_standard is not None:
            print([UnitaryGenpos(
                to_standard @ g.mat4x4 @ np.linalg.inv(to_standard))
                   for g in unitary_gs])
        print(char_matrix)


    # result = coreps("228.139", "GM")
    # result = coreps("228.139", "L")
    # result = coreps("2.4", "R")
    helper("134.481", "X")
    helper("13.72", "A", np.array(
        [
            [ 1.,  0., -1.,  0.],
            [ 0.,  0.,  1.,  0.],
            [ 0., -1.,  0.,  0.],
            [ 0.,  0.,  0.,  1.],
            ])
           )
    helper("13.72", "B", np.array(
        [
            [ 1.,  0., -1.,  0.],
            [ 0.,  0.,  1.,  0.],
            [ 0., -1.,  0.,  0.],
            [ 0.,  0.,  0.,  1.],
            ])
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
    from super_and_sub import SuperAndSub

    # msg = Msg("62.448")
    # msg = Msg("167.103")
    msg = Msg("62.448")




    for gstrs, presc in gstrs_and_presc_of_subgroups(msg):
        supersub = SuperAndSub(msg, gstrs.split(';'))
        # print(supersub.super_msg)
        print(supersub.sub_msg)
        # print(gstrs)
        # print(supersub.super_to_sub)


def test_super_and_sub():
    from msg import Msg
    from super_and_sub import SuperAndSub
    from identify_group import identify_group
    import numpy as np

    msg = Msg("167.103")
    supersub = SuperAndSub(super_msg=msg,
                           sub_msg_gstrs=
"-x+1/3,-y+2/3,-z+2/3,+1;-x+2/3,-y+1/3,-z+1/3,+1;-x+y+1/3,-x+2/3,z+2/3,+1;-x+y+2/3,-x+1/3,z+1/3,+1;-x+y,-x,z,+1;-x,-y,-z,+1;-y+1/3,x-y+2/3,z+2/3,+1;-y+2/3,x-y+1/3,z+1/3,+1;-y,x-y,z,+1;x+1/3,y+2/3,z+2/3,+1;x+2/3,y+1/3,z+1/3,+1;x,y,z,+1;x-y+1/3,x+2/3,-z+2/3,+1;x-y+2/3,x+1/3,-z+1/3,+1;x-y,x,-z,+1;y+1/3,-x+y+2/3,-z+2/3,+1;y+2/3,-x+y+1/3,-z+1/3,+1;y,-x+y,-z,+1".split(';')
)
    print(supersub.super_msg)
    print(supersub.super_msg.kvectors)
    print(supersub.sub_msg)
    print(supersub.sub_msg.kvectors)

    from fractions import Fraction
    Fraction.__repr__ = Fraction.__str__

    for ksub in supersub.sub_msg.kvectors:
        print(ksub, supersub.superunitarygs_and_superk_from_subk(ksub)[1])
    print(supersub.super_to_sub)


def test_irrep_decomp():
    from msg import Msg
    from super_and_sub import SuperAndSub
    from kvector import KVector

    msg = Msg("62.448")
    # msg = Msg("2.4")
    supersub = SuperAndSub(super_msg=msg,
                           sub_msg_gstrs="-x+1/2,y+1/2,z+1/2,-1;-x,-y,-z,+1;x+1/2,-y+1/2,-z+1/2,-1;x,y,z,+1".split(';')
                           )

    for kvec in supersub.sub_msg.kvectors:
        for decomp in supersub.irrep_decomp(kvec):
            print(decomp)


def test_irrep_tr_partners():
    from kvector import KVector
    from msg import Msg

    # msg = Msg("228.134")
    # msg = Msg("162.77")
    msg = Msg("184.191")

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
    msg2 = Msg('129.419')
    for lhs, rhs in msg2.common_comp_rels_dict('GM', 'Z').items():
        print(lhs, rhs)


def test_sis():
    from msg import Msg

    msg = Msg('102.187')
    print(msg.si_orders)
    print(msg.si_matrix)
    print(msg.irrep_labels)
    print(msg.comp_rels)
    print(msg.symmetry_indicator)
    print(msg.bs_basis)

    for bs in msg.bs_basis.T:
        print([" + ".join([label]*count)
               for count, label in zip(bs, msg.irrep_labels)
               if count != 0])


def test_diagnose():
    from band import Band
    from msg import Msg
    from itertools import accumulate
    from random import shuffle

    msg = Msg('62.441')

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

def main():
    from msg import session
    # if not session.started:
    #     session.start()
    # assert session.started

    # test_mkvec()
    # test_identify_group()
    # test_wyckoff_brs()
    # test_coreps()
    # test_Msg()
    # test_super_and_sub()
    # test_all_subgroups()
    test_irrep_decomp()
    # test_comp_rels()
    # test_irrep_tr_partners()
    # test_sis()
    # test_diagnose()

    session.terminate()


if __name__ == '__main__':
    main()
