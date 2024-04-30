from summary.msg_summary_pb2 import MsgsSummary
from config.output_dirs_python import get_output_dirs
import json


def get_msgs_summary_pathname():
    return "{}/msgs_summary.pb.txt".format(get_output_dirs()["output_base_dir"])


def main():
    MATERIALS_JSON_FILE = "data/materials.json"
    materials = json.load(open(MATERIALS_JSON_FILE, "r"))

    def wps_tuple(material):
        return tuple(sorted(list(material["wp_labels"])))

    msg_to_wps_to_materials = {}
    for material in materials:
        msg = material["msg"]
        if msg not in msg_to_wps_to_materials:
            msg_to_wps_to_materials[msg] = {}

        wps = wps_tuple(material)
        if wps not in msg_to_wps_to_materials[msg]:
            msg_to_wps_to_materials[msg][wps] = []

        msg_to_wps_to_materials[msg][wps].append(material)

    msgs_summary = MsgsSummary()
    for msg, wps_to_materials in msg_to_wps_to_materials.items():
        msg_summary = msgs_summary.msg_summary.add()
        msg_summary.msg_number = msg

        for wps, materials in wps_to_materials.items():
            wps_summary = msg_summary.wps_summary.add()
            for wp in list(wps):
                wps_summary.wp_label.append(wp)

            for material in materials:
                example_material = wps_summary.example_material.add()
                example_material.formula = material["material"]
                example_material.reference.append(material["link"])
                if material["tc"] != "-":
                    example_material.temperature_k = material["tc"]
    msgs_summary_pathname = get_msgs_summary_pathname()
    print(msgs_summary, file=open(msgs_summary_pathname, "w"))
    print("Output: {}".format(msgs_summary_pathname))


if __name__ == "__main__":
    main()
