import unittest

from magnon.fetch.magnetic_kvector_type import fetch_magnetic_kvector_types

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class FetchMagneticKVectorTypesTest(unittest.TestCase):
    def kvector_types(self):
        kvector_types = fetch_magnetic_kvector_types("2.4")
        assert kvector_types.magnetic_space_group.number == "2.4"
        assert kvector_types.type[0].label == "GM"
        assert kvector_types.type[-1].label == "GP"
        assert len(kvector_types.type) == 9

    def test_kvector_types_205_33(self):
        kvector_types = fetch_magnetic_kvector_types("205.33")

        assert kvector_types.magnetic_space_group.number == "205.33"
        assert len(kvector_types.type) == 15
        assert kvector_types.type[0].label == "GM"
        assert kvector_types.type[-1].label == "C"


if __name__ == "__main__":
    unittest.main()
