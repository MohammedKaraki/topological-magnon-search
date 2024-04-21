from magnon.diagnose2.process_tables import process_tables
import json
import sys
import os

from magnon.utils.logger import create_root_logger, create_logger

_logger = create_logger(__name__)


def main():
    output_materials = []
    key_to_materials = {}
    for mat in json.load(open("data/materials.json", "r")):
        key = (mat["msg"], tuple(mat["wp_labels"]))
        if key not in key_to_materials:
            key_to_materials[key] = []
        key_to_materials[key].append(mat)
    for i, (key, mats) in enumerate(key_to_materials.items()):
        try:
            label = "{}-{}".format(key[0], "-".join(key[1]))
            filename = "/tmp/{}.txtpb".format(label)
            _logger.info(label)
            if os.path.exists(filename):
                continue
            spectrums = process_tables(key[0], list(key[1]))
            _logger.info(spectrums)
            with open(filename, "w") as f:
                f.write(str(spectrums))
        except Exception as e:
            _logger.error(e)
            raise e


if __name__ == "__main__":
    create_root_logger()
    main()
