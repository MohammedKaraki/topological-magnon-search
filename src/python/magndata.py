from cached_requests import cached_get
from utility import contents_as_str
from bs4 import BeautifulSoup as bs
from re import search
from pathlib import Path
import json


import log
logger = log.create_logger(__name__)


def magndata_html(number):
    return cached_get(
        url=r'http://webbdcrista1.ehu.es/magndata/index.php',
        params={'this_label': number},
        cache_filename=fr'magndata-{number}.html')


def load_materials():
    jsonfilename = str(
        (Path(__file__) / '../../../data/filtered-materials.json.txt').resolve()
        )

    with open(jsonfilename, 'r') as f:
        data = json.loads(f.read())

    return data['materials']


def material_doi(material):
    saved = {
        "0.375": "10.1021/acsomega.8b01701",
        "0.116": "10.1063/1.1729389",
        "3.2":   "10.1107/97809553602060000904",
        "0.327": "10.1016/0022-4596(80)90559-9",
        "0.142": "10.1016/0022-3697(68)90187-x",
        "0.169": "10.1016/0038-1098(81)90449-x",
        "1.153": "10.1016/0038-1098(70)90579-x",
        "1.230": "10.1016/0167-2738(86)90055-x",
        "0.170": "10.1016/0038-1098(81)90449-x",
        "0.610": "10.1006/jssc.1997.7414",
        "2.12":  "10.1016/0038-1098(75)90544-x",
        "2.11":  "10.1016/0038-1098(75)90544-x",
        "0.405": "10.1016/0022-4596(91)90271-i",
        "1.323": "10.1007/s00269-009-0335-x",
        "0.434": "10.1021/acs.jpcc.8b11371",
        "2.36":  "10.1088/0953-8984/19/23/236201",
        "1.336": "10.1016/s0925-8388(97)00385-x",
        "1.276": "10.1016/j.jmmm.2015.03.016",
        "1.134": "10.1021/ic402186p",
        }
    if material['number'] in saved:
        return saved[material['number']]
    html = magndata_html(material['number'])
    gs = search(r'doi.org/([^"]+)"', html).groups()
    assert len(gs) == 1
    doi = gs[0].lower()

    return doi


def test():
    materials = load_materials()
    for i, material in enumerate(materials):
        html = magndata_html(material['number'])
        print(i, material)
        print(material_doi(material))



if __name__ == "__main__":
    test()
