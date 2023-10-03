import sys
import argparse
from subprocess import check_output

from magnon.preprocess.msg import Msg
from magnon.preprocess.all_subgroups import gstrs_and_presc_of_subgroups
from magnon.preprocess.msg_info_table import MSG_INFO_TABLE
from magnon.preprocess.identify_group import identify_group
from magnon.preprocess.perturb_wp import perturb_wp

from magnon.preprocess import log


def main():
    parser = argparse.ArgumentParser(
        prog="analyze_relevant_subgroups",
        description="Given a magnetic space group and a Wyckoff position of the magnetic "
        "atoms, this program filters the subgroups of the MSG for non-trivial symmetry "
        "indicator groups. Subsequently, it fetches and analysis releveant "
        "group representation tables for the remaining subgroups. The resulting data "
        "can then be used by processed by the topological magnon diagnosis tool to "
        "identify candidate materials.",
    )
    parser.add_argument(
        "--msg_number",
        required=True,
        help="The magnetic space group number",
    )
    parser.add_argument(
        "--wp",
        required=True,
        help="The Wyckoff position of the magnetic atoms",
    )
    args = parser.parse_args()
    msg_number, wp = args.msg_number, args.wp

    msg = Msg(msg_number)
    for id, (gstrs, _) in enumerate(gstrs_and_presc_of_subgroups(msg)):
        identified_number = identify_group(gstrs.split(";"))["group_number"]
        si_str = MSG_INFO_TABLE[identified_number][1]
        if si_str != "(1)":
            print(id, identified_number, si_str, file=sys.stderr)
            perturb_wp(msg_number, wp, str(id))


if __name__ == "__main__":
    logger = log.create_root_logger()
    main()
