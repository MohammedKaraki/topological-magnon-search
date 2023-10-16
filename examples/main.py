from magnon.groups.find_subgroups_py import find_subgroups
from magnon.groups.read_standard_magnetic_space_groups_py import (
    read_standard_msgs_from_disk,
)
from magnon.fetch.magnetic_space_group_from_generators import fetch_msg_from_generators
from magnon.diagnose2.process_tables import process_tables
import json

from magnon.common.logger import create_root_logger


def main():
    create_root_logger()
    print(process_tables("205.33", ["4a", "4a"], 2))


if __name__ == "__main__":
    main()
