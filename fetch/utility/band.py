from itertools import accumulate

from magnon.utils.logger import create_logger

logger = create_logger(__name__)


class Band:
    def __init__(self, modes_lists):
        def modes_list_dim(modes_list):
            return sum(irrep.dim for irrep in modes_list)

        self._dim = modes_list_dim(modes_lists[0])
        assert all(
            [self.dim == modes_list_dim(modes_list) for modes_list in modes_lists]
        )

        self._modes_lists = modes_lists

    @property
    def dim(self):
        return self._dim

    @property
    def modes_lists(self):
        return self._modes_lists

    def shuffle(self, seed):
        import random

        for idx, modes_list in enumerate(self.modes_lists):
            random.Random(seed + idx).shuffle(modes_list)

    @property
    def all_irreps(self):
        return [mode for modes_list in self.modes_lists for mode in modes_list]

    def __add__(self, rhs):
        modes_lists = [a + b for a, b in zip(self.modes_lists, rhs.modes_lists)]
        return Band(modes_lists)

    def __radd__(self, lhs):
        assert lhs == 0
        return self

    @property
    def dims_and_modeaccums_lists(self):
        dims_lists = []
        accums_lists = []
        for modes_list in self.modes_lists:
            dims_list = [mode.dim for mode in modes_list]
            accums_list = list(accumulate(dims_list))

            dims_lists.append(dims_list)
            accums_lists.append(accums_list)

        return dims_lists, accums_lists

    @property
    def sub_bands(self):
        dims_lists, accums_lists = self.dims_and_modeaccums_lists

        accums_intersect = set(accums_lists[0])
        for accums_list in accums_lists[1:]:
            accums_intersect = accums_intersect.intersection(list(accums_list))
        accums_intersect = sorted(list(accums_intersect))

        subbands_parts = []
        for modes_list, accums_list in zip(self.modes_lists, accums_lists):
            bounds = [0] + [1 + accums_list.index(x) for x in accums_intersect]
            subbands_parts.append([modes_list[a:b] for a, b in zip(bounds, bounds[1:])])

        result = list(map(Band, zip(*subbands_parts)))
        assert sum(result) == self
        return result

    def __eq__(self, rhs):
        assert isinstance(rhs, Band)
        return all(l == r for l, r in zip(self.modes_lists, rhs.modes_lists))

    def __repr__(self):
        return " - ".join(
            [
                r" \oplus{} ".join([str(irrep.label) for irrep in modes_list])
                for modes_list in self.modes_lists
            ]
        )
