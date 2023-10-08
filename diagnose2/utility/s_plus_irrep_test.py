import unittest

from magnon.diagnose2.utility.s_plus_irrep import s_plus_irrep_for_point_group


class SPlusIrrepTest(unittest.TestCase):
    def test_full_table(self):
        point_group_labels = [
            "1",
            "2'",
            "m'",
            "-1",
            "2'/m'",
            "2",
            "2'2'2",
            "m'm'2",
            "2/m",
            "m'm'm",
            "m",
            "m'm2'",
            "4",
            "-4",
            "42'2'",
            "4m'm'",
            "-42'm'",
            "3",
            "32'",
            "3m'",
            "6",
            "62'2'",
            "6m'm'",
            "-6",
            "-6m'2'",
            "4/m",
            "4/mm'm'",
            "-3",
            "-3m'",
            "6/m",
            "6/mm'm'",
        ]

        expected_s_plus_irreps = [
            "A",
            "A",
            "A",
            "A_{g}",
            "A_{g}",
            "B",
            "B",
            "B",
            "B_{g}",
            "B_{g}",
            "A''",
            "A''",
            "^{1}E",
            "^{1}E",
            "^{1}E",
            "^{1}E",
            "^{1}E",
            "^{1}E",
            "^{1}E",
            "^{1}E",
            "^{1}E_{2}",
            "^{1}E_{2}",
            "^{1}E_{2}",
            "^{1}E''",
            "^{1}E''",
            "^{1}E_{g}",
            "^{1}E_{g}",
            "^{1}E_{g}",
            "^{1}E_{g}",
            "^{1}E_{2g}",
            "^{1}E_{2g}",
        ]

        assert len(point_group_labels) == len(expected_s_plus_irreps)
        for point_group, expected_irrep in zip(
            point_group_labels, expected_s_plus_irreps
        ):
            assert s_plus_irrep_for_point_group(point_group) == expected_irrep, (
                point_group,
                expected_irrep,
            )


if __name__ == "__main__":
    unittest.main()
