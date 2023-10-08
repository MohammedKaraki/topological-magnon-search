from wolframclient.evaluation import WolframLanguageSession
from wolframclient.language import wl
import numpy as np
from joblib import Memory


def intinv(mat):
    assert mat.dtype == int
    invmat = np.linalg.inv(mat).round().astype(int)
    assert invmat.dtype == int
    assert np.all(invmat @ mat == np.eye(mat.shape[0])), (invmat, mat)

    return invmat


memory = Memory("/tmp", verbose=0)


@memory.cache
def snf(mat):
    assert mat.dtype == int

    session = WolframLanguageSession()

    p, Sigma, q = [
        np.array(x, dtype=int)
        for x in session.evaluate(wl.SmithDecomposition(mat.tolist()))
    ]

    assert np.all(intinv(p) @ Sigma @ intinv(q) == mat)

    session.terminate()
    return p, Sigma, q
