from primvecs import find_primvecsmat
from msg import Msg
from magndata import load_materials
import json


def primcells_per_convcell(latticetype):
    return {'P': 1,
            'A': 2,
            'C': 2,
            'I': 2,
            'R': 3,
            'F': 4
            }[latticetype]


def int_cast(f):
    result = int(round(f))
    assert float(result) - f == 0.0

    return result


def mat_to_numbands(mat):
    msg_number = mat['msg']
    latticetype = Msg(msg_number).label[0]
    wps = mat['wp']

    conv_sites = 0
    for wp in wps:
        delta = int(wp[0][:-1])
        conv_sites += delta

    prim_sites = int_cast(conv_sites / primcells_per_convcell(latticetype))
    return conv_sites, prim_sites


def summarize():
    materials = load_materials()

    for mat in materials:
        msg_number = mat['msg']
        wps = mat['wp']

        print(*mat_to_numbands(mat), msg_number, Msg(msg_number).label, wps)


def main():
    summarize()


if __name__ == "__main__":
    main()
