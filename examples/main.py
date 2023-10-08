from magnon.groups.find_subgroups_py import find_subgroups
from magnon.groups.read_standard_magnetic_space_groups_py import (
    read_standard_msgs_from_disk,
)
from magnon.fetch.magnetic_space_group_from_generators import fetch_msg_from_generators
from magnon.diagnose2.process_tables import process_tables
import json

def main():
    print(json.dumps(process_tables("205.33", "4a", 2)))
    pass

if __name__ == "__main__":
    main()
