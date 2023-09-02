from subprocess import check_output
from re import fullmatch

import log

logger = log.create_logger(__name__)


def gstrs_and_presc_of_subgroups(msg):
    exec_path = "diagnose/find_subgroups"

    input = "{} {}".format(msg.number, ";".join(msg.all_gstrs))
    output = check_output([exec_path], input=input.encode("ascii")).decode("ascii")
    result = []
    for line in output.split("\n"):
        m = fullmatch(r"\?([^?]+)\? \?([^?]+)\?", line)
        assert m is not None, line
        result.append(m.groups())

    return result
