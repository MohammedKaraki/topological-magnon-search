import unittest

from magnon.diagnose2.process_tables import process_tables
from magnon.diagnose2.perturbed_band_structure_pb2 import PerturbedBandStructure

from google.protobuf import text_format

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""

_PROCESSED_TABLES_PATH = "diagnose2/test_data/processed_tables_205_33_4a_2_4.txtpb"


class ProcessTablesTest(unittest.TestCase):
    def test_process_table_example(self):
        expected_structure = PerturbedBandStructure()
        with open(_PROCESSED_TABLES_PATH, "r") as f:
            text_format.Parse(f.read(), expected_structure)
        assert expected_structure.supergroup.number == "205.33"
        assert expected_structure.subgroup.number == "2.4"

        structure = process_tables("205.33", ["4a"], 2)[0]
        print(structure.supergroup)

        assert structure == expected_structure


if __name__ == "__main__":
    unittest.main()
