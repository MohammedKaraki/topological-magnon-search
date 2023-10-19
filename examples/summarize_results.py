from magnon.diagnose2.result_pb2 import SubgroupWyckoffPositionResult, Results
from magnon.latexify.latexify_result import (
    tex_document_from_template,
    possible_si_table_from_result,
    possible_gap_count_table_from_result,
)
from google.protobuf import text_format
import sys
import json
from typing import List, Tuple


def _key_from_result(result: SubgroupWyckoffPositionResult):
    supergroup_number = str(result.supergroup_number)
    wp_labels = tuple(
        sorted(
            [
                str(atomic_orbital.wyckoff_position.label)
                for atomic_orbital in result.atomic_orbital
            ]
        )
    )
    return (supergroup_number, wp_labels)


def _key_from_material(material):
    supergroup_number = str(material["msg"])
    wp_labels = tuple(sorted([str(wp) for wp in material["wp_labels"]]))
    return (supergroup_number, wp_labels)


def section_from_key(
    key: Tuple[str, List[str]], materials, results: Results, counter_ptr
):
    atoms = []

    wp_labels = "+".join(
        [
            "${}$".format(atomic_orbital.wyckoff_position.label)
            for atomic_orbital in results[0].atomic_orbital
        ]
    )
    atoms.append(
        r"\section{{${}~({})$ {}}}".format(
            results[0].supergroup_label, results[0].supergroup_number, wp_labels
        )
    )
    atoms.append(r"Materials:\\")
    atoms.append(r"\begin{center}")
    for material in materials:
        counter_ptr[0] += 1
        atoms.append(
            r"{{\color{{red}}{}}}. {{\color{{purple}}{}}} ({{\color{{blue}}{}K}})\\".format(
                counter_ptr[0], material["material"], material["tc"]
            )
        )
    atoms.append(r"\end{center}")
    for result in results:
        assert not result.is_timeout and not result.is_negative_diagnosis
        atoms.append(
            r"\subsection{{${}~({})$}}".format(
                result.subgroup_label, result.subgroup_number
            )
        )
        atoms.append(possible_gap_count_table_from_result(result))
        atoms.append(possible_si_table_from_result(result))

    return "\n".join(atoms)


def main():
    def helper():
        key_to_materials = {}
        materials = json.load(open("data/materials.json", "r"))
        for material in materials:
            key = _key_from_material(material)
            if key not in key_to_materials:
                key_to_materials[key] = []
            key_to_materials[key].append(material)
        return key_to_materials

    key_to_materials = helper()

    def helper():
        key_to_results = {}
        results = text_format.Parse(sys.stdin.read(), Results())
        for result in results.result:
            if result.is_timeout or result.is_negative_diagnosis:
                continue
            key = _key_from_result(result)
            if key not in key_to_results:
                key_to_results[key] = []
            key_to_results[key].append(result)
        return key_to_results

    key_to_results = helper()

    body_atoms = []
    body_atoms.append(r"\tableofcontents\\")

    counter_ptr = [0]
    for key, materials in key_to_materials.items():
        if key not in key_to_results:
            continue
        results = key_to_results[key]
        body_atoms.append(section_from_key(key, materials, results, counter_ptr))

    body = "\n".join(body_atoms)
    print(tex_document_from_template(body=body))


if __name__ == "__main__":
    main()
