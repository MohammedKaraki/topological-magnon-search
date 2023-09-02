from topomagnons.diagnose.preprocess.cached_requests import cached_post
from topomagnons.diagnose.preprocess.utility import contents_as_str
from bs4 import BeautifulSoup as bs
from topomagnons.diagnose.preprocess.kvector import KVector
from re import fullmatch
from topomagnons.diagnose.preprocess.utility import cleanup_ebr_html
from topomagnons.diagnose.preprocess.br import LittleIrrep

import log

logger = log.create_logger(__name__)


def mbandpaths_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mbandpaths.pl",
        data={"super": group_number},
        cache_filename=rf"mbandpathsc-{group_number}.html",
    )


def html_has_antiunitrels_table_method1(html):
    count = html.count("Constrains due to the antiunitary symmetry operations")

    assert count in (0, 1)
    return count == 1


def html_has_antiunitrels_table_method2(html):
    count = html.count(
        r"""<td bgcolor="#d5d5d5" style="min-width:50px" height="40">"""
        r"""Maximal k-vector</td><td bgcolor="#d5d5d5" """
        r"""style="min-width:50px" height="40">Maximal k-vector"""
        r"""</td><td bgcolor="#d5d5d5" style="min-width:50px" """
        r"""height="40">Connections</td>"""
    )

    assert count in (0, 1)
    return count == 1


def html_has_antiunitrels_table(html):
    result1 = html_has_antiunitrels_table_method1(html)
    result2 = html_has_antiunitrels_table_method2(html)
    assert result1 == result2
    return result1


def antiunit_related_irreps(group_number):
    result = []

    html = mbandpaths_html(group_number)
    if not html_has_antiunitrels_table(html):
        return result

    soup = bs(html, "html5lib")

    tables = soup.findAll(
        "table", attrs={"frame": "box", "rules": "all", "align": "center"}
    )

    num_relevant_tables_found = 0
    for table in tables:
        if len(table.tbody.tr.findAll("td", recursive=False)) == 3:
            num_relevant_tables_found += 1

            trs = table.tbody.findAll("tr", recursive=False)
            for tr in trs[1:]:
                tds = tr.findAll("td", recursive=False)
                assert len(tds) == 3
                kvec1 = KVector(contents_as_str(tds[0]).strip())
                kvec2 = KVector(contents_as_str(tds[1]).strip())
                rels_raw = [
                    [LittleIrrep(y.strip() + "(77)") for y in x.split(r"↔")]
                    for x in cleanup_ebr_html(contents_as_str(tds[2])).split(r"<br/>")
                    if "[overline]" not in x
                ]
                rels = []
                for irrep1, irrep2 in rels_raw:
                    assert irrep1.ksymbol == kvec1.symbol
                    assert irrep2.ksymbol == kvec2.symbol
                    rels.append([irrep1.label, irrep2.label])

                result.append((kvec1.symbol, kvec2.symbol, rels))

    assert num_relevant_tables_found == 1

    return result


def test():
    from msg_info_table import MSG_INFO_TABLE

    for group_number in MSG_INFO_TABLE:
        k1k2irreps_list = antiunit_related_irreps(group_number)
        if len(k1k2irreps_list) != 0:
            print(group_number)
            for ksymbol1, ksymbol2, rels in k1k2irreps_list:
                print("\t", ksymbol1, ksymbol2, ": ")
                for irrep1, irrep2 in rels:
                    print("\t", irrep1, "\t", irrep2)
                print()
            print()


if __name__ == "__main__":
    test()
