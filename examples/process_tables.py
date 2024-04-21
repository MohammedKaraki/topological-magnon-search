from magnon.diagnose2.process_tables import process_tables
from magnon.utils.logger import create_root_logger


def main():
    result = process_tables("205.33", ["4a"], 2)
    print(result[0])


if __name__ == "__main__":
    create_root_logger()
    main()
