from bazel_tools.tools.python.runfiles import runfiles
from preprocess.all_subgroups import gstrs_and_presc_of_subgroups
from preprocess.msg import Msg
from preprocess.msg_info_table import MSG_INFO_TABLE
from preprocess.identify_group import identify_group
from subprocess import check_output
from preprocess.perturb_wp import perturb_wp
import sys

def logfile_path():
    return "logs/tests.log"

def read_args():

    try:
        _, msg_number, wp = sys.argv
    except:
        raise ValueError("Invalid input arguments.")

    return str(msg_number), str(wp)


def main():
    r = runfiles.Create()
    print(r.Rlocation("bazel-bin/preprocess/perturb_wp"))
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
    import log

    logger = log.create_root_logger(filename=logfile_path())
    main()
