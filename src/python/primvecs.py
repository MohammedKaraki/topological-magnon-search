import numpy as np
from latticetype import find_latticetype
from genpos import fetch_unitary_gs, unitary_gstr_to_mat4x4

import log
logger = log.create_logger(__name__)


def find_primvecsmat_method1(msg_number):
    from fractions import Fraction
    latticetype = find_latticetype(msg_number)
    assert latticetype in ['P', 'A', 'I', 'F', 'C', 'R']

    f11 = Fraction(1)
    f12 = Fraction(1, 2)
    f13 = Fraction(1, 3)
    f23 = Fraction(2, 3)

    vec1 = [f11, 0, 0]
    vec2 = [0, f11, 0]
    vec3 = [0, 0, f11]

    if latticetype == 'P':
        pass
    elif latticetype == 'A':
        vec1 = [0, f12, f12]
    elif latticetype == 'C':
        vec1 = [f12, f12, 0]
    elif latticetype == 'I':
        vec1 = [f12, f12, f12]
    elif latticetype == 'R':
        vec1 = [f23, f13, f13]
        vec2 = [f13, f23, f23]
    elif latticetype == 'F':
        vec1 = [0, f12, f12]
        vec2 = [f12, 0, f12]
        vec3 = [f12, f12, 0]
    else:
        assert False

    result = np.array([vec1, vec2, vec3]).T
    det = np.linalg.det(result.astype(float))

    if latticetype == 'P':
        assert det == 1.0
    elif latticetype == 'A':
        assert det == 1.0 / 2.0
    elif latticetype == 'C':
        assert det == 1.0 / 2.0
    elif latticetype == 'I':
        assert det == 1.0 / 2.0
    elif latticetype == 'R':
        assert det == 1.0 / 3.0
    elif latticetype == 'F':
        assert det == 1.0 / 4.0
    else:
        assert False

    return result


def find_primvecsmat_method2(msg_number):
    from genpos import fetch_gdicts
    result = np.eye(3)

    next_col = 0
    zero_translation_already_found = False
    for g in fetch_gdicts(msg_number)['unitary']:
        pres = unitary_gstr_to_mat4x4(g['str'])
        if (pres[:3,:3] == np.eye(3)).all():
            translation = pres[:3, 3]
            if (translation == np.zeros(3)).all():
                assert not zero_translation_already_found
                zero_translation_already_found = True
            else:
                result[:, next_col] = translation
                next_col += 1

    return result


def find_primvecsmat(msg_number):
    result1 = find_primvecsmat_method1(msg_number)
    result2 = find_primvecsmat_method2(msg_number)
    assert np.allclose(result1.astype(float), result2.astype(float)), result1 - result2

    return result1.astype(float)


def find_recip_primvecsmat(msg_number):
    direct_vecs = find_primvecsmat(msg_number)
    result = np.linalg.inv(direct_vecs.T)

    return result
