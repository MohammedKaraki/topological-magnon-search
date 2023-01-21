from all_subgroups import gstrs_and_presc_of_subgroups
from msg import Msg
from msg_info_table import MSG_INFO_TABLE
from identify_group import identify_group
from subprocess import check_output
import sys


def preprocess(msg_number, wp):
    msg = Msg(msg_number)
    id = 0
    for gstrs, _ in gstrs_and_presc_of_subgroups(msg):
        identified_number = identify_group(gstrs.split(';'))['group_number']
        si_str = MSG_INFO_TABLE[identified_number][1]
        if si_str != '(1)':
            print(id, identified_number, si_str)
            check_output(['python', 'perturb_wp.py',
                          msg_number,
                          wp,
                          str(id)
                          ]
                         )

        id += 1


def main():
    args = sys.argv
    assert len(args) >= 2
    command = args[1]

    valid_commands = {"preprocess"}
    assert command in valid_commands

    if command == "preprocess":
        assert len(args) >= 2 + 2
        msg_number = str(args[2])
        wp = str(args[3])
        preprocess(msg_number, wp)

    else:
        assert False


if __name__ == "__main__":
    main()
