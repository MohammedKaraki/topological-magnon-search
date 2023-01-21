from all_subgroups import gstrs_and_presc_of_subgroups
from msg import Msg
from msg_info_table import MSG_INFO_TABLE
from identify_group import identify_group
from subprocess import check_output


def read_args():
    import sys

    try:
        _, msg_number, wp = sys.argv
    except:
        raise ValueError("Invalid input arguments.")

    return str(msg_number), str(wp)


def main():
    msg_number, wp = read_args()


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


if __name__ == "__main__":
    main()
