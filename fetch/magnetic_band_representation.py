from bs4 import BeautifulSoup as bs
from re import fullmatch, search
import numpy as np

from magnon.fetch.utility.scrape_utility import (
    contents_as_str,
    find_unique_subtag,
    cleanup_ebr_html,
    list_transpose,
    list_flatten_one_level,
    cleanup_pointgroup_html,
)
from magnon.fetch.utility.cached_requests import cached_post
from magnon.fetch.utility.kvector import KVector
from magnon.fetch.utility.br import LittleIrrep, Br
from magnon.common.logger import create_logger
from magnon.groups.atomic_orbital_pb2 import (
    WyckoffPosition as WyckoffPositionProto,
    AtomicOrbital as AtomicOrbitalProto,
    Irrep as IrrepProto,
)
from magnon.groups.magnetic_space_group_pb2 import (
    AtomicBandRepresentation as AtomicBandRepresentationProto,
)


logger = create_logger(__name__)


#
# TODO: Allow this to be passed as a config.
#
_CACHE_DIR = "/tmp"

IRREP_PATTERN = (
    r"((\([A-Z]+\))?(\[overline\])?[A-Z]+_\{[0-9]+\}(\^\{[-+]\})?){1,2}\([0-9]+\)"
)


def _ebrs_html(group_number):
    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mbandrep.pl",
        data={"super": group_number, "elementary": "Elementary"},
        cache_dir=_CACHE_DIR,
    )


def _matches_momentum_label(text):
    return fullmatch(r".+:\([^,]+,[^,]+,[^,]+\)$", text) is not None


def _parse_representation(text):
    irrep_content = []

    for x in text.split(r" [oplus] "):
        m = fullmatch(r"(([0-9]+) )?({})".format(IRREP_PATTERN), x)
        assert m is not None, x

        count = 1
        if m.groups()[1]:
            count = int(m.groups()[1])
            assert count > 1

        label = m.groups()[2]
        irrep_content.extend([label] * count)

    assert all(
        fullmatch(IRREP_PATTERN, x) is not None for x in irrep_content
    ), irrep_content
    return irrep_content


def _parse_tr(tr):
    tds = tr.findAll("td", recursive=False)
    assert len(tds) >= 2
    first_cell_text = tds[0].text

    row_type = None
    if first_cell_text in (
        "Wyckoff pos.",
        "Band-rep.",
        "Band-type",
    ) or first_cell_text.startswith("Decomposable"):
        row_type = "Header:" + first_cell_text
    else:
        assert _matches_momentum_label(first_cell_text), first_cell_text
        row_type = "Data"

    result = [row_type]

    for j, td in enumerate(tds):
        if row_type == "Data" and j == 1:
            assert _matches_momentum_label(td.text), td.text
            continue

        row_cell = cleanup_ebr_html(contents_as_str(td))
        assert row_cell.isascii()

        if row_type == "Data" and j > 1:
            row_cell = _parse_representation(row_cell)

        result.append(row_cell)

    return result


def _wyckoff_brs_html(group_number, wyckoff):
    from re import fullmatch

    m = fullmatch(r"([0-9]+)([a-z]+)", wyckoff)
    assert m is not None

    number, letter = m.groups()
    wyckoff_in_request = "{}&{}".format(number, letter)
    wyckoff_in_filename = "{}amp{}".format(number, letter)

    return cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/mbandrep.pl",
        data={"super": group_number, "wyck": "Wyckoff", "wyckoff": wyckoff_in_request},
        cache_dir=_CACHE_DIR,
    )


def _ebrs_parsed(group_number):
    html = _ebrs_html(group_number)
    soup = bs(html, "html5lib")

    table = find_unique_subtag(
        soup, "table", attrs={"frame": "box", "rules": "all", "align": "center"}
    )

    rows = []
    trs = table.tbody.findAll("tr", recrusive=False)
    gamma_cell_deleted = False
    for i, tr in enumerate(trs):
        rows.append(_parse_tr(tr))

    assert all(len(x) == len(rows[0]) for x in rows)

    return rows


