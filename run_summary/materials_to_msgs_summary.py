from run_summary.msg_summary_pb2 import MsgsSummary
import json


def main():
    args = CommandLineArgs()
    materials = json.load(open(args.input, "r"))

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

    print(msgs_summary, file=open(args.output, "w"))


class CommandLineArgs:
    def __init__(self):
        import argparse

        parser = argparse.ArgumentParser(allow_abbrev=False)
        parser.add_argument(
            "--input", required=True, type=str, help="Materials JSON file  path"
        )
        parser.add_argument(
            "--output",
            required=True,
            type=str,
            help="MSG summary proto output filepath",
        )
        args = parser.parse_args()

        self.input = args.input
        self.output = args.output


if __name__ == "__main__":
    main()
