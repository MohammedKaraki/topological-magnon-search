from cached_requests import cached_post
from utility import contents_as_str, cleanup_corep_html
from bs4 import BeautifulSoup as bs
from mkvec import symbol_from_klabel
from re import fullmatch, sub
from genpos import mat3x4_to_unitary_gstr, UnitaryGenpos
import numpy as np
from fractions import Fraction

import log
logger = log.create_logger(__name__)


ROUND_DECIMALS = 10


def fetch_corep_html(group_number, ksymbol):
    return cached_post(
        url=r'https://www.cryst.ehu.es/cgi-bin/cryst/programs/corepresentations_out.pl',
        data={'super': group_number,
              'symbol': '',
              'vecfinal': ksymbol,
              'list': 'Submit'},
        cache_filename=fr'corep-{group_number}-{ksymbol}.html')



def eval_reprmatrix_element(s):
    s = s \
        .replace('t1', '(0)') \
        .replace('t2', '(0)') \
        .replace('t3', '(0)')

    s = sub(r'([0-9]+)', r'(\1)', s)

    s = s \
        .replace('i', '(i)') \
        .replace('P', '(P)') \
        .replace(')(', ')*(') \
        .replace('i', '1.0j') \
        .replace('e^{', 'np.exp(') \
        .replace('}', ')') \
        .replace('P', 'np.pi') \
        .replace(')Sqrt', ')*Sqrt') \
        .replace('Sqrt', 'np.sqrt')

    return eval(s)


def tabletag_to_trace(table_tag):
    if 'color="red"' in str(table_tag):
        return "Antiunitary Element... not parsed"

    rows = []
    ncols = -1

    trs = table_tag.tbody.findAll('tr', recursive=False)
    for tr in trs:
        tds = tr.findAll('td', recursive=False)
        if ncols == -1:
            ncols = len(tds)
        else:
            assert ncols == len(tds)

        row = []
        for td in tds:
            row.append(cleanup_corep_html(contents_as_str(td)).strip())

        rows.append(row)


    assert len(rows) > 0, table_tag
    assert len(rows[0]) > 0, table_tag

    INDEX_PATTERN = r'\(([1-6]);([1-6])\): (.+)'
    def parse_index_form(s):
        m = fullmatch(INDEX_PATTERN, s)
        if m is None:
            return None
        return m.groups()


    is_index_form = parse_index_form(rows[0][0]) is not None

    assert all(
        is_index_form == (parse_index_form(x) is not None)
        for row in rows for x in row
        )

    if is_index_form:
        matrix = [(rownum, colnum, (eval_reprmatrix_element(valstr)))
                  for row in rows
                  for entry in row
                  for rownum, colnum, valstr in [parse_index_form(entry)]
                  ]
    else:
        matrix = [(rowidx+1, colidx+1, val)
                  for rowidx, row in enumerate(rows)
                  for colidx, valstr in enumerate(row)
                  for val in [(eval_reprmatrix_element(valstr))]
                  if val != 0
                  ]

    trace = 0.0
    for rownum, colnum, val in matrix:
        if rownum == colnum:
            trace += val

    return np.round(trace, ROUND_DECIMALS)


def char_table_info(msg_number, ksymbol):
    html = fetch_corep_html(msg_number, ksymbol)
    soup = bs(html, 'html5lib')

    tables = soup.findAll('table',
                          attrs={'border': '5',
                                 'align': 'center'})
    assert len(tables) == 2, len(tables)
    table = tables[0]

    trs = table.tbody.findAll('tr', recursive=False)
    header = trs[0]
    data = trs[1:]

    irrep_labels = []
    header_tds = header.findAll('td', recursive=False)
    assert len(header_tds) >= (2     # presentation & seitz columns
                               + 2   # at least 2 irrep columns
                               )
    for td in header_tds[2:]:
        irrep_labels.append(cleanup_corep_html(contents_as_str(td.center)))

    unitary_gs = []
    char_table = []
    for tr in data:
        tds = tr.findAll('td', recursive=False)
        assert len(irrep_labels) + 3 == len(tds)

        chars = []
        for irrep_td in tds[3:]:
            chars.append(tabletag_to_trace(irrep_td.table.table))

        tmp = cleanup_corep_html(
            contents_as_str(tds[0].table.tbody.tr.pre)
            .replace("t1", "0")
            .replace("t2", "0")
            .replace("t3", "0")
            ).split()
        color_to_unitary = {"[black]": True,
                            "[red]": False,
                            }
        is_unitary = color_to_unitary[tmp[0]]
        gstr = mat3x4_to_unitary_gstr(
            np.reshape([Fraction(x) for x in tmp[1:13]],
                       newshape=(3, 4)))

        if is_unitary:
            unitary_gs.append(UnitaryGenpos.from_gstr(gstr))
            char_table.append(chars)

    assert len(char_table) >= 1

    char_matrix = np.array(char_table)
    # assert np.all(char_matrix[:,0] == char_matrix[0,0])

    inner_prods = char_matrix.T.conj() @ char_matrix
    diag = np.diag(inner_prods)
    assert np.min(np.abs(diag)) > 1.0, diag
    assert np.allclose(np.diag(diag),
                       inner_prods)
    assert np.allclose(inner_prods.imag, 0), inner_prods.imag
    assert np.all(
        np.abs(inner_prods.real.astype(int) - inner_prods.real) <= 1e-8
        )

    def as_positive_int(x):
        result = int(x.real)
        assert result == x
        assert result >= 1
        return result


    irrep_labels_dimmed = [
        r"{}({})".format(label, as_positive_int(dim))
        for label, dim in zip(irrep_labels, char_matrix[0])
        ]

    num_single_irreps = len([label for label in irrep_labels
                             if 'overline' not in label])

    return (irrep_labels_dimmed[:num_single_irreps],
            unitary_gs,
            char_matrix[:, :num_single_irreps]
            )
