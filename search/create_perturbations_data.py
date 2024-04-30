from magnon.diagnose2.process_tables import process_tables
from magnon.utils.logger import create_root_logger
import os


def main():
    args = CommandLineArgs()
    filename = make_filename(args.msg, args.wps)

    output_filepath = os.path.join(args.output_dir, filename)
    print(output_filepath)
    if os.path.isfile(output_filepath):
        print("Cache hit: {}".format(output_filepath))
        return

    result = process_tables(args.msg, args.wps)
    with open(output_filepath, "w") as f:
        print(result, file=f)


def make_filename(msg, wps):
    wps_encoding = "+".join(sorted(list(wps)))
    return "{}_{}.pb.txt".format(msg, wps_encoding)


class CommandLineArgs:
    def __init__(self):
        import argparse

        parser = argparse.ArgumentParser(allow_abbrev=False)
        parser.add_argument("--msg", required=True, type=str, help="MSG Number")
        parser.add_argument(
            "--wps", required=True, type=str, help="Wyckoff Positions (comma-separated)"
        )
        parser.add_argument(
            "--output_dir", required=True, type=str, help="Output result directory."
        )
        args = parser.parse_args()

        self.msg = args.msg
        self.wps = args.wps.split(",")
        self.output_dir = args.output_dir


if __name__ == "__main__":
    create_root_logger()
    main()
