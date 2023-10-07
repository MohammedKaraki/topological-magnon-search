import unittest

from magnon.fetch.general_positions import fetch_unitary_general_positions

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class GeneralPositionsTest(unittest.TestCase):
    def test_fetch_unitary_general_positions_trivial_group(self):
        general_positions = fetch_unitary_general_positions("2.4")

        expected_coordinates_forms = ["x,y,z,+1", "-x,-y,-z,+1"]
        expected_seitz_forms = ["{1|0}", "{-1|0}"]
        assert [
            general_position.coordinates_form
            for general_position in general_positions.general_position
        ] == expected_coordinates_forms
        assert [
            general_position.seitz_form
            for general_position in general_positions.general_position
        ] == expected_seitz_forms

    def test_fetch_unitary_general_positions_2_7(self):
        general_positions = fetch_unitary_general_positions("2.7")

        assert len(general_positions.general_position) == 2
        expected_coordinates_forms = [
            "x,y,z,+1",
            "-x,-y,-z,+1",
        ]
        expected_seitz_forms = ["{1|0}", "{-1|0}"]
        assert [
            general_position.coordinates_form
            for general_position in general_positions.general_position
        ] == expected_coordinates_forms
        assert [
            general_position.seitz_form
            for general_position in general_positions.general_position
        ] == expected_seitz_forms

    def test_fetch_unitary_general_positions_167_103(self):
        general_positions = fetch_unitary_general_positions("167.103")

        assert len(general_positions.general_position) == 36
        expected_last_coordinates_forms = [
            "-y+1/3,-x+2/3,z+1/6,+1",
            "x+1/3,x-y+2/3,z+1/6,+1",
        ]
        expected_last_seitz_forms = ["{m_{110}|1/32/31/6}", "{m_{010}|1/32/31/6}"]
        assert [
            general_position.coordinates_form
            for general_position in general_positions.general_position
        ][-2:] == expected_last_coordinates_forms
        assert [
            general_position.seitz_form
            for general_position in general_positions.general_position
        ][-2:] == expected_last_seitz_forms


if __name__ == "__main__":
    unittest.main()
