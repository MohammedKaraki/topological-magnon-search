from magnon.diagnose.preprocess.cached_requests import cached_check_output
from hashlib import sha256
from magnon.diagnose.preprocess.magndata import load_materials, material_doi
from re import sub

import log

logger = log.create_logger(__name__)


def doi2bib(doi):
    cache_filename = "doi2bib-" + sha256(doi.encode("utf-8")).hexdigest() + ".txt"
    return cached_check_output(
        command="doi2bib", args=[doi], cache_filename=cache_filename
    )


def test():
    materials = load_materials()

    all_bibs = []
    for i, material in enumerate(materials):
        doi = material_doi(material)
        bib = "\n".join(x.strip() for x in doi2bib(doi).strip().split("\n"))
        bib = sub(r"^(@[a-z]+\{)([^,]+),\n", r"\g<1>" + doi + ",\n", bib)
        if bib not in all_bibs:
            all_bibs.append(bib)
        # print('{' + material['formula']
        #       .replace('_{', '$_{')
        #       .replace('}', '}$')
        #       .replace('^{', r'$^{')
        #       + r"}~\cite{" + doi + r"}\\")
    for bib in all_bibs:
        print(bib.encode("ascii", "ignore").decode())


if __name__ == "__main__":
    test()
