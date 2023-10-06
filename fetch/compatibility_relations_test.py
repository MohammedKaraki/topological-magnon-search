import unittest

from magnon.fetch.compatibility_relations import (
    fetch_compatibility_relations,
)

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class CompatibilityRelationsTest(unittest.TestCase):
    def test_trivial_group_gamma(self):
        relations = fetch_compatibility_relations("1.1", "GM")
        assert len(relations.relation) == 1
        assert relations.relation[0].kpoint_little_irrep.label == "GM_{1}"
        assert relations.relation[0].kpoint_little_irrep.dimension == 1

        assert relations.relation[0].kline_kvector_type_label == "GP"
        assert relations.relation[0].kline_coordinates == "(u,v,w)"

        assert len(relations.relation[0].kline_little_irrep) == 1
        assert relations.relation[0].kline_little_irrep[0].label == "GP_{1}"
        assert relations.relation[0].kline_little_irrep[0].dimension == 1

    def test_166_99_f(self):
        relations = fetch_compatibility_relations("166.99", "F")
        assert len(relations.relation) == 6

        assert relations.relation[0].kpoint_little_irrep.label == "F_{1}"
        assert relations.relation[0].kpoint_little_irrep.dimension == 1

        assert relations.relation[0].kline_kvector_type_label == "SM"
        assert relations.relation[0].kline_coordinates == "(1-2*u,u,1)"

        assert len(relations.relation[0].kline_little_irrep) == 1
        assert relations.relation[0].kline_little_irrep[0].label == "SM_{1}"
        assert relations.relation[0].kline_little_irrep[0].dimension == 1

        assert relations.relation[1].kpoint_little_irrep.label == "F_{2}"
        assert relations.relation[1].kpoint_little_irrep.dimension == 1

        assert relations.relation[1].kline_kvector_type_label == "SM"
        assert relations.relation[1].kline_coordinates == "(1-2*u,u,1)"

        assert len(relations.relation[1].kline_little_irrep) == 1
        assert relations.relation[1].kline_little_irrep[0].label == "SM_{1}"
        assert relations.relation[1].kline_little_irrep[0].dimension == 1

    def test_205_33_x(self):
        relations = fetch_compatibility_relations("205.33", "X")
        assert len(relations.relation) == 16

        assert relations.relation[0].kpoint_little_irrep.label == "X_{1}"
        assert relations.relation[0].kpoint_little_irrep.dimension == 2

        assert relations.relation[0].kline_kvector_type_label == "DT"
        assert relations.relation[0].kline_coordinates == "(0,v,0)"

        assert len(relations.relation[0].kline_little_irrep) == 2
        assert relations.relation[0].kline_little_irrep[0].label == "DT_{2}"
        assert relations.relation[0].kline_little_irrep[0].dimension == 1
        assert relations.relation[0].kline_little_irrep[1].label == "DT_{3}"
        assert relations.relation[0].kline_little_irrep[1].dimension == 1

        assert relations.relation[1].kpoint_little_irrep.label == "X_{2}"
        assert relations.relation[1].kpoint_little_irrep.dimension == 2

        assert relations.relation[1].kline_kvector_type_label == "DT"
        assert relations.relation[1].kline_coordinates == "(0,v,0)"

        assert len(relations.relation[1].kline_little_irrep) == 2
        assert relations.relation[1].kline_little_irrep[0].label == "DT_{1}"
        assert relations.relation[1].kline_little_irrep[0].dimension == 1
        assert relations.relation[1].kline_little_irrep[1].label == "DT_{4}"
        assert relations.relation[1].kline_little_irrep[1].dimension == 1

        assert relations.relation[2].kpoint_little_irrep.label == "X_{1}"
        assert relations.relation[2].kpoint_little_irrep.dimension == 2

        assert relations.relation[2].kline_kvector_type_label == "S"
        assert relations.relation[2].kline_coordinates == "(u,1/2,u)"

        assert len(relations.relation[2].kline_little_irrep) == 2
        assert relations.relation[2].kline_little_irrep[0].label == "S_{1}"
        assert relations.relation[2].kline_little_irrep[0].dimension == 1
        assert relations.relation[2].kline_little_irrep[1].label == "S_{2}"
        assert relations.relation[2].kline_little_irrep[1].dimension == 1


if __name__ == "__main__":
    unittest.main()
