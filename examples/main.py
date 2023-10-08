from magnon.groups.find_subgroups_py import find_subgroups
from magnon.groups.read_standard_magnetic_space_groups_py import (
    read_standard_msgs_from_disk,
)
from magnon.fetch.magnetic_space_group_from_generators import fetch_msg_from_generators


def main():
    supergroup_number = "205.33"
    for induced_group_info in find_subgroups(
        supergroup_number, read_standard_msgs_from_disk()
    ):
        subgroup = fetch_msg_from_generators(
            induced_group_info.unbroken_standard_general_positions
        )
        print(induced_group_info.perturbation_prescription)
        print(subgroup.label)


if __name__ == "__main__":
    main()
