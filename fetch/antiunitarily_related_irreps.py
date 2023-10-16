from magnon.fetch.utility.cached_requests import cached_post
from magnon.fetch.utility.scrape_utility import contents_as_str, cleanup_ebr_html
from magnon.fetch.utility.br import LittleIrrep
from magnon.fetch.utility.kvector import KVector
from magnon.groups.magnetic_space_group_pb2 import (
    AntiunitarilyRelatedIrrepPair as AntiunitarilyRelatedIrrepPairProto,
    AntiunitarilyRelatedIrrepPairs as AntiunitarilyRelatedIrrepPairsProto,
)

from bs4 import BeautifulSoup as bs


def _mbandpaths_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mbandpaths.pl",
        data={"super": group_number},
    )


def _html_has_antiunitrels_table_method1(html):
    count = html.count("Constrains due to the antiunitary symmetry operations")

    assert count in (0, 1)
    return count == 1


def _html_has_antiunitrels_table_method2(html):
    count = html.count(
        r"""<td bgcolor="#d5d5d5" style="min-width:50px" height="40">"""
        r"""Maximal k-vector</td><td bgcolor="#d5d5d5" """
        r"""style="min-width:50px" height="40">Maximal k-vector"""
        r"""</td><td bgcolor="#d5d5d5" style="min-width:50px" """
        r"""height="40">Connections</td>"""
    )

    assert count in (0, 1)
    return count == 1


def _html_has_antiunitrels_table(html):
    result1 = _html_has_antiunitrels_table_method1(html)
    result2 = _html_has_antiunitrels_table_method2(html)
    assert result1 == result2
    return result1


def _antiunit_related_irreps_impl(group_number):
    """When two k-vectors are related only by an anti-unitary symmetry (e.g. time reversal combined
    with inversion), BCS lists them as two k-vector types, as if they are unrelated by symemtry.
    In particular, the EBR tables will show irreps for each of these k-vectors, which might give
    the wrong impression that the energetics of these irreps will be independent of each other.

    In our framework, we need to be careful about the constraint on the energetics of these
    anti-unitarily related k-vector types.

    For a given MSG number, this function returns a list of all k-vector pairs related purely
    by an anti-unitary symmetry. Additionally, for each such k-vector pair, this function returns
    the related irrep pairs.
    """
    result = []

    html = _mbandpaths_html(group_number)
    if not _html_has_antiunitrels_table(html):
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


def fetch_antiunitarily_related_irreps(msg_number):
    pairs = AntiunitarilyRelatedIrrepPairsProto()
    for k1_label, k2_label, irrep_pairs in _antiunit_related_irreps_impl(msg_number):
        for irrep1_label, irrep2_label in irrep_pairs:
            pair = pairs.pair.add()
            pair.first_kstar.label = k1_label
            pair.first_little_irrep_label = irrep1_label
            pair.second_kstar.label = k2_label
            pair.second_little_irrep_label = irrep2_label
    return pairs
