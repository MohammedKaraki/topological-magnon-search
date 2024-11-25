import numpy as np
from re import fullmatch, findall
from fractions import Fraction


def _mat4x4_to_unitary_gstr(mat):
    assert mat.shape == (4, 4)
    assert np.all(
        mat[3] == np.array([Fraction(0), Fraction(0), Fraction(0), Fraction(1)])
    )

    parts = [_vec4_to_str(v) for v in mat[:-1]]
    parts.append("+1")
    return ",".join(parts)


def _letter_to_vec4(letter):
    letter_to_index = {"x": 0, "y": 1, "z": 2, "": 3}

    vec4 = _zero_vec4()
    vec4[letter_to_index[letter]] = 1

    return vec4


def _zero_vec4():
    return np.array([Fraction(0)] * 4)


def _vec4_to_str(vec4):
    joined = "".join(
        _fraction_to_str(frac, lett) for frac, lett in zip(vec4, ["x", "y", "z", ""])
    )

    assert len(joined) > 0
    if joined[0] == "+":
        return joined[1:]
    else:
        return joined


def _str_to_vec4(input_str):
    PART_PATTERN = r"([-+])([0-9]+)?([xyz])?(/([0-9]+))?"
    FULL_PATTERN = (
        r"(([-+])((([0-9]+)?([xyz])(/([0-9]+))?)" + r"|(([0-9]+)(/([0-9]+))?)))+"
    )

    s = input_str
    assert len(input_str) > 0
    if s[0] != "-":
        s = "+" + s
    assert fullmatch(FULL_PATTERN, s) is not None, s

    sign_to_int = {"+": +1, "-": -1}

    vec4_result = _zero_vec4()
    for sign_g, numerator_g, letter_g, _, denominator_g in findall(PART_PATTERN, s):
        sign = sign_to_int[sign_g]

        numerator = 1
        if numerator_g:
            numerator = int(numerator_g)
            assert numerator > 0

        letter = letter_g

        denominator = 1
        if denominator_g:
            denominator = int(denominator_g)
            assert denominator > 1

        vec4_term = sign * Fraction(numerator, denominator) * _letter_to_vec4(letter)

        vec4_result += vec4_term

    reproduced_input_str = _vec4_to_str(vec4_result)
    assert reproduced_input_str == input_str, (input_str, reproduced_input_str)
    return vec4_result


def _fraction_to_str(fraction, letter_factor):
    assert letter_factor in ["", "x", "y", "z"]

    if isinstance(fraction, float):
        fraction = Fraction(fraction).limit_denominator(max_denominator=10**12)
        assert fraction.denominator <= 7, fraction
    assert isinstance(fraction, Fraction)

    if fraction == Fraction(0):
        return ""

    numerator, denominator = abs(fraction.numerator), fraction.denominator

    parts = [
        "-" if fraction < 0 else "+",
        "" if (numerator == 1 and letter_factor != "") else str(numerator),
        letter_factor,
        ("/" + str(denominator)) if denominator != 1 else "",
    ]

    return "".join(parts)


class UnitaryGenpos:
    def __init__(self, mat4x4, seitz=None):
        mat4x4 = mat4x4.astype(float).round(15)
        assert mat4x4.shape == (4, 4)
        assert np.all(mat4x4[3, :] == [0, 0, 0, 1])

        self._mat4x4 = mat4x4
        self._gstr = _mat4x4_to_unitary_gstr(mat4x4)
        self._seitz = seitz

    @classmethod
    def from_gstr(cls, gstr, cross_check_mat3x4=None, seitz=None):
        mat4x4 = unitary_gstr_to_mat4x4(gstr)
        if cross_check_mat3x4 is not None:
            assert np.all(mat4x4[:3, :] == cross_check_mat3x4)

        return cls(mat4x4, seitz=seitz)

    @property
    def gstr(self):
        return self._gstr

    @property
    def mat4x4(self):
        return self._mat4x4

    @property
    def seitz(self):
        return self._seitz

    def __matmul__(self, other):
        return UnitaryGenpos(self.mat4x4 @ other.mat4x4)

    def __eq__(self, other):
        assert isinstance(other, UnitaryGenpos)
        result1 = self.gstr == other.gstr
        result2 = np.all(self.mat4x4 == other.mat4x4)
        assert result1 == result2

        return result1

    def __hash__(self):
        assert False

    def __repr__(self):
        if self.seitz:
            return self.seitz
        return self.gstr

    def inv(self):
        return UnitaryGenpos(np.linalg.inv(self.mat4x4.astype(float)))


def is_valid_transform(matrix):
    if matrix.shape != (4, 4):
        return False
    if not np.all(matrix[3, :] == [0, 0, 0, 1]):
        return False

    return True


def mat3x4_to_unitary_gstr(mat):
    assert mat.shape == (3, 4)
    mat4 = np.concatenate(
        (mat, np.array([[Fraction(0), Fraction(0), Fraction(0), Fraction(1)]]))
    )
    return _mat4x4_to_unitary_gstr(mat4)


def unitary_gstr_to_mat4x4(input_str):
    components = input_str.split(",")
    assert len(components) == 4, components
    assert components[3] == "+1", components[3]

    l = [_str_to_vec4(x) for x in components[:-1]]
    l.append(np.array([Fraction(0), Fraction(0), Fraction(0), Fraction(1)]))

    mat = np.array(l)
    assert input_str == _mat4x4_to_unitary_gstr(mat)

    return mat
