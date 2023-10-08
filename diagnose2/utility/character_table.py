import numpy as np

from magnon.fetch.character_table import fetch_single_valued_unitary_character_table


def character_table_info(msg_number, kvector_type_label):
    # only temporary until refactoring is complete
    # only temporary until refactoring is complete
    # only temporary until refactoring is complete
    from magnon.fetch.character_table import _char_table_info
    a, b, c = _char_table_info(msg_number, kvector_type_label)
    b = ["{}({})".format(ba, bb) for ba, bb in b]
    return a, b, c



from magnon.fetch.utility.general_position_utility import UnitaryGenpos


class CharTable:
    def __init__(self, msg, kvector):
        self._msg = msg
        self._kvector = kvector

        self._unitary_gs, self._irrep_labels, self._char_matrix = character_table_info(
            msg.number, self._kvector.symbol
        )

    @property
    def irrep_labels(self):
        return self._irrep_labels

    @property
    def unitary_gs(self):
        return self._unitary_gs

    @property
    def char_matrix(self):
        return self._char_matrix

    @property
    def msg(self):
        return self._msg

    @property
    def kvector(self):
        return self._kvector

    def charsvec_from_g(self, g):
        assert isinstance(g, UnitaryGenpos)

        # almost equivalent = equivalent modulus translation
        almostequivs = [
            (idx, h)
            for idx, h in enumerate(self.unitary_gs)
            if np.all(h.mat4x4[:3, :3] == g.mat4x4[:3, :3])
        ]
        assert len(almostequivs) == 1, almostequivs
        # assert almostequivs[0][1] == almostequivs[1][1]

        # the two almost equivalent are different by 2pi rotation (double group)
        # assert almostequivs[1][0] - almostequivs[0][0] == len(self.unitary_gs) / 2

        def g_to_translation_vec3(g):
            assert np.all(g.mat4x4[:3, :3] == np.eye(3))
            return g.mat4x4[:3, 3]

        h_row_idx, h = almostequivs[0]
        dr_correct = g_to_translation_vec3(g @ h.inv())
        dr_incorrect = g_to_translation_vec3(h.inv() @ g)

        phase_correct_1 = np.exp(2.0j * np.pi * (self.kvector.coords @ dr_correct))
        phase_correct_2 = np.exp(2.0j * np.pi * (self.kvector.coords @ dr_incorrect))
        assert np.allclose(phase_correct_1, phase_correct_2)

        assert np.allclose(dr_correct, (g.mat4x4[:3, 3] - h.mat4x4[:3, 3]))

        assert np.allclose(
            np.fmod(
                (np.linalg.inv(self.msg.primvecsmat) @ dr_correct)
                .astype(float)
                .round(14),
                1.0,
            ),
            0.0,
        ), (np.linalg.inv(self.msg.primvecsmat) @ dr_correct).astype(float)

        return phase_correct_1 * self.char_matrix[h_row_idx, :]
