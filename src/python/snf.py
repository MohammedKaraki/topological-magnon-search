# from wolframclient.evaluation import WolframLanguageSession
from wolframclient.language import wl
import numpy as np

import log
logger = log.create_logger(__name__)


def intinv(mat):
    assert mat.dtype == int
    invmat = np.linalg.inv(mat).round().astype(int)
    assert invmat.dtype == int
    assert np.all(invmat @ mat == np.eye(mat.shape[0])), (invmat, mat)

    return invmat


def snf(mat, session=None):
    assert mat.dtype == int

    # logger.debug(mat)
    # session = WolframLanguageSession(kernel="/usr/local/bin/WolframKernel")
    p, Sigma, q = [np.array(x, dtype=int)
                   for x in session.evaluate(wl.SmithDecomposition(mat.tolist()))]

    assert np.all(intinv(p) @ Sigma @ intinv(q) == mat)

    # session.terminate()
    return p, Sigma, q
