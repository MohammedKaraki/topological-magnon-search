_POINT_GROUPS_AND_IRREP_PAIRS = [
    ("1 2' m'", "A"),
    ("-1 2'/m'", "A_{g}"),
    ("2 2'2'2 m'm'2", "B"),
    ("2/m m'm'm", "B_{g}"),
    ("m m'm2'", "A''"),
    ("4 -4 42'2' 4m'm' -42'm' 3 32' 3m'", "^{1}E"),
    ("6 62'2' 6m'm'", "^{1}E_{2}"),
    ("-6 -6m'2'", "^{1}E''"),
    ("4/m 4/mm'm' -3 -3m'", "^{1}E_{g}"),
    ("6/m 6/mm'm'", "^{1}E_{2g}"),
]

_POINT_GROUP_TO_IRREP_MAP = {
    point_group: irrep
    for point_groups, irrep in _POINT_GROUPS_AND_IRREP_PAIRS
    for point_group in point_groups.split()
}


def s_plus_irrep_for_point_group(point_group_label):
    return _POINT_GROUP_TO_IRREP_MAP[point_group_label]
