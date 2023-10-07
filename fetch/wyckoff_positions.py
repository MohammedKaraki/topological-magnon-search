from magnon.fetch.utility.cached_requests import cached_post
from magnon.fetch.utility.scrape_utility import contents_as_str
from magnon.groups.atomic_orbital_pb2 import WyckoffPosition as WyckoffPositionProto
from bs4 import BeautifulSoup as bs


#
# TODO: Allow this to be passed as a config.
#
_CACHE_DIR = "/tmp"


def _wp_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/nph-magwplist",
        data={"gnum": group_number, "list": "Standard/Default Setting"},
        cache_dir=_CACHE_DIR,
    )


def _parse_wp_html(group_number):
    html = _wp_html(group_number)
    soup = bs(html, "html5lib")

    tables = soup.findAll("table", attrs={"border": "5"})
    table = tables[1]
    assert "Multiplicity" in table.tbody.tr.text, table.tbody.tr.text

    wp_to_coords = {}
    for tr in table.tbody.findAll("tr", recursive=False):
        tds = tr.findAll("td", recursive=False)
        if len(tds) == 3:
            wp = contents_as_str(tds[0]) + contents_as_str(tds[1])
            wp_to_coords[wp] = []
            for td in tds[2].findAll("td"):
                wp_to_coords[wp].append(
                    "".join(contents_as_str(td.a).split())
                    .replace(r"<sub>", "_{")
                    .replace(r"</sub>", "}")
                    .replace(r"</sub>", "}")
                    .replace(r"|", "),(")
                )
    return wp_to_coords


def fetch_wyckoff_positions(group_number):
    result = []
    for wp_label, orbit_coords in _parse_wp_html(group_number).items():
        wyckoff_position = WyckoffPositionProto(label=wp_label)

        for coords in orbit_coords:
            wyckoff_position.orbit.coordinates.append(coords)
        result.append(wyckoff_position)

    return result
