from re import fullmatch
from itertools import groupby
from magnon.fetch.utility.band import Band

from magnon.common.logger import create_logger

logger = create_logger(__name__)


class LittleIrrep:
    IRREP_PATTERN_BLOCK = r"([A-Z]+)(_\{[0-9]+\})(\^\{[+-]\})?"
    IRREP_PATTERN = r"{0}({0})?\(([0-9]+)\)".format(IRREP_PATTERN_BLOCK)

    def __init__(self, irreplabel_with_dim):
        def extract_ksymbol_irreplabel_and_dim(s):
            m = fullmatch(
                r"\(([A-Z]+)\)([A-Z]+)(_\{[0-9]+\}(\^\{[-+]\})?)\(([0-9]+)\)", s
            )
            if m:
                k1symbol, k2symbol, irrepsubsuper, _, dim = m.groups()
                irreplabel = r"{}{}".format(k1symbol, irrepsubsuper)
                dim = int(dim)
                assert dim >= 1
                return k1symbol, irreplabel, dim

            m = fullmatch(
                r"\(([A-Z]+)\)([A-Z]+)(_\{[0-9]+\}(\^\{[-+]\})?)"
                + r"([A-Z]+)(_\{[0-9]+\}(\^\{[-+]\})?)\(([0-9]+)\)",
                s,
            )
            if m:
                (
                    k1symbol,
                    k2symbol,
                    irrepsubsuper1,
                    _,
                    k3symbol,
                    irrepsubsuper2,
                    _,
                    dim,
                ) = m.groups()
                assert k2symbol == k3symbol
                irreplabel = r"{}{}{}{}".format(
                    k1symbol, irrepsubsuper1, k1symbol, irrepsubsuper2
                )
                dim = int(dim)
                assert dim >= 1
                return k1symbol, irreplabel, dim

            m = fullmatch(r"([A-Z]+)(_\{[0-9]+\}(\^\{[-+]\})?)\(([0-9]+)\)", s)
            if m:
                ksymbol, irrepsubsuper, _, dim = m.groups()
                irreplabel = r"{}{}".format(ksymbol, irrepsubsuper)
                dim = int(dim)
                assert dim >= 1
                return ksymbol, irreplabel, dim

            m = fullmatch(
                r"([A-Z]+)(_\{[0-9]+\}(\^\{[-+]\})?)"
                + r"([A-Z]+)(_\{[0-9]+\}(\^\{[-+]\})?)\(([0-9]+)\)",
                s,
            )
            if m:
                (
                    k1symbol,
                    irrepsubsuper1,
                    _,
                    k2symbol,
                    irrepsubsuper2,
                    _,
                    dim,
                ) = m.groups()
                assert k1symbol == k2symbol
                irreplabel = r"{}{}{}{}".format(
                    k1symbol, irrepsubsuper1, k1symbol, irrepsubsuper2
                )
                dim = int(dim)
                assert dim >= 1
                return k1symbol, irreplabel, dim

            assert False, "'" + s + "'"

        if "[overline]" in irreplabel_with_dim:
            self._ksymbol, self._label, self._dim = [
                "DOUBLE-VALUED IRREP UNSUPPORTED"
            ] * 3
            return

        self._ksymbol, self._label, self._dim = extract_ksymbol_irreplabel_and_dim(
            irreplabel_with_dim
        )
        assert irreplabel_with_dim.startswith(self.ksymbol) or (
            irreplabel_with_dim.startswith("(")
            and irreplabel_with_dim[1:].startswith(self.ksymbol)
        )
        assert isinstance(self.dim, int)
        assert self.dim >= 1

    @property
    def ksymbol(self):
        return self._ksymbol

    @property
    def label(self):
        return self._label

    def as_str(self):
        return "{}({})".format(self.label, self.dim)

    @property
    def dim(self):
        return self._dim

    def __lt__(self, other):
        lt = self.label < other.label
        gt = self.label > other.label
        if lt == gt:
            assert lt == False
            assert self == other

        return lt

    def __eq__(self, other):
        assert isinstance(other, LittleIrrep)

        if self.label == other.label:
            assert self.ksymbol == other.ksymbol
            assert self.dim == other.dim

        return (self.ksymbol, self.label, self.dim) == (
            other.ksymbol,
            other.label,
            other.dim,
        )

    def __hash__(self):
        return hash(tuple(sorted(self.__dict__.items())))

    def __repr__(self):
        return self.as_str()
        # return r'({}, dim: {}, k: {})'.format(
        #     self.label, self.dim, self.ksymbol)

    def as_tuple(self):
        return (self.ksymbol, self.label, self.dim)

    def __str__(self):
        return self.__repr__()


class Br:
    def __init__(self, irreps):
        assert all(isinstance(x, LittleIrrep) for x in irreps), irreps
        irreps.sort()
        assert irreps == sorted(irreps), irreps
        self._irreps = irreps

    @property
    def irreps(self):
        assert self._irreps == sorted(self._irreps)
        return self._irreps

    def as_band(self):
        result = []
        for _, g in groupby(self.irreps, key=lambda irrep: irrep.ksymbol):
            result.append(list(g))

        return Band(result)

    def __add__(self, rhs):
        return Br(self.irreps + rhs.irreps)

    def __radd__(self, rhs):
        assert rhs == 0
        return Br(self.irreps)

    def __eq__(self, other):
        assert self.irreps == sorted(self.irreps)
        assert other.irreps == sorted(other.irreps)

        return self.irreps == other.irreps

    def __hash__(self):
        assert False

    def __str__(self):
        return " [oplus] ".join(x.label for x in self.irreps)

    def __repr__(self):
        return str(self)
