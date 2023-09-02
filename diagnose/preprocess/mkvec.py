from topomagnons.diagnose.preprocess.cached_requests import cached_post
from topomagnons.diagnose.preprocess.utility import contents_as_str
from bs4 import BeautifulSoup as bs
import numpy as np
from re import fullmatch

import log

logger = log.create_logger(__name__)


KLABEL_PATTERN = r"([A-Z]+):\(([^,]+),([^,]+),([^,]+)\)"


def symbol_and_vec3_from_klabel(klabel):
    m = fullmatch(KLABEL_PATTERN, klabel)
    assert m is not None

    symbol = m.groups()[0]
    vec3 = np.array([float(eval(k)) for k in m.groups()[1:4]])

    return symbol, vec3


def symbol_from_klabel(klabel):
    symbol, _ = symbol_and_vec3_from_klabel(klabel)
    return symbol


def vec3_from_klabel(klabel):
    logger.debug(klabel)
    logger.debug(type(klabel))
    _, vec3 = symbol_and_vec3_from_klabel(klabel)
    return vec3


def mkvec_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mkvec.pl",
        data={"super": group_number, "list": "Submit"},
        cache_filename=rf"mkvec-{group_number}.html",
    )


def mkvec_parsed(group_number):
    html = mkvec_html(group_number)
    soup = bs(html, "html5lib")

    tables = soup.findAll(
        "table", attrs={"frame": "box", "rules": "all", "align": "center"}
    )

    if len(tables) == 1:
        table = tables[0]

        result = []
        for tr in table.tbody.findAll("tr", recursive=False)[1:]:
            tds = tr.findAll("td", recursive=False)
            assert len(tds) == 6, tds
            result.append(
                {
                    "label": contents_as_str(tds[0]),
                    "star": contents_as_str(tds[1]),
                    "cogroup": contents_as_str(tds[2]),
                }
            )

        assert len(result) > 4
        assert result[0]["label"] == "GM"

        return result

    else:
        assert len(tables) == 2
        generalk_table = tables[0]
        specialk_table = tables[1]

        result = []

        generalk_to_cogroup = {}
        for tr in generalk_table.tbody.findAll("tr", recursive=False)[1:]:
            tds = tr.findAll("td", recursive=False)
            assert len(tds) == 6, tds
            result.append(
                {
                    "label": contents_as_str(tds[0]),
                    "star": contents_as_str(tds[1]),
                    "cogroup": contents_as_str(tds[2]),
                }
            )

            generalk_to_cogroup[contents_as_str(tds[0])] = contents_as_str(tds[2])

        for tr in specialk_table.tbody.findAll("tr", recursive=False)[1:]:
            tds = tr.findAll("td", recursive=False)
            assert len(tds) == 8, tds
            new_entry = {
                "label": contents_as_str(tds[0]),
                "star": contents_as_str(tds[1]),
                "cogroup": generalk_to_cogroup[contents_as_str(tds[2])],
            }

            assert new_entry not in result
            result.append(new_entry)

        assert len(result) > 4
        assert any(x["label"] == "GM" for x in result)

        return result
