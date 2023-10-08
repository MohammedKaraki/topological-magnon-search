_TABLE_FILEPATH = "data/msg_number_label_si_genpos.txt"


def _create_msg_info_table():
    result = {}

    with open(_TABLE_FILEPATH, "r") as file:
        for line in file.readlines():
            fields = line.split()
            assert len(fields) == 4

            msg_number, msg_label, msg_si, msg_gstrs = fields
            assert msg_number not in result
            result[msg_number] = (msg_label, msg_si, msg_gstrs)

    assert len(result) == 1651
    return result


_MSG_INFO_TABLE = _create_msg_info_table()


def msg_label_from_number(msg_number):
    return _MSG_INFO_TABLE[msg_number][0]


def msg_si_from_number(msg_number):
    return _MSG_INFO_TABLE[msg_number][1]


def msg_gstrs_from_number(msg_number):
    return _MSG_INFO_TABLE[msg_number][2].split(";")
