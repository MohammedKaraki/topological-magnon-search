import numpy as np
from latticetype import find_latticetype
from re import fullmatch
from mbandrep import kvectors_and_ebrs
from genpos import UnitaryGenpos, fetch_unitary_gs, unitary_gstr_to_mat4x4
from char_table import CharTable
from kvector import KVector
from primvecs import find_primvecsmat
from msg_info_table import MSG_INFO_TABLE
from br import Br
from comp_rels import comp_rels
from band import Band
from br import LittleIrrep
from itertools import accumulate


import log
logger = log.create_logger(__name__)


MSG_NUMBER_PATTERN = r'[0-9]+\.[0-9]+'


def is_valid_msg_number(val):
    return fullmatch(MSG_NUMBER_PATTERN, val) is not None


def dim_from_irreplabel(label):
    m = fullmatch(r'\(([0-9])\)', label[-3:])
    assert m is not None

    result = int(m.groups()[0])
    assert result >= 1
    return result


def _decompose_vec_into_complex_ebr_titles_helper(paired_vec,
                                                  curr_decomp,
                                                  memo,
                                                  wp_and_irrep_to_paired_vec):

    if tuple(paired_vec) in memo:
        return memo[tuple(paired_vec)]

    if np.any(paired_vec < 0):
        return []

    if np.all(paired_vec == 0):
        new_decomp = sorted(curr_decomp)
        return [new_decomp]

    result = []
    for wp_and_irrep, vec in wp_and_irrep_to_paired_vec.items():

        for new in _decompose_vec_into_complex_ebr_titles_helper(
            paired_vec - vec,
            curr_decomp + [wp_and_irrep],
            memo,
            wp_and_irrep_to_paired_vec,
                ):
            if new not in result:
                result.append(new)

    memo[tuple(paired_vec)] = result
    return result


