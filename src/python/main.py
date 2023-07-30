from all_subgroups import gstrs_and_presc_of_subgroups
from msg import Msg
from msg_info_table import MSG_INFO_TABLE
from identify_group import identify_group
from subprocess import check_output
import sys


def preprocess(msg_number, wps):
    msg = Msg(msg_number)

    for id, (gstrs, _) in enumerate(gstrs_and_presc_of_subgroups(msg)):
        identified_number = identify_group(gstrs.split(';'))['group_number']
        si_str = MSG_INFO_TABLE[identified_number][1]
        if si_str != '(1)':
            print(id, identified_number, si_str)
            check_output(['python', 'perturb_wps.py',
                          msg_number,
                          *wps,
                          str(id)
                          ]
                         )


def main():
    assert len(sys.argv) >= 2
    command = sys.argv[1]
    args = sys.argv[2:]

    if command == "preprocess":
        assert len(args) >= 2
        msg_number = str(args[0])
        wps = args[1:]

        preprocess(msg_number, wps)

    else:
        assert False


if __name__ == "__main__":
    main()
