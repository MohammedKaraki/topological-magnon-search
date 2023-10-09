from magnon.fetch.utility.cached_requests import cached_post
from magnon.groups.atomic_orbital_pb2 import Irrep as IrrepProto
from magnon.groups.compatibility_relations_pb2 import (
    CompatibilityRelation as CompatibilityRelationProto,
)

from bs4 import BeautifulSoup as bs
from re import fullmatch, findall
from magnon.fetch.utility.br import LittleIrrep


#
# TODO: Allow this to be passed as a config.
#
_CACHE_DIR = "/tmp"


def _comp_rels_html(group_number, ksymbol):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mcomprel.pl",
        data={
            "super": group_number,
            "symbol": "",
            "vecfinal": "{}&".format(ksymbol),
            "list": "Submit",
        },
        cache_dir=_CACHE_DIR,
    )


def _comp_rels_impl(group_number, ksymbol):
    soup = bs(_comp_rels_html(group_number, ksymbol), "html5lib")
    inputs = soup.findAll("input", attrs={"name": "rf"})
    assert len(inputs) == 1
    input = inputs[0]
    src = str(input.attrs["value"])
    assert src.startswith(ksymbol)
    assert src[-1] == ")"

    klines = findall(r"((?:[A-Z]+)\$\([^)]+\))zzz", src)
    klines = [x.replace("$", ":") for x in klines]
    assert len(klines) >= 1

    def klinesymbol_to_kline(symbol):
        result = None
        for kline in klines:
            if kline.startswith(symbol + ":"):
                assert result is None
                result = kline
        assert result is not None
        return result

    lines = src.replace("%", "\n").replace("&", "\n").replace("<br>", "\n").split("\n")

    result = []

    for line in lines:
        num_commas, num_flecha, num_gggg, num_hhhh = (
            line.count(","),
            line.count("flecha"),
            line.count("gggg"),
            line.count("hhhh"),
        )

        assert (num_commas == 4) != (num_flecha == 1)
        assert num_commas in (0, 4)
        assert num_flecha in (0, 1)
        assert num_gggg == 0 or num_gggg >= 2
        assert num_gggg == num_hhhh

        if num_flecha == 1 and num_gggg == 0:
            rel = (
                line.replace("<sub>", "_{")
                .replace("</sub>", "}")
                .replace("<sup>", "^{")
                .replace("</sup>", "}")
                .replace("flecha", "[to]")
                .replace("oplus", "[oplus]")
            )
            lhs, rhs = rel.split("[to]")
            lhs = LittleIrrep(lhs)

            rhs_irreps = []
            for x in rhs.split("[oplus]"):
                m = fullmatch("([0-9]+)?([(A-Z].+)", x)
                assert m is not None, x
                count = 1

                count_str, label = m.groups()
                if count_str is not None:
                    count = int(count_str)
                    assert count >= 2

                rhs_irreps.extend([LittleIrrep(label)] * count)

            lhs_dim = lhs.dim
            rhs_dim = sum([x.dim for x in rhs_irreps])

            assert lhs_dim >= 1
            assert lhs_dim == rhs_dim

            rhs_ksymbol_list = list(
                set([klinesymbol_to_kline(x.ksymbol) for x in rhs_irreps])
            )
            assert len(rhs_ksymbol_list) == 1
            rhs_ksymbol = rhs_ksymbol_list[0]

            ktype_label, ktype_coords = rhs_ksymbol.split(":")

            result.append((lhs, ktype_label, ktype_coords, rhs_irreps))

    return result


def fetch_compatibility_relations(msg_number: str, kvector_star_label: str):
    relations = []
    for lhs_irrep, klabel, kcoords, rhs_irreps in _comp_rels_impl(
        msg_number, kvector_star_label
    ):
        relation = CompatibilityRelationProto()
        relation.decomposition.supergroup_irrep.CopyFrom(
            IrrepProto(label=lhs_irrep.label, dimension=lhs_irrep.dim)
        )
        relation.point_kvector.star.label = kvector_star_label
        relation.line_kvector.star.label = klabel
        relation.line_kvector.coordinates = kcoords
        for rhs_irrep in rhs_irreps:
            relation.decomposition.subgroup_irrep.append(
                IrrepProto(label=rhs_irrep.label, dimension=rhs_irrep.dim)
            )
        relations.append(relation)
    return relations
