import argparse

from magnon.diagnose2.process_tables import process_tables
from magnon.common.logger import create_root_logger


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--msg", required=True, type=str, help="MSG Number")
    parser.add_argument(
        "--wps", required=True, type=str, help="Wyckoff Positions (comma-separated)"
    )
    parser.add_argument("--output", required=True, type=str, help="Output filename")
    args = parser.parse_args()
    print(args)
    msg = args.msg
    wps = args.wps.split(",")
    output = args.output
    result = process_tables(msg, wps)
    with open(output, "w") as f:
        print(result, file=f)


if __name__ == "__main__":
    create_root_logger()
    main()
