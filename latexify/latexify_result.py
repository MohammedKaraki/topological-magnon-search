from magnon.diagnose2.search_result_pb2 import (
    SearchResults,
    SearchResult,
)
from typing import List

_LONGTABLE_TEMPLATE_PATH = "latexify/longtable_template.tex"
_DOCUMENT_TEMPLATE_PATH = "latexify/document_template.tex"


def tex_document_from_template(body: str, extra_packages: str = ""):
    template = open(_DOCUMENT_TEMPLATE_PATH, "r").read()
    return template.replace("{BODY}", body).replace("{EXTRA_PACKAGES}", extra_packages)


def _tex_longtable_from_template(
    caption: str,
    label: str,
    num_columns: int,
    header: str,
    rows: List[List[str]],
    with_midrules: bool = False,
):
    assert isinstance(num_columns, int)
    assert isinstance(rows, list)
    rows = [
        row if isinstance(row, str) else r"{}\\".format("&".join(row)) for row in rows
    ]
    row_separator = "\\midrule\n" if with_midrules else "\n"
    # Don't draw additional midline in foot during table break
    with_midrules_in_foot = not with_midrules
    body = row_separator.join(rows)

    alignments = "".join(["c"] * num_columns)
    template = open(_LONGTABLE_TEMPLATE_PATH, "r").read()
    return (
        template.replace("{CAPTION}", caption)
        .replace("{LABEL}", label)
        .replace("{ALIGNMENTS}", alignments)
        .replace("{NUMBER_OF_COLUMNS}", str(num_columns))
        .replace("{MAYBE_MIDRULE}", "\\midrule" if with_midrules_in_foot else "")
        .replace("{HEADER}", header)
        .replace("{BODY}", body)
    )


def _partition(l: list, n: int):
    return [l[i : i + n] for i in range(0, len(l), n)]


def _subgroup_wps_case_from_result(result: SearchResult):
    wp_labels = "+".join(
        [
            "${}$".format(atomic_orbital.wyckoff_position.label)
            for atomic_orbital in result.atomic_orbital
        ]
    )
    return "Supergroup: ${}~({})$, WP: {}, Subgroup: ${}~({})$".format(
        result.supergroup_label,
        result.supergroup_number,
        wp_labels,
        result.subgroup_label,
        result.subgroup_number,
    )


def _make_multirows_rows(column_index_to_cells: List[List[str]]):
    depths = [len(cells) for cells in column_index_to_cells]
    max_depth = max(depths)
    rows = []
    for depth_idx in range(0, max_depth):
        row_cells = [
            r"\multirow{{{}}}{{*}}{{{}}}".format(
                max_depth + 1 - len(cells), cells[depth_idx]
            )
            if depth_idx < len(cells)
            else ""
            for cells in column_index_to_cells
        ]
        rows.append(row_cells)
    return rows


def possible_gap_count_table_from_result(result: SearchResult):
    rows = []
    for si in result.si_to_possible_gap_count:
        cell_1 = si.replace("undefined (gap closed)", "gapless").replace(
            "well-defined \& nontrivial", "nontrivial"
        )
        cell_2 = ", ".join(
            [
                str(gap_count)
                for gap_count in result.si_to_possible_gap_count[si].gap_count
            ]
        )
        rows.append([cell_1, cell_2])
    caption = "Number of times a given SI can appear in the band structure. {}.".format(
        _subgroup_wps_case_from_result(result)
    )
    return _tex_longtable_from_template(
        caption=caption,
        label="lablllll",
        num_columns=2,
        header="SI&Possible \#Gaps",
        rows=rows,
    )


def possible_si_table_from_result(result: SearchResult):
    rows = []
    for gap in result.gap_to_possible_si_values:
        cells_1 = [str(gap)]

        cells_2 = [
            ", ".join(sis)
            for sis in _partition(
                [
                    str(si).replace("g", "gapless")
                    for si in result.gap_to_possible_si_values[gap].si
                ],
                5,
            )
        ]

        new_rows = _make_multirows_rows([cells_1, cells_2])
        new_rows = [r"{}\\".format("&".join(new_row)) for new_row in new_rows]
        rows.append("*\n".join(new_rows))

    caption = "Possible SI values in each gap. {}.".format(
        _subgroup_wps_case_from_result(result)
    )
    return _tex_longtable_from_template(
        caption=caption,
        label="lablllll",
        num_columns=2,
        header="Gap\#& Possible SI Values",
        rows=rows,
        with_midrules=True,
    )


def document_from_result(result: SearchResult):
    table_1 = possible_gap_count_table_from_result(result)
    table_2 = possible_si_table_from_result(result)

    body = "\n".join([table_1, table_2])
    return tex_document_from_template(body=body)


def document_from_results(results: SearchResults):
    sections = []
    for result in results.result:
        if result.is_timeout or result.is_negative_diagnosis:
            continue
        section_header = r"\section{{{}}}".format(
            _subgroup_wps_case_from_result(result)
        )
        table_1 = possible_gap_count_table_from_result(result)
        table_2 = possible_si_table_from_result(result)
        sections.append("\n".join([section_header, table_1, table_2]))

    body = "\n".join(sections)
    return tex_document_from_template(body=body)
