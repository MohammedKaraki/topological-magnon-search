from magnon.preprocess.genpos import fetch_gdicts
from magnon.preprocess.lattice_type import find_latticetype
from magnon.preprocess.mbandrep import kvectors_and_ebrs
from magnon.preprocess.genpos import unitary_gstr_to_mat4x4
from magnon.preprocess.mkvec import vec3_from_klabel
import numpy as np
from re import fullmatch

from magnon.preprocess import log

logger = log.create_logger(__name__)


def is_zero(array):
    tolerance = 1.0e-4
    return (abs(array) < tolerance).all()


def map_klabels(group_number, subgroup_number, subgroup_to_standard3):
    assert subgroup_to_standard3.shape == (3, 3)

    unitaries = fetch_gdicts(group_number)["unitary"]

    group_klabels, _ = kvectors_and_ebrs(group_number)
    subgroup_klabels, _ = kvectors_and_ebrs(subgroup_number)

    latticetype = find_latticetype(group_number)
    direct_vecs = direct_primvecs_matrix(group_number)
    recip_vecs = recip_primvecs_matrix(group_number)
    logger.debug("Lattice type: {}".format(latticetype))
    logger.debug("Primitive translations:\n{}".format(direct_vecs))
    logger.debug("Primitive reciprocals:\n{}".format(recip_vecs))

    result = []
    subgroupklabel_to_groupklabel = {}
    M = subgroup_to_standard3
    for group_klabel in group_klabels:
        for subgroup_klabel in subgroup_klabels:
            for unitary in unitaries:
                Rg = unitary_gstr_to_mat4x4(unitary["str"])[:3, :3].astype(float)
                diff = np.linalg.inv(Rg.T) @ vec3_from_klabel(
                    group_klabel
                ) - np.linalg.inv(M.T) @ vec3_from_klabel(subgroup_klabel)
                if is_zero(np.fmod(diff @ direct_vecs, 1.0)):
                    if subgroup_klabel in subgroupklabel_to_groupklabel:
                        assert (
                            group_klabel
                            == subgroupklabel_to_groupklabel[subgroup_klabel]
                        )
                    else:
                        subgroupklabel_to_groupklabel[subgroup_klabel] = group_klabel

                        result.append(
                            "\t".join(
                                [
                                    group_klabel,
                                    subgroup_klabel,
                                    str(diff),
                                    unitary["seitz"],
                                ]
                            )
                        )

    return result
