from re import fullmatch

_latticetype_dic = None


def _parse_msg_number(msg_number):
    m = fullmatch(r"([0-9]+)\.([0-9]+)", msg_number)
    assert m

    n1, n2 = m.groups()
    return n1, n2


def lattice_type_for_group_number(msg_number):
    sg_number, _ = _parse_msg_number(msg_number)
    assert fullmatch(r"[0-9]+", sg_number), sg_number
    global _latticetype_dic
    if not _latticetype_dic:
        _latticetype_dic = {}

        from pathlib import Path

        filename = "data/lattice_type_space_group_number.txt"

        with open(filename, "r") as f:
            for line in f.readlines():
                letter, nums_col = line.strip().split(":")
                assert letter in ["P", "A", "I", "F", "C", "R"]
                nums = nums_col.split(",")
                for num in nums:
                    _latticetype_dic[num] = letter

    return _latticetype_dic[sg_number]
