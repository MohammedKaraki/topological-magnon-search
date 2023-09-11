from magnon.preprocess.cached_requests import cached_post
from magnon.preprocess.genpos import is_valid_mat4x4
from magnon.preprocess.utility import contents_as_str, find_unique
from bs4 import BeautifulSoup as bs
from fractions import Fraction
import re
import numpy as np
from hashlib import sha256

from magnon.preprocess import log

logger = log.create_logger(__name__)


def identify_group_html(gstrs):
    assert isinstance(gstrs, list)
    gstrs_combined = "\r\n".join(sorted(list(set(gstrs))))

    assert gstrs_combined.count(" ") == 0 and gstrs_combined.count("\t") == 0

    cache_filename = (
        "identify_group-" + sha256(gstrs_combined.encode("utf-8")).hexdigest() + ".html"
    )

    logger.info(
        "gstrs_combined (new lines replaces with semicolons): "
        "{}\nfilename: {}".format(gstrs_combined.replace("\r\n", ";"), cache_filename)
    )

    result = cached_post(
        url=r"https://www.cryst.ehu.es/cgi-bin/cryst/programs/checkgr.pl",
        data={
            "tipog": "gmag",
            "setting": "BNS",
            "generators": gstrs_combined,
            "list": "Submit",
        },
        cache_filename=cache_filename,
    )

    return result


def identify_group(gstrs):
    html = identify_group_html(gstrs)
    soup = bs(html, "html5lib")

    def extract_group_label_and_number():
        h2 = find_unique(soup, "h2", attrs={"align": "center"})
        a = find_unique(h2, "a", attrs={"style": "color:#0081a4"})
        raw_result = contents_as_str(a)
        processed_result = (
            raw_result.replace('<font style="text-decoration: overline;">', "-")
            .replace("</font>", "")
            .replace("<sub>", "_{")
            .replace("</sub>", "}")
            .replace("<i>", "")
            .replace("</i>", "")
        )

        m = re.fullmatch(
            r"([-'0-9a-zA-Z/_{}]+) \(No\. ([0-9]+)\.([0-9]+)\)", processed_result
        )
        assert m is not None, processed_result

        return m.groups()[0], r"{}.{}".format(m.groups()[1], m.groups()[2])

    def extract_transformation_matrix_entries():
        td = find_unique(soup, "td", attrs={"valign": "middle"})

        assert len(td.pre.contents) == 1

        entries = contents_as_str(td.pre).split()
        assert len(entries) == 3 * 4

        result = np.reshape(
            [float(Fraction(x)) for x in entries] + [0, 0, 0, 1], (4, 4)
        )
        assert is_valid_mat4x4(result)

        return result

    label, number = extract_group_label_and_number()
    to_standard = extract_transformation_matrix_entries()

    logger.debug("Identified group: {}, {},\n{}".format(number, label, to_standard))

    return {"group_label": label, "group_number": number, "to_standard": to_standard}
