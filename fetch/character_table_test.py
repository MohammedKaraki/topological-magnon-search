import unittest

from magnon.fetch.character_table import fetch_single_valued_unitary_character_table
from magnon.common.matrix_converter_py import matrixxcd_from_proto

import numpy as np

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class MagneticSpaceGroupFromGeneratorsTest(unittest.TestCase):
    def test_trivial_group_at_gamma(self):
        table = fetch_single_valued_unitary_character_table("1.1", "GM")

        expected_general_positions = ["x,y,z,+1"]
        expected_irrep_labels = ["GM_{1}"]
        expected_irrep_dimensions = [1]
        expected_matrix = np.array([[1.0]])

        assert table.general_position == expected_general_positions
        assert [irrep.label for irrep in table.irrep] == expected_irrep_labels
        assert [irrep.dimension for irrep in table.irrep] == expected_irrep_dimensions

        matrix = matrixxcd_from_proto(table.matrix)
        assert np.all(matrix == expected_matrix)

    def test_inversion_group_at_gamma(self):
        table = fetch_single_valued_unitary_character_table("2.4", "GM")

        expected_general_positions = ["x,y,z,+1", "-x,-y,-z,+1"]
        expected_irrep_labels = ["GM_{1}^{+}", "GM_{1}^{-}"]
        expected_irrep_dimensions = [1, 1]
        expected_matrix = np.array([[1.0, 1.0], [1.0, -1.0]])

        assert table.general_position == expected_general_positions
        assert [irrep.label for irrep in table.irrep] == expected_irrep_labels
        assert [irrep.dimension for irrep in table.irrep] == expected_irrep_dimensions

        matrix = matrixxcd_from_proto(table.matrix)
        assert np.all(matrix == expected_matrix)

    def test_p_6_1_group_at_h(self):
        table = fetch_single_valued_unitary_character_table("169.113", "H")

        expected_general_positions = [
            "x,y,z,+1",
            "-y,x-y,z+1/3,+1",
            "-x+y,-x,z+2/3,+1",
        ]
        expected_irrep_labels = [
            "H_{1}",
            "H_{2}",
            "H_{3}",
        ]
        expected_irrep_dimensions = [1, 1, 1]
        omega = np.exp(1.0j * np.pi / 3.0)
        omega2 = np.exp(2.0j * np.pi / 3.0)
        expected_matrix = np.array(
            [[1.0, 1.0, 1.0], [omega, -1.0, omega.conj()], [omega2, 1.0, omega2.conj()]]
        )

        matrix = matrixxcd_from_proto(table.matrix)
        assert table.general_position == expected_general_positions
        assert [irrep.label for irrep in table.irrep] == expected_irrep_labels
        assert [irrep.dimension for irrep in table.irrep] == expected_irrep_dimensions
        assert np.allclose(matrix, expected_matrix), matrix - expected_matrix


if __name__ == "__main__":
    unittest.main()
