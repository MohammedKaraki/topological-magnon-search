import unittest

from magnon.fetch.wyckoff_positions import fetch_wyckoff_positions

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class WyckoffPositions(unittest.TestCase):
    def test_fetch_wyckoff_positions_2_4(self):
        wps = fetch_wyckoff_positions("2.4")
        assert len(wps) == 9
        assert wps[0].label == "2i"
        assert wps[0].orbit.coordinates == [
            "(x,y,z),(m_{x},m_{y},m_{z})",
            "(-x,-y,-z),(m_{x},m_{y},m_{z})",
        ]
        assert wps[-1].label == "1a"
        assert wps[-1].orbit.coordinates == ["(0,0,0),(m_{x},m_{y},m_{z})"]


if __name__ == "__main__":
    unittest.main()
