import os

print(os.path.abspath(os.getcwd()))
import sys
from subprocess import check_output
from magnon.preprocess.msg import Msg
from magnon.preprocess.all_subgroups import gstrs_and_presc_of_subgroups
from magnon.preprocess.msg_info_table import MSG_INFO_TABLE
from magnon.preprocess.identify_group import identify_group
from magnon.preprocess.perturb_wp import perturb_wp

from magnon.preprocess import log


def logfile_path():
    return "/tmp/output.log"


def read_args():
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
        identified_number = identify_group(gstrs.split(";"))["group_number"]
        si_str = MSG_INFO_TABLE[identified_number][1]
        if si_str != "(1)":
            print(id, identified_number, si_str, file=sys.stderr)
            perturb_wp(msg_number, wp, str(id))

        id += 1


if __name__ == "__main__":
    logger = log.create_root_logger(filename=logfile_path())
    main()