def _wyckoff_brs_parsed(group_number, wyckoff):
    html = _wyckoff_brs_html(group_number, wyckoff)
    soup = bs(html, "html5lib")

    point_group = cleanup_pointgroup_html(
        search(
            r"Magnetic point group isomorphic to the site-symmetry group of the "
            "Wyckoff position: (.+)<br>\s+and unitary subgroup",
            html,
        ).group(1)
    ).replace("[overline]", "-")

    table = find_unique_subtag(
        soup, "table", attrs={"frame": "box", "rules": "all", "align": "center"}
    )

    rows = []
    trs = table.tbody.findAll("tr", recrusive=False)

    for i, tr in enumerate(trs):
        rows.append(_parse_tr(tr))

    assert all(len(x) == len(rows[0]) for x in rows)

    return point_group, rows


class _Wyckoff:
    def __init__(self, text):
        m = fullmatch(r"([0-9]+[a-z])\(([^,]+),[^,]+\)", text)
        assert m is not None
        self._label, self._sitesymmetry = m.groups()
        self._sitesymmetry = self._sitesymmetry.replace("[overline]", "-")

    @property
    def label(self):
        return self._label

    @property
    def sitesymmetry(self):
        return self._sitesymmetry

    def __eq__(self, other):
        if self.label == other.label:
            assert self.sitesymmetry == other.sitesymmetry
            return True

        return False

    def __hash__(self):
        return hash(tuple(sorted(self.__dict__.items())))

    def __str__(self):
        return "{}({})".format(self.label, self.sitesymmetry)

    def __repr__(self):
        return str(self)


class _WyckoffIrrep:
    def __init__(self, label):
        assert "uparrow" not in label
        self._label = label

    @property
    def label(self):
        return self._label

    def __eq__(self, other):
        return (self.label) == (other.label)

    def __hash__(self):
        return hash(tuple(sorted(self.__dict__.items())))

    def __str__(self):
        return "{}".format(self.label)

    def __repr__(self):
        return str(self)

    def is_real(self):
        if self.label[0] == "^":
            return False

        assert self.label[0] in ("A", "B", "E", "T"), self
        return True

    def tr_partner(self):
        if self.is_real():
            return self

        tr_dict_1 = {"^{1}E^{2}E": "^{1}E^{2}E"}
        if self.label in tr_dict_1:
            return _WyckoffIrrep(tr_dict_1[self.label])

        first, last = self.label[:4], self.label[4:]
        tr_dict_2 = {"^{1}": "^{2}", "^{2}": "^{1}"}
        return _WyckoffIrrep(tr_dict_2[first] + last)


def _remove_uparrowG(text):
    m = fullmatch(r"(.+)\[uparrow\].+", text)
    assert m is not None, text
    return m.groups()[0]


def _kvectors_and_wp_to_site_irrep_to_br_map(group_number):
    parsed = _ebrs_parsed(group_number)

    wyckoffs = [_Wyckoff(x) for x in parsed[0][2:]]
    wyckoffirreps = [_WyckoffIrrep(_remove_uparrowG(x)) for x in parsed[1][2:]]

    data_rows = [x for x in parsed if x[0] == "Data"]
    assert len(data_rows) > 1
    assert (
        data_rows[0][1] == "GM:(0,0,0)" or data_rows[0][1] == "A:(0,0,1/2)"
    ), data_rows[0][1]

    data_cols = list_transpose(data_rows)
    kvectors = [KVector(x) for x in data_cols[1]]
    ebrs_unflattened = data_cols[2:]
    assert kvectors[0].symbol == "GM" or kvectors[0].symbol == "A"
    assert np.all(kvectors[0].coords == np.array([0, 0, 0])) or np.all(
        kvectors[0].coords == np.array([0, 0, 0.5])
    )

    ebrs = [
        Br([LittleIrrep(y) for y in list_flatten_one_level(x)])
        for x in ebrs_unflattened
    ]
    assert all(type(ebr) is Br for ebr in ebrs), ebrs

    assert len(ebrs) == len(wyckoffs)
    assert len(ebrs) == len(wyckoffirreps)

    ebrs_dict = {}
    for wyckoff, wyckoffirrep, ebr in zip(wyckoffs, wyckoffirreps, ebrs):
        if "overline" in wyckoffirrep.label:
            continue

        if wyckoff not in ebrs_dict:
            ebrs_dict[wyckoff] = {}
        ebrs_dict[wyckoff][wyckoffirrep] = ebr

    return kvectors, ebrs_dict


