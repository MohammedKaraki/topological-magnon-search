from magnon.summary.msg_summary_pb2 import MsgsSummary
from magnon.latexify.latexify_result import (
    tex_document_from_template,
    possible_si_table_from_result,
    possible_gap_count_table_from_result,
)
from google.protobuf import text_format
from config.output_dirs_python import get_output_dirs

output_dirs = get_output_dirs()
msg_summary_dir = output_dirs["msg_summary_dir"]
si_tables_dir = output_dirs["si_tables_dir"]
gap_tables_dir = output_dirs["gap_tables_dir"]


def main():
    args = CommandLineArgs()

    msg_summary_filepath = "{}/{}.pb.txt".format(msg_summary_dir, args.msg)
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

            def write_to_file(text, pathname):
                print(text, file=open(pathname, "w"))
                print("Output: {}".format(pathname))

            write_to_file(
                si_table,
                make_table_filepath(
                    si_tables_dir,
                    result.supergroup_number,
                    result.subgroup_number,
                    wps,
                ),
            )
            write_to_file(
                gap_table,
                make_table_filepath(
                    gap_tables_dir,
                    result.supergroup_number,
                    result.subgroup_number,
                    wps,
                ),
            )


def make_table_filepath(tables_dir, supergroup_number, subgroup_number, wps):
    wps_encoding = "+".join(sorted(list(wps)))
    return "{}/{}_{}_{}_table.tex".format(
        tables_dir, supergroup_number, subgroup_number, wps_encoding
    )


class CommandLineArgs:
    def __init__(self):
        import argparse

        parser = argparse.ArgumentParser(allow_abbrev=False)
        parser.add_argument("--msg", required=True, type=str, help="MSG Number")
        args = parser.parse_args()

        self.msg = args.msg


if __name__ == "__main__":
    main()
