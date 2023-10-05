from magnon.fetch.magnetic_band_representation import (
    _point_group_and_site_irrep_to_induced_little_irreps_map,
    kvectors_and_ebrs,
)


def main():
    print(_point_group_and_site_irrep_to_induced_little_irreps_map("2.4", "4a"))
    print(kvectors_and_ebrs("2.4"))


if __name__ == "__main__":
    main()