def _point_group_and_site_irrep_to_br_map(group_number, wyckoff_label):
    point_group, rows = _wyckoff_brs_parsed(group_number, wyckoff_label)
    table_rows = [x for x in rows if x[0] in ("Data", "Header:Band-rep.")]
    assert len(table_rows) > 1
    assert table_rows[0][0] == "Header:Band-rep.", table_rows[0][0]
    assert (
        table_rows[1][1] == "GM:(0,0,0)" or table_rows[1][1] == "A:(0,0,1/2)"
    ), table_rows[1][1]
    assert table_rows[2][0] == "Data"

    table_cols = list_transpose(table_rows)

    kpoints = table_cols[1][1:]
    assert kpoints[0] == "GM:(0,0,0)" or kpoints[0] == "A:(0,0,1/2)"
    assert all(_matches_momentum_label(x) for x in kpoints)

    brs = {}
    for row in table_cols[2:]:
        key = _remove_uparrowG(row[0])
        assert key not in brs
        if "overline" in key:
            continue

        brs[key] = Br([LittleIrrep(label) for label in list_flatten_one_level(row[1:])])

    assert all(type(br) is Br for br in brs.values())

    return point_group, brs


def fetch_atomic_band_representations_for_wyckoff_position(
    msg_number, wyckoff_position_label
):
    point_group_label, site_irrep_to_br_map = _point_group_and_site_irrep_to_br_map(
        msg_number, wyckoff_position_label
    )

    wyckoff_position = WyckoffPositionProto()
    wyckoff_position.label = wyckoff_position_label
    wyckoff_position.site_symmetry_group_label = point_group_label

    atomic_band_representations = []
    for site_irrep, br in site_irrep_to_br_map.items():
        atomic_band_representation = AtomicBandRepresentationProto()
        atomic_band_representation.atomic_orbital.wyckoff_position.CopyFrom(
            wyckoff_position
        )
        atomic_band_representation.atomic_orbital.site_symmetry_irrep.label = site_irrep

        little_irreps = [
            IrrepProto(label=irrep.label, dimension=irrep.dim)
            for irrep in br.irreps
            if "DOUBLE-VALUED" not in irrep.label
        ]
        for irrep in little_irreps:
            atomic_band_representation.kspace_little_irrep.append(irrep)

        atomic_band_representations.append(atomic_band_representation)

    return atomic_band_representations


def fetch_elementary_band_representations_for_group(msg_number):
    _, wp_to_site_irrep_to_br_map = _kvectors_and_wp_to_site_irrep_to_br_map(msg_number)

    elementary_band_representations = []

    for (
        wyckoff_position_internal,
        site_irrep_to_br_map,
    ) in wp_to_site_irrep_to_br_map.items():
        wyckoff_position = WyckoffPositionProto()
        wyckoff_position.label = wyckoff_position_internal.label
        wyckoff_position.site_symmetry_group_label = (
            wyckoff_position_internal.sitesymmetry
        )

        for site_irrep, br in site_irrep_to_br_map.items():
            elementary_band_representation = AtomicBandRepresentationProto()
            elementary_band_representation.atomic_orbital.wyckoff_position.CopyFrom(
                wyckoff_position
            )
            elementary_band_representation.atomic_orbital.site_symmetry_irrep.label = (
                site_irrep.label
            )

            little_irreps = [
                IrrepProto(label=irrep.label, dimension=irrep.dim)
                for irrep in br.irreps
            ]
            for irrep in little_irreps:
                elementary_band_representation.kspace_little_irrep.append(irrep)

            elementary_band_representations.append(elementary_band_representation)

    return elementary_band_representations
