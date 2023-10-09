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
        print(relations)
        assert len(relations) == 1
        assert relations[0].decomposition.supergroup_irrep.label == "GM_{1}"
        assert relations[0].decomposition.supergroup_irrep.dimension == 1

        assert relations[0].line_kvector.star.label == "GP"
        assert relations[0].line_kvector.coordinates == "(u,v,w)"

        assert len(relations[0].decomposition.subgroup_irrep) == 1
        assert relations[0].decomposition.subgroup_irrep[0].label == "GP_{1}"
        assert relations[0].decomposition.subgroup_irrep[0].dimension == 1

    def test_166_99_f(self):
        relations = fetch_compatibility_relations("166.99", "F")
        assert len(relations) == 6

        assert relations[0].decomposition.supergroup_irrep.label == "F_{1}"
        assert relations[0].decomposition.supergroup_irrep.dimension == 1

        assert relations[0].line_kvector.star.label == "SM"
        assert relations[0].line_kvector.coordinates == "(1-2*u,u,1)"

        assert len(relations[0].decomposition.subgroup_irrep) == 1
        assert relations[0].decomposition.subgroup_irrep[0].label == "SM_{1}"
        assert relations[0].decomposition.subgroup_irrep[0].dimension == 1

        assert relations[1].decomposition.supergroup_irrep.label == "F_{2}"
        assert relations[1].decomposition.supergroup_irrep.dimension == 1

        assert relations[1].line_kvector.star.label == "SM"
        assert relations[1].line_kvector.coordinates == "(1-2*u,u,1)"

        assert len(relations[1].decomposition.subgroup_irrep) == 1
        assert relations[1].decomposition.subgroup_irrep[0].label == "SM_{1}"
        assert relations[1].decomposition.subgroup_irrep[0].dimension == 1

    def test_205_33_x(self):
        relations = fetch_compatibility_relations("205.33", "X")
        assert len(relations) == 16

        assert relations[0].decomposition.supergroup_irrep.label == "X_{1}"
        assert relations[0].decomposition.supergroup_irrep.dimension == 2

        assert relations[0].line_kvector.star.label == "DT"
        assert relations[0].line_kvector.coordinates == "(0,v,0)"

        assert len(relations[0].decomposition.subgroup_irrep) == 2
        assert relations[0].decomposition.subgroup_irrep[0].label == "DT_{2}"
        assert relations[0].decomposition.subgroup_irrep[0].dimension == 1
        assert relations[0].decomposition.subgroup_irrep[1].label == "DT_{3}"
        assert relations[0].decomposition.subgroup_irrep[1].dimension == 1

        assert relations[1].decomposition.supergroup_irrep.label == "X_{2}"
        assert relations[1].decomposition.supergroup_irrep.dimension == 2

        assert relations[1].line_kvector.star.label == "DT"
        assert relations[1].line_kvector.coordinates == "(0,v,0)"

        assert len(relations[1].decomposition.subgroup_irrep) == 2
        assert relations[1].decomposition.subgroup_irrep[0].label == "DT_{1}"
        assert relations[1].decomposition.subgroup_irrep[0].dimension == 1
        assert relations[1].decomposition.subgroup_irrep[1].label == "DT_{4}"
        assert relations[1].decomposition.subgroup_irrep[1].dimension == 1

        assert relations[2].decomposition.supergroup_irrep.label == "X_{1}"
        assert relations[2].decomposition.supergroup_irrep.dimension == 2

        assert relations[2].line_kvector.star.label == "S"
        assert relations[2].line_kvector.coordinates == "(u,1/2,u)"

        assert len(relations[2].decomposition.subgroup_irrep) == 2
        assert relations[2].decomposition.subgroup_irrep[0].label == "S_{1}"
        assert relations[2].decomposition.subgroup_irrep[0].dimension == 1
        assert relations[2].decomposition.subgroup_irrep[1].label == "S_{2}"
        assert relations[2].decomposition.subgroup_irrep[1].dimension == 1


if __name__ == "__main__":
    unittest.main()
