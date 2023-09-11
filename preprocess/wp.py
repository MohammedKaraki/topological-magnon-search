from magnon.preprocess.cached_requests import cached_post
from magnon.preprocess.utility import contents_as_str
from bs4 import BeautifulSoup as bs
import numpy as np
from re import fullmatch

from magnon.preprocess import log

logger = log.create_logger(__name__)


def wp_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/nph-magwplist",
        data={"gnum": group_number, "list": "Standard/Default Setting"},
        cache_filename=rf"wp-{group_number}.html",
    )


def parse_wp_html(group_number):
    html = wp_html(group_number)
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


def fetch_wp(group_number, wp):
    return parse_wp_html(group_number)[wp]


def main():
    print(fetch_wp("138.521", "8i"))


if __name__ == "__main__":
    main()
