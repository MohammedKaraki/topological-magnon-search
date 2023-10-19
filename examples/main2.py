from magnon.groups.find_subgroups_py import find_subgroups
from magnon.groups.read_standard_magnetic_space_groups_py import (
    read_standard_msgs_from_disk,
)
from magnon.fetch.magnetic_space_group_from_generators import fetch_msg_from_generators
from magnon.diagnose2.process_tables import process_tables
from magnon.diagnose2.perturbed_band_structure_pb2 import PerturbedBandStructures
import json
import sys
import os

from magnon.common.logger import create_root_logger, create_logger
from google.protobuf import text_format

_logger = create_logger(__name__)


def main():
    create_root_logger()

    assert len(sys.argv) == 2
    sys.argv[1]

    p = text_format.Parse(open(sys.argv[1], "r").read(), PerturbedBandStructures())
    print(len(p.SerializeToString()))


if __name__ == "__main__":
    main()
