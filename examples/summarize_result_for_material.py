from magnon.diagnose2.result_pb2 import SubgroupWyckoffPositionResult, Results
from magnon.diagnose2.perturbed_band_structure_pb2 import PerturbedBandStructures
from magnon.latexify.latexify_result import (
    tex_document_from_template,
    possible_si_table_from_result,
    possible_gap_count_table_from_result,
)
from google.protobuf import text_format
import random
import string
import sys
import json
import os
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


def section_from_key(
    key: Tuple[str, List[str]],
    materials,
    results: List[SubgroupWyckoffPositionResult],
    get_perturbations,
    counter_ptr,
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
            r"\subsection{{Subgroup ${}~({})$}}".format(
                result.subgroup_label, result.subgroup_number
            )
        )
        perturbations = get_perturbations(
            result.supergroup_number,
            result.subgroup_number,
            result.supergroup_from_subgroup_basis,
        )
        if len(perturbations) > 1:
            atoms.append("Perturbations:")
        else:
            assert len(perturbations) == 1
            atoms.append("Perturbation:")

        atoms.append(r"\begin{itemize}")
        for perturbation in perturbations:
            atoms.append(r"\item {}".format(perturbation))
        atoms.append(r"\end{itemize}")
        atoms.append(possible_gap_count_table_from_result(result))
        atoms.append(possible_si_table_from_result(result))

    return "\n".join(atoms)


def make_function_get_perturbations():
    structures = PerturbedBandStructures()
    with open("/tmp/intermediate_result_1.txtpb", "r") as f:
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
        materials = [json.load(open("data/material.json", "r"))]
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
            open("/tmp/intermediate_result_2.txtpb", "r").read(), Results()
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

    body_atoms = []
    body_atoms.append(r"\tableofcontents")


    output_dir = "/tmp/" + ''.join(random.choice(string.ascii_lowercase + string.digits) for _ in
                                   range(20))
    os.makedirs(output_dir)
    os.makedirs(output_dir + "/sections")
    print("Output saved to: " + output_dir)

    counter_ptr = [0]
    for key, materials in key_to_materials.items():
        if key not in key_to_results:
            continue
        results = key_to_results[key]
        section_content = section_from_key(
            key, materials, results, get_perturbations, counter_ptr
        )
        section_filename = r"sections/{}_{}.tex".format(key[0], "+".join(key[1]))
        with open(output_dir + "/{}".format(section_filename), "w") as f:
            f.write(section_content)
        body_atoms.append(r"\include{{{}}}".format(section_filename))

    body = "\n".join(body_atoms)
    with open(output_dir + "/main.tex", 'w') as f:
        f.write(tex_document_from_template(body=body))


if __name__ == "__main__":
    main()
