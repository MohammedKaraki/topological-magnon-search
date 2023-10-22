from magnon.diagnose2.process_tables import process_tables
from magnon.common.logger import create_root_logger


def main():
    process_result = process_tables("205.33",['4a'])
    print(process_result)

if __name__ == "__main__":
    create_root_logger()
    main()
