from magnon.fetch.utility.cached_requests import cached_post
from magnon.fetch.utility.general_position_utility import (
    UnitaryGenpos,
    unitary_gstr_to_mat4x4,
)
from magnon.fetch.utility.scrape_utility import (
    contents_as_str,
    find_unique_subtag,
    cleanup_genpos_html,
)
from magnon.groups.magnetic_space_group_pb2 import (
    GeneralPosition as GeneralPositionProto,
    GeneralPositions as GeneralPositionsProto,
)

from bs4 import BeautifulSoup as bs
import numpy as np
from fractions import Fraction
from re import fullmatch, findall


def _genpos_html(msg_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/nph-magtrgen",
        data={"gnum": msg_number, "list": "Standard/Default Setting"},
    )


def _fetch_gdicts(msg_number):
    html = _genpos_html(msg_number)
    soup = bs(html, "html5lib")

    table = find_unique_subtag(soup, "table", attrs={"border": "5"})

    unitary_entries = []
    antiunitary_entries = []
    for tr in table.tbody.findAll("tr", recursive=False)[2:]:
        tds = tr.findAll("td", recursive=False)
        assert len(tds) == 5, len(tds)

        genpos_str = cleanup_genpos_html(
            contents_as_str(tds[1]).split(r"<br/>")[0].replace(" ", "")
        )

        unitarity_sign = genpos_str[-2:]
        assert unitarity_sign in ["+1", "-1"]

        mat3x4str = cleanup_genpos_html(contents_as_str(tds[2].table.tbody.tr.pre))

        mat3x4 = None
        if unitarity_sign == "+1":
            mat3x4 = np.reshape(
                [Fraction(x) for x in mat3x4str.split()], newshape=(3, 4)
            )
        else:
            assert unitarity_sign == "-1"

        new_entry = {
            "counter": cleanup_genpos_html(contents_as_str(tds[0])),
            "str": genpos_str,
            "mat3x4": mat3x4,
            "interpretation": cleanup_genpos_html(contents_as_str(tds[3])),
            "seitz": cleanup_genpos_html(contents_as_str(tds[4])).replace(" ", ""),
        }

        def sanity_check():
            test_mat = unitary_gstr_to_mat4x4(new_entry["str"])[:3, :]
            assert np.all(test_mat == new_entry["mat3x4"]), (
                test_mat,
                new_entry["mat3x4"],
            )

        if unitarity_sign == "+1":
            sanity_check()

        if unitarity_sign == "+1":
            unitary_entries.append(new_entry)
        elif unitarity_sign == "-1":
            antiunitary_entries.append(new_entry)
        else:
            assert False

    assert len(unitary_entries) >= 1
    assert unitary_entries[0]["str"] == "x,y,z,+1"

    return {"unitary": unitary_entries, "antiunitary": antiunitary_entries}


def _fetch_unitary_gs_impl(msg_number):
    unitary_gdicts = _fetch_gdicts(msg_number)["unitary"]
    assert len(unitary_gdicts) >= 1
    assert unitary_gdicts[0]["str"] == "x,y,z,+1"

    return [
        UnitaryGenpos.from_gstr(
            gdict["str"], cross_check_mat3x4=gdict["mat3x4"], seitz=gdict["seitz"]
        )
        for gdict in unitary_gdicts
    ]


def fetch_unitary_general_positions(msg_number):
    general_positions = GeneralPositionsProto()
    for unitary_g in _fetch_unitary_gs_impl(msg_number):
        general_positions.general_position.append(
            GeneralPositionProto(
                coordinates_form=unitary_g.gstr, seitz_form=unitary_g.seitz
            )
        )
    return general_positions
