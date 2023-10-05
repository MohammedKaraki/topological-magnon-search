import unittest

from magnon.fetch.magnetic_band_representation import (
    fetch_elementary_band_representations_for_group,
    fetch_atomic_band_representations_for_wyckoff_position,
)

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class FetchElementaryBandRepresentationsTest(unittest.TestCase):
    def test_fetch_elementary_band_representations(self):
        ebrs = fetch_elementary_band_representations_for_group("139.536")
        assert len(ebrs) == 20

        third_ebr = ebrs[2]
        assert third_ebr.atomic_orbital.wyckoff_position.label == "2a"
        assert (
            third_ebr.atomic_orbital.wyckoff_position.site_symmetry_group_label
            == "4'/m'm'm"
        )
        assert third_ebr.atomic_orbital.site_symmetry_irrep.label == "B_{1}"
        assert third_ebr.kspace_little_irrep[0].label == "GM_{2}"
        assert third_ebr.kspace_little_irrep[0].dimension == 1
        assert third_ebr.kspace_little_irrep[3].label == "PA_{2}"
        assert third_ebr.kspace_little_irrep[3].dimension == 1


class FetchAtomicBandRepresentationsTest(unittest.TestCase):
    def test_fetch_atomic_band_representations_for_ebr(self):
        brs = fetch_atomic_band_representations_for_wyckoff_position("139.536", "8f")
        assert len(brs) == 2

        first_br = brs[0]
        assert first_br.atomic_orbital.wyckoff_position.label == "8f"
        assert (
            first_br.atomic_orbital.wyckoff_position.site_symmetry_group_label == "2'/m"
        )
        assert first_br.atomic_orbital.site_symmetry_irrep.label == "A'"
        assert len(first_br.kspace_little_irrep) == 20
        assert first_br.kspace_little_irrep[2].label == "GM_{5}"
        assert first_br.kspace_little_irrep[2].dimension == 2

    def test_fetch_atomic_band_representations_for_non_ebr(self):
        brs = fetch_atomic_band_representations_for_wyckoff_position("139.536", "16k")
        assert len(brs) == 1

        first_br = brs[0]
        assert first_br.atomic_orbital.wyckoff_position.label == "16k"
        assert (
            first_br.atomic_orbital.wyckoff_position.site_symmetry_group_label == "2'"
        )
        assert first_br.atomic_orbital.site_symmetry_irrep.label == "A"
        assert len(first_br.kspace_little_irrep) == 40
        assert first_br.kspace_little_irrep[3].label == "GM_{4}"
        assert first_br.kspace_little_irrep[3].dimension == 1


if __name__ == "__main__":
    unittest.main()
