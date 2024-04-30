from magnon.run_summary.msg_summary_pb2 import MsgsSummary
from magnon.latexify.latexify_result import (
    tex_document_from_template,
    possible_si_table_from_result,
    possible_gap_count_table_from_result,
)
from google.protobuf import text_format

MSG_SUMMARY_DIR = "data/msg_summary"


def main():
    args = CommandLineArgs()

    msg_summary_filepath = "{}/{}.pb.txt".format(MSG_SUMMARY_DIR, args.msg)
    msg_summary = text_format.Parse(
        open(msg_summary_filepath, "r").read(), MsgsSummary.MsgSummary()
    )
    for wps_summary in msg_summary.wps_summary:
        for pert_summary in wps_summary.perturbation_summary:
            result = pert_summary.search_result
            if result.is_negative_diagnosis:
                continue

            si_table = possible_si_table_from_result(result)
            gap_table = possible_si_table_from_result(result)

            wps = [str(orb.wyckoff_position.label) for orb in result.atomic_orbital]
            print(
                si_table,
                file=open(
                    make_si_table_filepath(
                        args.output_dir,
                        result.supergroup_number,
                        result.subgroup_number,
                        wps,
                    ),
                    "w",
                ),
            )
            print(
                gap_table,
                file=open(
                    make_gap_table_filepath(
                        args.output_dir,
                        result.supergroup_number,
                        result.subgroup_number,
                        wps,
                    ),
                    "w",
                ),
            )


def make_si_table_filepath(output_dir, supergroup_number, subgroup_number, wps):
    wps_encoding = "+".join(sorted(list(wps)))
    return "{}/si_tables/{}_{}_{}_table.tex".format(
        output_dir, supergroup_number, subgroup_number, wps_encoding
    )


def make_gap_table_filepath(output_dir, supergroup_number, subgroup_number, wps):
    wps_encoding = "+".join(sorted(list(wps)))
    return "{}/gap_tables/{}_{}_{}_table.tex".format(
        output_dir, supergroup_number, subgroup_number, wps_encoding
    )


class CommandLineArgs:
    def __init__(self):
        import argparse

        parser = argparse.ArgumentParser(allow_abbrev=False)
        parser.add_argument("--msg", required=True, type=str, help="MSG Number")
        parser.add_argument(
            "--output_dir", required=True, type=str, help="Output result directory."
        )
        args = parser.parse_args()

        self.msg = args.msg
        self.output_dir = args.output_dir


if __name__ == "__main__":
    main()
