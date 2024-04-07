from magnon.diagnose2.process_tables import process_tables
import json
import sys
import os

from magnon.common.logger import create_root_logger, create_logger

_logger = create_logger(__name__)


def main():
    mat = json.load(open("data/material.json", "r"))
    msg, wp_labels = mat["msg"], tuple(mat["wp_labels"])
    filename = "/tmp/intermediate_result_1.txtpb".format()
    spectrums = process_tables(msg, list(wp_labels))
    with open(filename, "w") as f:
        f.write(str(spectrums))
    print(str(spectrums))


if __name__ == "__main__":
    create_root_logger()
    main()
