from re import fullmatch

from magnon.preprocess import log

logger = log.create_logger(__name__)


latticetype_dic = None


def parse_msg_number(msg_number):
    m = fullmatch(r"([0-9]+)\.([0-9]+)", msg_number)
    assert m

    n1, n2 = m.groups()
    return n1, n2


def find_latticetype(msg_number):
    sg_number, _ = parse_msg_number(msg_number)
    assert fullmatch(r"[0-9]+", sg_number), sg_number
    global latticetype_dic
    if not latticetype_dic:
        latticetype_dic = {}

        from pathlib import Path

        filename = "data/latticetype_spacegroupnumber.txt"

        with open(filename, "r") as f:
            for line in f.readlines():
                letter, nums_col = line.strip().split(":")
                assert letter in ["P", "A", "I", "F", "C", "R"]
                nums = nums_col.split(",")
                for num in nums:
                    latticetype_dic[num] = letter

    return latticetype_dic[sg_number]
