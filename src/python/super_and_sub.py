from msg import Msg
from genpos import UnitaryGenpos
from identify_group import identify_group
import numpy as np

import log
logger = log.create_logger(__name__)


class SuperAndSub:

    def __init__(self, super_msg, sub_msg_gstrs):
        assert isinstance(super_msg, Msg)
        self._super_msg = super_msg

        identified_submsg = identify_group(sub_msg_gstrs)
        self._sub_msg = Msg(identified_submsg['group_number'])

        self._super_to_sub = identified_submsg['to_standard']

        for gstr in sub_msg_gstrs:
            if gstr[-2:] != '-1':
                assert self.super_msg.contains_unitary_g(
                    UnitaryGenpos.from_gstr(gstr)), \
                    gstr

    @property
    def super_msg(self):
        return self._super_msg

    @property
    def sub_msg(self):
        return self._sub_msg

    @property
    def super_to_sub(self):
        return self._super_to_sub

    def superunitarygs_and_superk_from_subk(self, subkvector):
        assert subkvector in self.sub_msg.kvectors

        super_primvecsmat = self.super_msg.primvecsmat
        def is_zero_mod_K(vec3):
            return np.allclose(np.fmod(super_primvecsmat.T @ vec3.astype(float),
                                  1.0), 0.0)

        subk_in_super_coords = \
            np.linalg.inv(self.super_to_sub[:3, :3].T) @ subkvector.coords
        superunitaryg_and_superk_matches = [
            (superunitaryg, superk)
            for superk in self.super_msg.kvectors
            for superunitaryg in self.super_msg.unitary_gs
            if is_zero_mod_K(
                np.linalg.inv(
                    superunitaryg.mat4x4[:3, :3].T.astype(float))@superk.coords
                - subk_in_super_coords)
            ]

        superunitaryg_matches = [
            x[0] for x in superunitaryg_and_superk_matches]
        superk_matches = list(set(
            (x[1] for x in superunitaryg_and_superk_matches)))

        assert len(superunitaryg_matches) >= 1
        assert len(superk_matches) == 1
        return superunitaryg_matches, superk_matches[0]

    def irrep_decomp(self, subk):
        superunitarygs, superk = self.superunitarygs_and_superk_from_subk(subk)

        logger.warning("Arbitrary choice here")
        superg = superunitarygs[0]

        char_rows = []
        for gprime in self.sub_msg.char_table(subk).unitary_gs:
            g = UnitaryGenpos(
                self.super_to_sub
                @ gprime.mat4x4
                @ np.linalg.inv(self.super_to_sub)
                )

            char_rows.append(
                self.super_msg.char_table(superk).charsvec_from_g(
                    UnitaryGenpos(
                        np.linalg.inv(superg.mat4x4.astype(float))
                        @ g.mat4x4
                        @ superg.mat4x4
                        )
                    ))


        supermat = np.array(char_rows)
        submat = self.sub_msg.char_table(subk).char_matrix

        super_labels = self.super_msg.char_table(superk).irrep_labels
        sub_labels = self.sub_msg.char_table(subk).irrep_labels

        def inv_diagonal(diag_mat):
            assert np.allclose(np.diag(np.diag(diag_mat)), diag_mat)
            return np.linalg.inv(diag_mat)
        normalizer = inv_diagonal(submat.T.conj() @ submat)

        def mat_as_int(mat):
            result = mat.real.astype(int)
            assert np.allclose(result,  mat), mat
            return result

        decomp_mat = mat_as_int(
            supermat.T.conj() @ submat @ normalizer
            )
        result = [
            (super_label,
             [(sub_label, count) for sub_label, count in zip(sub_labels, count_row)
              if count > 0]
             )
            for count_row, super_label in zip(decomp_mat, super_labels)
            ]

        return result
