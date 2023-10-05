from re import fullmatch

import numpy as np
from bs4 import BeautifulSoup as bs

from magnon.groups.magnetic_kvector_type_pb2 import (
    MagneticKVectorType,
    MagneticKVectorTypes,
)
from magnon.fetch.cached_requests import cached_post
from magnon.fetch.utility.scrape_utility import contents_as_str
from magnon.common.logger import create_logger

logger = create_logger(__name__)


_CACHE_DIR = "/tmp"

_KLABEL_PATTERN = r"([A-Z]+):\(([^,]+),([^,]+),([^,]+)\)"


def _symbol_and_vec3_from_klabel(klabel):
    m = fullmatch(_KLABEL_PATTERN, klabel)
    assert m is not None

    symbol = m.groups()[0]
    vec3 = np.array([float(eval(k)) for k in m.groups()[1:4]])

    return symbol, vec3


def _symbol_from_klabel(klabel):
    symbol, _ = _symbol_and_vec3_from_klabel(klabel)
    return symbol


def _vec3_from_klabel(klabel):
    logger.debug(klabel)
    logger.debug(type(klabel))
    _, vec3 = _symbol_and_vec3_from_klabel(klabel)
    return vec3


def _mkvec_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mkvec.pl",
        data={"super": group_number, "list": "Submit"},
        cache_dir=_CACHE_DIR,
    )


def fetch_magnetic_kvector_types(group_number):
    """Given a magnetic space group number (e.g. "205.33"), returns a message with a list of
    magnetic k-vector types (which include the type labels, star coordinates and the little co-group
    label).
    """

    magnetic_kvector_types = MagneticKVectorTypes()
    magnetic_kvector_types.magnetic_space_group.number = group_number

    html = _mkvec_html(group_number)
    soup = bs(html, "html5lib")

    tables = soup.findAll(
        "table", attrs={"frame": "box", "rules": "all", "align": "center"}
    )

    # The BCS webpage can be in one of two formats: either all data is in one table, or in two.
    # So we need to handle both cases.

    def handle_page_with_one_table():
        table = tables[0]

        result = []
        for tr in table.tbody.findAll("tr", recursive=False)[1:]:
            tds = tr.findAll("td", recursive=False)
            assert len(tds) == 6, tds

            kvector_type = MagneticKVectorType()
            kvector_type.label = contents_as_str(tds[0])
            kvector_type.star_coordinates = contents_as_str(tds[1])
            kvector_type.little_cogroup_label = contents_as_str(tds[2])
            magnetic_kvector_types.type.append(kvector_type)

    def handle_page_with_two_tables():
        generalk_table = tables[0]
        specialk_table = tables[1]

        generalk_to_cogroup = {}
        for tr in generalk_table.tbody.findAll("tr", recursive=False)[1:]:
            tds = tr.findAll("td", recursive=False)
            assert len(tds) == 6, tds

            kvector_type = MagneticKVectorType()
            kvector_type.label = contents_as_str(tds[0])
            kvector_type.star_coordinates = contents_as_str(tds[1])
            kvector_type.little_cogroup_label = contents_as_str(tds[2])
            magnetic_kvector_types.type.append(kvector_type)

            generalk_to_cogroup[kvector_type.label] = kvector_type.little_cogroup_label

        for tr in specialk_table.tbody.findAll("tr", recursive=False)[1:]:
            tds = tr.findAll("td", recursive=False)
            assert len(tds) == 8, tds
            kvector_type = MagneticKVectorType()
            kvector_type.label = contents_as_str(tds[0])
            kvector_type.star_coordinates = contents_as_str(tds[1])
            kvector_type.little_cogroup_label = generalk_to_cogroup[
                contents_as_str(tds[2])
            ]

            assert kvector_type not in magnetic_kvector_types.type
            magnetic_kvector_types.type.append(kvector_type)

    if len(tables) == 1:
        handle_page_with_one_table()
    elif len(tables) == 2:
        handle_page_with_two_tables()
    else:
        assert (
            False
        ), "Unrecognized k-vector types webpage format with more than 2 tables!"

    assert (
        len(magnetic_kvector_types.type) >= 4
    ), "No MSG has less than 4 k-vector types!"
    assert magnetic_kvector_types.type[0].label == "GM"
    return magnetic_kvector_types
