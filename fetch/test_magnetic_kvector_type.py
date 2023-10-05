from magnon.fetch.magnetic_kvector_type import fetch_magnetic_kvector_types


def main():
    kvector_types_for_2_4 = fetch_magnetic_kvector_types("2.4")
    kvector_types_for_205_33 = fetch_magnetic_kvector_types("205.33")
    assert kvector_types_for_2_4.magnetic_space_group.number == "2.4"
    assert kvector_types_for_2_4.type[0].label == "GM"
    assert kvector_types_for_2_4.type[-1].label == "GP"
    assert len(kvector_types_for_2_4.type) == 9

    assert kvector_types_for_205_33.magnetic_space_group.number == "205.33"
    assert len(kvector_types_for_205_33.type) == 15
    assert kvector_types_for_205_33.type[0].label == "GM"
    assert kvector_types_for_205_33.type[-1].label == "C"


if __name__ == "__main__":
    main()
