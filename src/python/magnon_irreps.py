
RAW_PG_TO_IRREPS = [
    ("1 2' m'", "A A"),
    ("-1 2'/m'", "A_{g} A_{g}"),
    ("2 2'2'2 m'm'2", "B B"),
    ("2/m m'm'm", "B_{g} B_{g}"),
    ("m m'm2'", "A'' A''"),
    ("4 -4 42'2' 4m'm' -42'm' 3  32' 3m'", "^{1}E ^{2}E"),
    ("6 62'2' 6m'm'", "^{1}E_{2} ^{2}E_{2}"),
    ("-6 -6m'2'", "^{1}E'' ^{2}E''"),
    ("4/m 4/mm'm' -3 -3m'", "^{1}E_{g} ^{2}E_{g}"),
    ("6/m 6/mm'm'", "^{1}E_{2g} ^{2}E_{2g}"),
    ]


def _process_pr_to_irreps_table(raw_table):
    result = {pg: irreps_str.split()
              for pgs_str, irreps_str in raw_table
              for pg in pgs_str.split()
              }
    for irreps in result.values():
        assert len(irreps) == 2

    return result


def magnon_irreps_from_pg(point_group):
    result = _process_pr_to_irreps_table(RAW_PG_TO_IRREPS)[point_group]
    assert result is not None
    return result
