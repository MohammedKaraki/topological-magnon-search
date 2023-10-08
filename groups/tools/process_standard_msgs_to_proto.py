from magnon.groups.magnetic_space_group_pb2 import (
    MagneticSpaceGroup,
    MagneticSpaceGroups,
    GeneralPosition,
)


def main():
    groups = MagneticSpaceGroups()
    with open("data/msg_number_label_si_genpos.txt", "r") as f:
        for line in f.readlines():
            number, label, si, genpos = line.split()
            group = MagneticSpaceGroup(number=number, label=label)
            for order in [int(x) for x in si[1:-1].split(",")]:
                assert order in (1, 2, 3, 4, 6, 12)
                group.symmetry_indicator_order.append(order)
            gstrs = genpos.split(";")
            assert len(gstrs) >= 1
            assert gstrs[0] == "x,y,z,+1"
            for gstr in gstrs:
                group.generators.general_position.append(
                    GeneralPosition(coordinates_form=gstr)
                )

            groups.group.append(group)
    print(groups)


if __name__ == "__main__":
    main()
