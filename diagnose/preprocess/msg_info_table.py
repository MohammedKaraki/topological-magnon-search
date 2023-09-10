from magnon.diagnose.preprocess import log

logger = log.create_logger(__name__)


def create_msg_info_table():
    from pathlib import Path

    TABLE_FILEPATH = "data/msgnumber_label_si_genpos.txt"

    result = {}

    with open(TABLE_FILEPATH, "r") as file:
        for line in file.readlines():
            fields = line.split()
            assert len(fields) == 4

            msg_number, msg_label, msg_si, msg_gstrs = fields
            assert msg_number not in result
            result[msg_number] = (msg_label, msg_si, msg_gstrs)

    assert len(result) == 1651
    return result


MSG_INFO_TABLE = create_msg_info_table()
