import unittest

from magnon.fetch.antiunitarily_related_irreps import fetch_antiunitarily_related_irreps

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class AntiunitarilyRelatedIrrepsTest(unittest.TestCase):
    def test_trivial_group_at_gamma(self):
        pairs = fetch_antiunitarily_related_irreps("2.7")
        assert len(pairs.pair) == 0

    def test_trivial_group_at_gamma(self):
        pairs = fetch_antiunitarily_related_irreps("103.198")
        print(pairs)
        assert len(pairs.pair) == 8
        expected_first_klabel = ["R"] * 4 + ["X"] * 4
        expected_first_irrep_label = ["R_{{{}}}".format(x) for x in [1, 2, 3, 4]] + [
            "X_{{{}}}".format(x) for x in [1, 2, 3, 4]
        ]
        expected_second_klabel = ["RA"] * 4 + ["XA"] * 4
        expected_second_irrep_label = ["RA_{{{}}}".format(x) for x in [2, 1, 3, 4]] + [
            "XA_{{{}}}".format(x) for x in [1, 2, 4, 3]
        ]
        assert [
            pair.first_kvector_type_label for pair in pairs.pair
        ] == expected_first_klabel
        assert [
            pair.first_little_irrep_label for pair in pairs.pair
        ] == expected_first_irrep_label
        assert [
            pair.second_kvector_type_label for pair in pairs.pair
        ] == expected_second_klabel
        assert [
            pair.second_little_irrep_label for pair in pairs.pair
        ] == expected_second_irrep_label


if __name__ == "__main__":
    unittest.main()
