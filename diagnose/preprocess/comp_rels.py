from topomagnons.diagnose.preprocess.cached_requests import cached_post
from bs4 import BeautifulSoup as bs
from re import fullmatch, findall
from topomagnons.diagnose.preprocess.br import LittleIrrep

import log

logger = log.create_logger(__name__)


def comp_rels_html(group_number, ksymbol):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mcomprel.pl",
        data={
            "super": group_number,
            "symbol": "",
            "vecfinal": "{}&".format(ksymbol),
            "list": "Submit",
        },
        cache_filename=rf"comp_rels-{group_number}-{ksymbol}.html",
    )


def dim_from_label(label):
    m = fullmatch(r"[(A-Z].+\(([0-9]+)\)", label)
    assert m is not None
    dim = int(m.groups()[0])
    assert dim >= 1
    return dim


def comp_rels(group_number, ksymbol):
    soup = bs(comp_rels_html(group_number, ksymbol), "html5lib")
    inputs = soup.findAll("input", attrs={"name": "rf"})
    assert len(inputs) == 1
    input = inputs[0]
    src = str(input.attrs["value"])
    assert src.startswith(ksymbol)
    assert src[-1] == ")"

    klines = findall(r"((?:[A-Z]+)\$\([^)]+\))zzz", src)
    klines = [x.replace("$", ":") for x in klines]
    # logger.debug(klines)
    assert len(klines) >= 1

    def klinesymbol_to_kline(symbol):
        result = None
        for kline in klines:
            if kline.startswith(symbol + ":"):
                assert result is None
                # if result is not None:
                # logger.error((symbol, klines))
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

            result.append((lhs, rhs_ksymbol, rhs_irreps))

    # logger.debug((ksymbol, result))

    return result
