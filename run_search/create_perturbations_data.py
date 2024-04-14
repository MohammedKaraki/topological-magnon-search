from magnon.diagnose2.process_tables import process_tables
from magnon.common.logger import create_root_logger


def main():
    args = CommandLineArgs()
    with open(args.output_filename, "w") as f:
        result = process_tables(args.msg, args.wps)
        print(result, file=f)


class CommandLineArgs:
    def __init__(self):
        import argparse

        parser = argparse.ArgumentParser(allow_abbrev=False)
        parser.add_argument("--msg", required=True, type=str, help="MSG Number")
        parser.add_argument(
            "--wps", required=True, type=str, help="Wyckoff Positions (comma-separated)"
        )
        parser.add_argument(
            "--output_file", required=True, type=str, help="Output filename"
        )
        args = parser.parse_args()

        self.msg = args.msg
        self.wps = args.wps.split(",")
        self.output_filename = args.output_file


if __name__ == "__main__":
    create_root_logger()
    main()
