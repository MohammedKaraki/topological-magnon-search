from magnon.diagnose.preprocess.msg import Msg
from magnon.diagnose.preprocess.genpos import UnitaryGenpos
from magnon.diagnose.preprocess.identify_group import identify_group
import numpy as np
from magnon.diagnose.preprocess.br import LittleIrrep

from magnon.diagnose.preprocess import log

logger = log.create_logger(__name__)


class SuperAndSubMsgs:
    def __init__(self, super_msg, sub_msg_gstrs):
        assert isinstance(super_msg, Msg)
        self._super_msg = super_msg

        identified_submsg = identify_group(sub_msg_gstrs)
        self._sub_msg = Msg(identified_submsg["group_number"])

        self._super_to_sub = identified_submsg["to_standard"]

        for gstr in sub_msg_gstrs:
            if gstr[-2:] != "-1":
                assert self.super_msg.contains_unitary_g(
                    UnitaryGenpos.from_gstr(gstr)
                ), gstr

        self._superkvec_to_superirrep_to_subirreps = None
        self._subkvec_to_superirrep_to_subirreps = None

    @property
    def super_msg(self):
        return self._super_msg

    @property
    def sub_msg(self):
        return self._sub_msg

    @property
    def super_to_sub(self):
        return self._super_to_sub

    def subkvec_to_gs_and_superkvec(self, subkvec):
        assert subkvec in self.sub_msg.kvectors

        super_primvecsmat = self.super_msg.primvecsmat

        def is_zero_mod_K(vec3):
            return np.allclose(
                np.fmod(super_primvecsmat.T @ vec3.astype(float), 1.0), 0.0
            )

        subk_in_super_coords = (
            np.linalg.inv(self.super_to_sub[:3, :3].T) @ subkvec.coords
        )
        superunitaryg_and_superk_matches = [
            (superunitaryg, superk)
            for superk in self.super_msg.kvectors
            for superunitaryg in self.super_msg.unitary_gs
            if is_zero_mod_K(
                np.linalg.inv(superunitaryg.mat4x4[:3, :3].T.astype(float))
                @ superk.coords
                - subk_in_super_coords
            )
        ]

        superunitaryg_matches = [x[0] for x in superunitaryg_and_superk_matches]
        superk_matches = list(set((x[1] for x in superunitaryg_and_superk_matches)))

        # import sys
        # print("{}".format(str(superunitaryg_matches)), flush=True, file=sys.stderr)

        assert len(superunitaryg_matches) >= 1, "{} {} {} {}".format(
            str(superunitaryg_matches),
            str(subkvec),
            str(superk_matches),
            str(self.super_msg.unitary_gs),
        )
        assert len(superk_matches) == 1
        return superunitaryg_matches, superk_matches[0]

    def _irrep_decomp(self, subkvec):
        superunitarygs, superk = self.subkvec_to_gs_and_superkvec(subkvec)

        logger.warning("Arbitrary choice here")
        superg = superunitarygs[0]

        char_rows = []
        for gprime in self.sub_msg.char_table(subkvec).unitary_gs:
            g = UnitaryGenpos(
                self.super_to_sub @ gprime.mat4x4 @ np.linalg.inv(self.super_to_sub)
            )

            char_rows.append(
                self.super_msg.char_table(superk).charsvec_from_g(
                    UnitaryGenpos(
                        np.linalg.inv(superg.mat4x4.astype(float))
                        @ g.mat4x4
                        @ superg.mat4x4
                    )
                )
            )

        supermat = np.array(char_rows)
        submat = self.sub_msg.char_table(subkvec).char_matrix

        super_labels = self.super_msg.char_table(superk).irrep_labels
        sub_labels = self.sub_msg.char_table(subkvec).irrep_labels

        def inv_diagonal(diag_mat):
            assert np.allclose(np.diag(np.diag(diag_mat)), diag_mat)
            return np.linalg.inv(diag_mat)

        normalizer = inv_diagonal(submat.T.conj() @ submat)

        def mat_as_int(mat):
            # result = mat.real.astype(int)
            result = mat.real.round(15).astype(int)
            assert np.allclose(result, mat), mat - result
            assert np.allclose(result - mat, 0), mat - result
            return result

        decomp_mat = mat_as_int(supermat.T.conj() @ submat @ normalizer)
        result = [
            (
                LittleIrrep(super_label),
                [
                    x
                    for xs in [
                        [LittleIrrep(sub_label)] * count
                        for sub_label, count in zip(sub_labels, count_row)
                    ]
                    for x in xs
                ],
            )
            for count_row, super_label in zip(decomp_mat, super_labels)
        ]

        return result

    def _load_kvec_to_superirrep_to_subirreps(self):
        assert self._superkvec_to_superirrep_to_subirreps is None
        assert self._subkvec_to_superirrep_to_subirreps is None

        self._superkvec_to_superirrep_to_subirreps = {}
        self._subkvec_to_superirrep_to_subirreps = {}

        for subkvec in self.sub_msg.kvectors:
            _, superkvec = self.subkvec_to_gs_and_superkvec(subkvec)

            if subkvec not in self._subkvec_to_superirrep_to_subirreps:
                self._subkvec_to_superirrep_to_subirreps[subkvec] = {}
            if superkvec not in self._superkvec_to_superirrep_to_subirreps:
                self._superkvec_to_superirrep_to_subirreps[superkvec] = {}

            for superirrep, subirreps in self._irrep_decomp(subkvec):
                subirreps.sort()
                assert sorted(subirreps) == subirreps, subirreps

                assert (
                    superirrep not in self._subkvec_to_superirrep_to_subirreps[subkvec]
                )
                self._subkvec_to_superirrep_to_subirreps[subkvec][
                    superirrep
                ] = subirreps

                if (
                    superirrep
                    not in self._superkvec_to_superirrep_to_subirreps[superkvec]
                ):
                    self._superkvec_to_superirrep_to_subirreps[superkvec][
                        superirrep
                    ] = []
                self._superkvec_to_superirrep_to_subirreps[superkvec][
                    superirrep
                ].extend(subirreps)

    @property
    def superkvec_to_superirrep_to_subirreps(self):
        if self._superkvec_to_superirrep_to_subirreps is None:
            self._load_kvec_to_superirrep_to_subirreps()
        return self._superkvec_to_superirrep_to_subirreps

    @property
    def subkvec_to_superirrep_to_subirreps(self):
        if self._subkvec_to_superirrep_to_subirreps is None:
            self._load_kvec_to_superirrep_to_subirreps()
        return self._subkvec_to_superirrep_to_subirreps
