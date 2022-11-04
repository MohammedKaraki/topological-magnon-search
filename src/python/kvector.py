from re import fullmatch
import numpy as np
from fractions import Fraction

import log
logger = log.create_logger(__name__)


def klabel_to_symbol_and_coords(klabel):
    KLABEL_PATTERN = r'([A-Z]+):\(([^,]+),([^,]+),([^,]+)\)'

    m = fullmatch(KLABEL_PATTERN, klabel)
    assert m is not None

    symbol = m.groups()[0]
    coords = np.array([Fraction(x) for x in m.groups()[1:]])
    assert coords.shape == (3,)

    return symbol, coords

class KVector:

    def __init__(self, klabel):
        self._symbol, self._coords = klabel_to_symbol_and_coords(klabel)

        assert klabel == str(self)

    @property
    def symbol(self):
        return self._symbol

    @property
    def coords(self):
        return self._coords

    def __eq__(self, other):
        assert len(self.__dict__) == 2

        result1 = self.symbol == other.symbol
        result2 = np.all(self.coords == other.coords)
        assert result1 == result2

        return result1

    def __hash__(self):
        assert len(self.__dict__) == 2

        return hash(self.symbol)

    def __repr__(self):
        return str(self)

    def __str__(self):
        return "{}:({},{},{})".format(self.symbol,
                                      *self.coords)

    def as_tuple(self):
        return (self.symbol,
                "(" + ",".join([str(x) for x in self.coords]) + ")")
