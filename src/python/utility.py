import log

logger = log.create_logger(__name__)


def contents_as_str(tag):
    """Extract the content of a BeautifulSoup tag as a single string"""
    return "".join(str(x) for x in tag.contents)


def find_unique(tag, subtag_name, attrs):
    subtags = tag.findAll(subtag_name, attrs=attrs)
    assert len(subtags) == 1, len(subtags)

    return subtags[0]


def cleanup_pointgroup_html(html):
    return (
        html.replace(r'<font style="text-decoration:overline;">', "[overline]")
        .replace(r"</font>", "")
        .replace(r"<sub>", "_{")
        .replace(r"</sub>", "}")
        .replace(r"<sup>", "^{")
        .replace(r"</sup>", "}")
        .replace(r"<i>", "")
        .replace(r"</i>", "")
    )


def cleanup_ebr_html(html):
    return (
        html.replace(r'<font size="5">↑</font>', "[uparrow]")
        .replace(r'<font style="text-decoration:overline;">', "[overline]")
        .replace(r"</font>", "")
        .replace(r"Γ", "GM")
        .replace(r"⊕", "[oplus]")
        .replace("\xa0", " ")
        .replace(r"<sub>", "_{")
        .replace(r"</sub>", "}")
        .replace(r"<sup>", "^{")
        .replace(r"</sup>", "}")
        .replace(r"<i>", "")
        .replace(r"</i>", "")
    )


def cleanup_corep_html(html):
    return (
        html.replace(
            r'<span style="white-space: nowrap">√<span '
            + r'style="text-decoration:overline;">2</span></span>',
            "Sqrt(2)",
        )
        .replace(r'<font style="text-decoration:overline;">', "[overline]")
        .replace(r'<font color="black">', "[black]")
        .replace(r'<font color="red">', "[red]")
        .replace(r'<font style="color:red">', "[red]")
        .replace(r"π", "(P)")
        .replace(r"</font>", "")
        .replace(r"<sub>", "_{")
        .replace(r"</sub>", "}")
        .replace(r"<sup>", "^{")
        .replace(r"</sup>", "}")
    )


def cleanup_genpos_html(html):
    return (
        html.replace(r"<sub>", "_{")
        .replace(r"</sub>", "}")
        .replace(r"<sup>", "^{")
        .replace(r"</sup>", "}")
        .replace(r'<fontcolor="#ff0000">', "")
        .replace(r"</font>", "")
        .replace(r"<nobr>", "")
        .replace(r"</nobr>", "")
    )


def list_transpose(l):
    assert len(l) > 0
    assert all(len(row) == len(l[0]) for row in l)
    return [list(col) for col in zip(*l)]


def list_flatten_one_level(l):
    assert all(type(x) == type([]) for x in l)

    return [y for l2 in l for y in l2]
