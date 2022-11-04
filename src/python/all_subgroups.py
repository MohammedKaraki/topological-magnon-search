from subprocess import check_output
from pathlib import Path
from re import fullmatch

import log
logger = log.create_logger(__name__)


def gstrs_and_presc_of_subgroups(msg):
    exec_path = str(
        (Path(__file__) / '../../../build/find_subgroups.exe').resolve())

    input = "{} {}".format(msg.number, ";".join(msg.all_gstrs))
    output = check_output([exec_path],
                   input=input.encode('ascii')
                   ).decode('ascii')
    result = []
    for line in output.split('\n'):
        m = fullmatch(r'\?([^?]+)\? \?([^?]+)\?', line)
        assert m is not None, line
        result.append(m.groups())

    return result
