import unittest

from magnon.fetch.kvector_stars import fetch_kvector_stars

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class FetchMagneticKVectorTypesTest(unittest.TestCase):
    def test_kvector_stars_2_4(self):
        kvector_stars = fetch_kvector_stars("2.4")
        assert kvector_stars[0].label == "GM"
        assert kvector_stars[-1].label == "GP"
        assert len(kvector_stars) == 9

    def test_kvector_stars_205_33(self):
        kvector_stars = fetch_kvector_stars("205.33")
        print(kvector_stars)

        assert len(kvector_stars) == 15
        assert kvector_stars[0].label == "GM"
        assert kvector_stars[-1].label == "C"


if __name__ == "__main__":
    unittest.main()
