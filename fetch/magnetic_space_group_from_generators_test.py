import unittest

from magnon.fetch.magnetic_space_group_from_generators import (
    fetch_magnetic_space_group_from_generators,
)

import numpy as np
from magnon.common.matrix_converter_py import matrix4d_from_proto

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class MagneticSpaceGroupFromGeneratorsTest(unittest.TestCase):
    def test_trivial_group_standard(self):
        msg = fetch_magnetic_space_group_from_generators(["x,y,z,+1"])
        assert msg.number == "1.1"
        assert msg.label == "P1"
        current_from_standard = matrix4d_from_proto(msg.current_from_standard_basis)
        assert np.all(current_from_standard == np.identity(4))

    def test_trivial_group_smaller_c(self):
        msg = fetch_magnetic_space_group_from_generators(["x,y,z+1/2,+1"])
        current_from_standard_expected = np.array(
            [
                [1.0, 0.0, 0.0, 0.0],
                [0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 0.5, 0.0],
                [0.0, 0.0, 0.0, 1.0],
            ]
        )

        assert msg.number == "1.1"
        assert msg.label == "P1"
        current_from_standard = matrix4d_from_proto(msg.current_from_standard_basis)
        assert np.all(current_from_standard == current_from_standard_expected)

    def test_inversion_displaced_origin(self):
        msg = fetch_magnetic_space_group_from_generators(["-x+1/2,-y,-z,+1"])
        current_from_standard_expected = np.array(
            [
                [1.0, 0.0, 0.0, 0.25],
                [0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 1.0, 0.0],
                [0.0, 0.0, 0.0, 1.0],
            ]
        )

        assert msg.number == "2.4"
        assert msg.label == "P-1"
        current_from_standard = matrix4d_from_proto(msg.current_from_standard_basis)
        assert np.all(current_from_standard == current_from_standard_expected)

    def test_rotated_axes(self):
        msg = fetch_magnetic_space_group_from_generators(
            [
                "x+1/2,y-1/2,-2x-z,+1",
                "-x-z,z,-y+z,+1",
            ]
        )
        current_from_standard_expected = np.array(
            [
                [1.0, 0.0, 0.0, 0.0],
                [-1.0, 1.0, 0.0, 0.0],
                [-1.0, 0.0, 1.0, 0.0],
                [0.0, 0.0, 0.0, 1.0],
            ]
        )
        assert msg.number == "205.33"
        assert msg.label == "Pa-3"
        current_from_standard = matrix4d_from_proto(msg.current_from_standard_basis)
        assert np.all(current_from_standard == current_from_standard_expected)


if __name__ == "__main__":
    unittest.main()
