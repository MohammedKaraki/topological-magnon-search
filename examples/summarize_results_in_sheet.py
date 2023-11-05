from magnon.diagnose2.result_pb2 import SubgroupWyckoffPositionResult, Results
from magnon.diagnose2.perturbed_band_structure_pb2 import PerturbedBandStructures
from magnon.latexify.latexify_result import (
    tex_document_from_template,
    possible_si_table_from_result,
    possible_gap_count_table_from_result,
)
from google.protobuf import text_format
import sys
import json
from typing import List, Tuple
from google.protobuf import text_format


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


def make_sheet(
    key: Tuple[str, List[str]],
    materials,
    results: List[SubgroupWyckoffPositionResult],
    get_perturbations,
    counter_ptr,
):
    atoms = []

    wp_labels = ",".join(
        [
            "{}".format(atomic_orbital.wyckoff_position.label)
            for atomic_orbital in results[0].atomic_orbital
        ]
    )
    supergroup_label = results[0].supergroup_label
    supergroup_number = results[0].supergroup_number

    subgroups = []
    for result in results:
        assert not result.is_timeout and not result.is_negative_diagnosis
        subgroups.append(
            r"{}({})".format(result.subgroup_label, result.subgroup_number)
        )
    if len(subgroups) == 0:
        return

    for idx, material in enumerate(materials):
        material_name, temperature = material["material"], material["tc"]
        material_name = material_name.replace(r"\textsubscript", "_")
        bcs_link = material["link"]
        counter_ptr[0] += 1

        row = []
        if idx == 0:
            row.append(supergroup_label)
            row.append(supergroup_number)
            row.append(wp_labels)
            row.append(",".join(subgroups))
        else:
            row.extend([""] * 4)

        row.append(material_name)
        row.append(temperature)
        row.append(bcs_link)

        print('"{}"'.format('","'.join(row)))


def make_function_get_perturbations():
    structures = PerturbedBandStructures()
    with open("/home/mohammed/Dropbox/backup_structures.abridged.txtpb", "r") as f:
        text_format.Parse(f.read(), structures)

    def function(supergroup_number, subgroup_number, transformation):
        for s in structures.structure:
            if (supergroup_number, subgroup_number, transformation) == (
                s.supergroup.number,
                s.subgroup.number,
                s.group_subgroup_relation.supergroup_from_subgroup_standard_basis,
            ):
                return s.group_subgroup_relation.perturbation_prescription
        assert False

    return function


def main():
    get_perturbations = make_function_get_perturbations()

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
        results = text_format.Parse(
            open("/home/mohammed/Dropbox/backup_result.txtpb", "r").read(), Results()
        )
        for result in results.result:
            if result.is_timeout or result.is_negative_diagnosis:
                continue
            key = _key_from_result(result)
            if key not in key_to_results:
                key_to_results[key] = []
            key_to_results[key].append(result)
        return key_to_results

    key_to_results = helper()

    counter_ptr = [0]
    for key, materials in key_to_materials.items():
        if key not in key_to_results:
            continue
        results = key_to_results[key]
        make_sheet(key, materials, results, get_perturbations, counter_ptr)


if __name__ == "__main__":
    main()