class Msg:

    def __init__(self, number):
        self._set_number(number)

        self._kvectors = None
        self._ebrs = None
        self._unitary_gs = None
        self._kvector_to_char_table = {}
        self._primvecsmat = None
        self._tr_partners = None
        self._comp_rels_at_ksymbol = {}
        self._irrep_labels = None
        self._si_matrix = None
        self._si_orders = None
        self._comp_rels = None
        self._bs_basis = None

    def _set_number(self, number):
        if not is_valid_msg_number(number):
            raise ValueError("Invalid MSG number")
        self._number = number

    @property
    def number(self):
        return self._number

    def _load_primvecsmat(self):
        assert self._primvecsmat is None
        self._primvecsmat = find_primvecsmat(self.number)
        assert self._primvecsmat is not None

    def _load_kvectors_and_ebrs(self):
        assert self._kvectors is None
        assert self._ebrs is None
        self._kvectors, self._ebrs = kvectors_and_ebrs(self.number)
        assert self._kvectors is not None
        assert self._ebrs is not None

    def _load_unitary_gs(self):
        assert self._unitary_gs is None
        self._unitary_gs = fetch_unitary_gs(self._number)
        assert self._unitary_gs is not None

    def _load_char_table(self, kvector):
        assert kvector not in self._kvector_to_char_table
        self._kvector_to_char_table[kvector] = \
            CharTable(msg=self, kvector=kvector)
        assert kvector in self._kvector_to_char_table

    @property
    def irrep_labels(self):
        if self._irrep_labels is None:
            self._load_irrep_labels()

        assert self._irrep_labels is not None
        return self._irrep_labels

    def _load_irrep_labels(self):
        assert self._irrep_labels is None
        self._irrep_labels = sorted(list(set([irrep.as_str()
                                              for wp in self.ebrs
                                              for ebr in self.ebrs[wp].values()
                                              for irrep in ebr.irreps])))

    def irreps_to_vec(self, irreps):
        assert isinstance(irreps[0], LittleIrrep)
        b = np.array([irreps.count(LittleIrrep(irrep_label))
                      for irrep_label in self.irrep_labels
                      ])
        assert sum(b) == len(irreps), b
        assert b.dtype == int

        return b

    def _load_sis_and_comp_rels(self):
        from snf import snf, intinv
        assert self._si_matrix is None
        assert self._si_orders is None
        assert self._comp_rels is None

        ebrs_matrix = np.array([
            self.irreps_to_vec(ebr.irreps)
            for wp in self.ebrs
            for ebr in self.ebrs[wp].values()
            ]).T
        # print(ebrs_matrix)
        # print("},{".join([",".join(str(x)for x in row) for row in ebrs_matrix]))

        Uinv, sigma, V = snf(ebrs_matrix)
        U = intinv(Uinv)

        def vec_to_str(vec):
            return " + ".join(str(count) + irreplabel
                              for count, irreplabel in zip(vec, self.irrep_labels)
                              if count != 0
                              )
        siggg = np.diag(sigma)
        siggg = siggg[siggg != 0]
        # for vec in ebrs_matrix.T:
        #     si = 1
        #     print(si, ":\n", vec_to_str(vec))
        # for si, vec in zip(siggg, U[:,:len(siggg)].T):
        #     if si != 1:
        #         print(si, ":\n", vec_to_str(vec))
        # print(self.kvectors)
        num_bs = len([1 for x in np.diag(sigma) if x != 0])
        assert np.all(np.diag(sigma)[:num_bs] >= 1)

        semi_identity = np.diag(
            [1]*num_bs + [0]*(sigma.shape[0]-num_bs)
            )
        self._comp_rels = np.array([
            row for row in ((U@semi_identity@Uinv)
                            - np.eye(semi_identity.shape[0], dtype=int)
                            )
            if not np.all(row == 0)
            ])
        self._bs_basis = U[:, :ebrs_matrix.shape[1]]

        assert self._comp_rels.dtype == int
        assert ebrs_matrix.dtype == int
        assert not np.all(self._comp_rels == 0)
        assert not np.all(ebrs_matrix == 0)
        assert np.all(self._comp_rels @ ebrs_matrix == 0)

        self._si_orders = np.array([
            si
            for si in np.diag(sigma)
            if si > 1
            ])
        self._si_matrix = (np.array([
            np.mod(row, si)
            for si, row in zip(np.diag(sigma), Uinv)
            if si > 1]
            ))
        assert len(self.si_orders) == 0 or self.si_orders.dtype == int
        assert len(self.si_orders) <= 4
        assert len(self.si_matrix) == 0 or self.si_matrix.dtype == int

        if len(self.si_orders) == 0:
            assert self.symmetry_indicator == "(1)"
        else:
            assert ",".join([str(x) for x in self.si_orders]) \
                == self.symmetry_indicator[1:-1], self.symmetry_indicator

    @property
    def bs_basis(self):
        if self._bs_basis is None:
            self._load_sis_and_comp_rels()
        assert self._bs_basis is not None
        return self._bs_basis

    @property
    def si_matrix(self):
        if self._si_matrix is None:
            self._load_sis_and_comp_rels()
        assert self._si_matrix is not None
        return self._si_matrix

    @property
    def si_orders(self):
        if self._si_orders is None:
            self._load_sis_and_comp_rels()
        assert self._si_orders is not None
        return self._si_orders

    @property
    def comp_rels(self):
        if self._comp_rels is None:
            self._load_sis_and_comp_rels()
        assert self._comp_rels is not None
        return self._comp_rels

    @property
    def primvecsmat(self):
        if self._primvecsmat is None:
            self._load_primvecsmat()
        assert self._primvecsmat is not None
        return self._primvecsmat

    @property
    def kvectors(self):
        if self._kvectors is None:
            self._load_kvectors_and_ebrs()

        assert self._kvectors is not None
        return self._kvectors

    @property
    def ebrs(self):
        if self._ebrs is None:
            self._load_kvectors_and_ebrs()

        assert self._ebrs is not None
        return self._ebrs

    def char_table(self, kvector):
        if kvector not in self._kvector_to_char_table:
            self._load_char_table(kvector)

        assert kvector in self._kvector_to_char_table
        return self._kvector_to_char_table[kvector]

    @property
    def unitary_gs(self):
        if self._unitary_gs is None:
            self._load_unitary_gs()

        assert self._unitary_gs is not None
        return self._unitary_gs


    def contains_unitary_g(self, g):
        primvecsmat_inv = np.linalg.inv(self.primvecsmat)

        for h in self.unitary_gs:
            if (np.all(h.mat4x4[:3, :3] == g.mat4x4[:3, :3])
                    and np.all(np.fmod(
                        primvecsmat_inv
                        @ (h.mat4x4[:3, 3]-g.mat4x4[:3, 3]).astype(float),
                        1.0) == 0.0)):
                return True

        return False

    @property
    def label(self):
        return MSG_INFO_TABLE[self.number][0]

    @property
    def symmetry_indicator(self):
        return MSG_INFO_TABLE[self.number][1]

    @property
    def all_gstrs(self):
        return MSG_INFO_TABLE[self.number][2].split(';')

    def comp_rels_at_ksymbol(self, ksymbol):
        assert [
            kvector.symbol for kvector in self.kvectors
            ].count(ksymbol) == 1

        if ksymbol not in self._comp_rels_at_ksymbol:
            logger.info('compatibility relations for {} at {} requested...'
                        .format(self.number, ksymbol)
                        )
            self._comp_rels_at_ksymbol[ksymbol] = comp_rels(self.number,
                                                            ksymbol)
        return self._comp_rels_at_ksymbol[ksymbol]

    def common_comp_rels_dict(self, ksymbol1, ksymbol2):
        comp_rels1 = self.comp_rels_at_ksymbol(ksymbol1)
        comp_rels2 = self.comp_rels_at_ksymbol(ksymbol2)

        assert comp_rels1[-1][1] == comp_rels2[-1][1] == 'GP:(u,v,w)'

        first_line_ksymbol = None

        line_ksymbols1 = [x for _, x, _ in comp_rels1]
        line_ksymbols2 = [x for _, x, _ in comp_rels2]

        for line_ksymbol1 in line_ksymbols1:
            if line_ksymbol1 in line_ksymbols2:
                first_line_ksymbol = line_ksymbol1
                break

        for line_ksymbol2 in line_ksymbols2:
            if line_ksymbol2 in line_ksymbols1:
                assert line_ksymbol2 == first_line_ksymbol
                break

        comp_rel_dict = {}

        for lhs, line, rhs in comp_rels1:
            if line == first_line_ksymbol:
                assert lhs not in comp_rel_dict
                comp_rel_dict[lhs] = rhs

        for lhs, line, rhs in comp_rels2:
            if line == first_line_ksymbol:
                comp_rel_dict[lhs] = rhs

        return comp_rel_dict


    def __str__(self):
        return "{{{}, {}, {}}}".format(self.number,
                                       self.label,
                                       self.symmetry_indicator)

    def _irrep_tr_partners_at_kvector(self, kvector):

        def unitaryg_and_trkvector():

            def is_zero_mod_K(vec3):
                return np.allclose(np.fmod(self.primvecsmat.T
                                           @ vec3.astype(float),
                                           1.0), 0.0)

            unitaryg_and_trkvector_matches = [
                (g, trk)
                for g in self.unitary_gs
                for trk in self.kvectors
                if is_zero_mod_K(
                    np.linalg.inv(
                        g.mat4x4[:3, :3].T.astype(float))@trk.coords
                    - (-kvector.coords))
                ]

            unitaryg_matches = [x[0] for x in unitaryg_and_trkvector_matches]
            trkvector_matches = list(
                set([x[1] for x in unitaryg_and_trkvector_matches]))


            assert len(unitaryg_matches) >= 1
            assert len(trkvector_matches) == 1


            logger.warning("Arbitrary choice here: unitaryg_matches[0]")
            return unitaryg_matches[0], trkvector_matches[0]

        g_trkvector_to_actual_trk, trkvector = unitaryg_and_trkvector()

        char_rows = []
        for g in self.char_table(trkvector).unitary_gs:
            char_rows.append(
                np.conj(
                    self.char_table(kvector).charsvec_from_g(
                        UnitaryGenpos(
                            np.linalg.inv(
                                g_trkvector_to_actual_trk.mat4x4.astype(float)
                                )
                            @ g.mat4x4
                            @ g_trkvector_to_actual_trk.mat4x4
                            )
                        )
                    )
                )


        tr_char_mat = np.array(char_rows)
        char_mat = self.char_table(trkvector).char_matrix

        kvector_irrep_labels = self.char_table(kvector).irrep_labels
        trkvector_irrep_labels = self.char_table(trkvector).irrep_labels

        def inv_diagonal(diag_mat):
            assert np.allclose(np.diag(np.diag(diag_mat)), diag_mat)
            return np.linalg.inv(diag_mat)
        normalizer = inv_diagonal(char_mat.T.conj() @ char_mat)

        def mat_as_int(mat):
            result = mat.real.round(12).astype(int)
            assert np.allclose(result,  mat), result
            return result

        decomp_mat = mat_as_int(
            tr_char_mat.T.conj() @ char_mat @ normalizer
            )

        def assert_has_exactly_one_irrep_and_return_it(l):
            assert len(l) == 1
            assert l[0][1] == 1
            return l[0][0]

        result = {
            original_label:
            assert_has_exactly_one_irrep_and_return_it(
                [(tr_label, count)
                 for tr_label, count in zip(trkvector_irrep_labels, count_row)
                 if count > 0
                 ]
                )
            for count_row, original_label in zip(decomp_mat, kvector_irrep_labels)
            }

        for orig in result:
            tr = result[orig]
            assert dim_from_irreplabel(orig) == dim_from_irreplabel(tr)

        return {LittleIrrep(x).as_str(): LittleIrrep(y).as_str()
                       for x, y in result.items()
                       }

    def _load_tr_parners(self):
        assert self._tr_partners == None

        self._tr_partners = {}
        for kvector in self.kvectors:
            self._tr_partners = {**self._tr_partners,
                                 **self._irrep_tr_partners_at_kvector(kvector)}

        def test_all_tr_partners():
            for wyckoff in self.ebrs:
                for wyckoffirrep in self.ebrs[wyckoff]:
                    wyckoffirrep_tr = wyckoffirrep.tr_partner()
                    ebr = self.ebrs[wyckoff][wyckoffirrep]
                    ebr_tr = self.ebrs[wyckoff][wyckoffirrep_tr]
                    assertion = (
                        Br([LittleIrrep(self._tr_partners[x.as_str()]) for x in
                                    ebr.irreps])
                        == ebr_tr
                        )
                    assert assertion

        test_all_tr_partners()

    @property
    def tr_partners(self):
        if self._tr_partners == None:
            self._load_tr_parners()
        assert isinstance(self._tr_partners, dict)

        return self._tr_partners

    def band_bs(self, band):
        assert isinstance(band, Band)
        return self.irreps_to_vec(band.all_irreps)

    def band_si(self, band):
        assert isinstance(band, Band)
        if len(self.si_orders) == 0:
            return None
        bs = self.band_bs(band)
        result = [a%b for a, b in zip(self.si_matrix @ bs, self.si_orders)]
        assert all(x >= 0 for x in result)
        return result

    def band_compatibility(self, band):
        assert isinstance(band, Band)
        bs = self.band_bs(band)
        return self.comp_rels @ bs

    def find_isolated_subbands(self, band):
        assert isinstance(band, Band)
        weak_subbands = band.sub_bands
        weak_subbands_compatibility = [self.band_compatibility(weak_subband)
                                       for weak_subband in weak_subbands
                                       ]
        subbands = []
        next_subband = 0
        for weak_subband, accum_comp in zip(weak_subbands,
                                            accumulate(
                                                weak_subbands_compatibility)
                                            ):
            next_subband = next_subband + weak_subband
            if all(x == 0 for x in accum_comp):
                subbands.append(next_subband)
                next_subband = 0

        assert sum(subbands) == band
        return subbands

    def make_irrep1wp_to_irreps(self):
        result = {}

        wp_to_irrep_to_ebr = self.ebrs

        for wp, irrep_to_ebr in wp_to_irrep_to_ebr.items():
            for irrep in irrep_to_ebr:

                ebr1 = irrep_to_ebr[irrep]

                irrep1wp = "({},{})".format(
                    irrep,
                    wp.label
                    )
                result[irrep1wp] = [str(x) for x in ebr1.irreps]

        return result

    def make_irrep12wp_to_paired_vec(self):
        result = {}

        wp_to_irrep_to_ebr = self.ebrs

        for wp, irrep_to_ebr in wp_to_irrep_to_ebr.items():
            for irrep in irrep_to_ebr:
                irrep2 = irrep.tr_partner()

                ebr1 = irrep_to_ebr[irrep]
                ebr2 = irrep_to_ebr[irrep2]

                def sanity_check():
                    ebr1_irreps = sorted([x.as_str() for x in ebr1.irreps])
                    ebr2_irreps = sorted([x.as_str() for x in ebr2.irreps])

                    ebr1_irreps_c = sorted([self.tr_partners[x]
                                            for x in ebr1_irreps])
                    ebr2_irreps_c = sorted([self.tr_partners[x]
                                            for x in ebr2_irreps])

                    assert ebr1_irreps == ebr2_irreps_c
                    assert ebr1_irreps_c == ebr2_irreps
                sanity_check()

                vec = np.array(self.irreps_to_vec((ebr1 + ebr2).irreps))
                assert vec.dtype == int

                irrep12wp = "({},{},{})".format(
                    irrep,
                    irrep2,
                    wp.label
                    )
                result[irrep12wp] = vec

        return result

    def decompose_vec_into_irrep12wps(self, paired_vec):
        assert paired_vec.dtype == int

        wp_and_irrep_to_paired_vec = self.make_irrep12wp_to_paired_vec()
        curr_decomp = []
        memo = {}

        return _decompose_vec_into_complex_ebr_titles_helper(
            paired_vec,
            curr_decomp,
            memo,
            wp_and_irrep_to_paired_vec,
            )

    def decompose_irreps_into_irrep12wps(self, irreps):
        vec = self.irreps_to_vec(irreps)
        return self.decompose_vec_into_irrep12wps(vec)
