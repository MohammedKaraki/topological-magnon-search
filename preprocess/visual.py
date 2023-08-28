from preprocess.br import LittleIrrep
import numpy as np

import log

logger = log.create_logger(__name__)


PALETTE = [
    (15, 181, 174),
    (222, 61, 130),
    (64, 70, 202),
    (246, 133, 17),
    (126, 132, 250),
    (114, 224, 106),
    (20, 122, 243),
    (115, 38, 211),
    (232, 198, 0),
    (203, 93, 0),
    (0, 143, 93),
    (188, 233, 49),
]


def latex_template_path():
    return  "src/latex/template.tex"


with open(latex_template_path(), "r") as f:
    LATEX_TEMPLATE = f.read()


def rgb(components):
    return r"{{rgb,1:red,{};green,{};blue,{}}}".format(
        *((np.array(components) / 255.0).round(2))
    )


def orbit(num):
    delta = -2.0 * np.pi / 4 if num == 3 else 0
    dtheta = 2.0 * np.pi / num

    scaling = 1.0 / np.sin(dtheta / 2.0)

    vertices = np.array(
        [
            np.array([np.cos(dtheta * t - delta), np.sin(dtheta * t - delta)])
            for t in range(num)
        ]
    )
    return scaling * (vertices - vertices.mean(axis=0)).round(3)


class LatexBoilerplate:
    def dump(style_code, nodes_code, edges_code):
        print(
            LATEX_TEMPLATE.replace(r"STYLE-CODE", "\n".join(style_code))
            .replace(r"EDGE-LAYER-CODE", "\n".join(edges_code))
            .replace(r"NODE-LAYER-CODE", "\n".join(nodes_code))
        )


class SuperMode:
    def __init__(self, super_irrep, sub_irreps):
        # assert isinstance(super_irrep, LittleIrrep)
        # assert isinstance(sub_irreps, list)
        # assert isinstance(sub_irreps[0], LittleIrrep)

        self._super_irrep = super_irrep
        self._sub_irreps = sub_irreps

    @property
    def super_irrep(self):
        return self._super_irrep.replace(r"GM", r"\Gamma")

    @property
    def sub_irreps(self):
        return [x.replace(r"GM", r"\Gamma") for x in self._sub_irreps]

    def draw(self, x, y, color_map, output):
        assert isinstance(output, list)

        sub_colors = np.array([color_map[irrep] for irrep in sorted(self.sub_irreps)])
        sub_coords = 0.75 * orbit(len(self.sub_irreps))

        output.append(
            r"""\node [minimum size=1.2cm, label=above:\huge ${}$,"""
            r"""color={}, line width=.1cm,"""
            r"""fill={}, circle, draw] """
            r""" at ({}cm, {}cm) {{}};""".format(
                self.super_irrep,
                # rgb(sub_colors.mean(axis=0)),
                rgb(np.sqrt(np.mean(sub_colors**2, axis=0))),
                rgb((240, 245, 250)),
                x,
                y,
            )
        )

        for coords, color in zip(sub_coords, sub_colors):
            output.append(
                r"""\node [dot={{.30cm}}, fill={}]"""
                r""" at ({}cm, {}cm) {{}};""".format(
                    rgb(color), *(np.array([x, y]) + 0.15 * coords)
                )
            )

        # output.append(r"""\node [label=center:\huge ${}$] (0) at ({}, {}) {{}};"""
        #               .format(
        #                   self.super_irrep,
        #                   x, y+1,
        #                   )
        #     )

    def draw_broken(self, x, y, color_map, output):
        assert isinstance(output, list)

        N = len(self.sub_irreps)

        sub_colors = np.array([color_map[irrep] for irrep in self.sub_irreps])
        dys = 0.75 * (N - 1) * np.linspace(-0.5, 0.5, N)
        if N == 1:
            dys = np.array([0])

        if N > 1:
            output.append(
                r"""\path [draw, dashed, draw opacity=.6, fill opacity=.1, fill={}] ({}cm, {}cm) ellipse ({}cm and {}cm);""".format(
                    rgb((200, 200, 255)), x, y, 0.5, dys[-1] + 0.5
                )
            )

        n = 0
        for dy, color, irrep in zip(dys, sub_colors, self.sub_irreps):
            n += 1
            output.append(
                r"""\node [dot={{.35cm}}, label={}:\huge${}$, fill={},"""
                r"""]"""
                r""" at ({}cm, {}cm) {{}};""".format(
                    "above"
                    if N == 1
                    else "below"
                    if n == 1
                    else "above"
                    if n == N
                    else "right",
                    irrep,
                    rgb(color),
                    x,
                    y + dy,
                )
            )


class ColorMap:
    def __init__(self):
        self._next_color_idx = 0
        self._color_map = {}

    def __getitem__(self, irrep):
        if irrep not in self._color_map:
            self._color_map[irrep] = PALETTE[self._next_color_idx]
            self._next_color_idx = (self._next_color_idx + 1) % len(PALETTE)

        return self._color_map[irrep]


class Visualizer:
    def __init__(self):
        self.color_map = ColorMap()
        self.style_code = []
        self.nodes_code = []
        self.edges_code = []

    def draw_super_mode(self, super_mode, x, y, broken):
        if broken:
            super_mode.draw_broken(
                x=x,
                y=y,
                color_map=self.color_map,
                output=self.nodes_code,
            )
        else:
            super_mode.draw(
                x=x,
                y=y,
                color_map=self.color_map,
                output=self.nodes_code,
            )

    def draw_super_modes(self, super_modes, x, ymin, ymax, broken=False):
        N = len(super_modes)

        dy = 0.0 if N == 1 else (ymax - ymin) / (N - 1)
        yc = 0.5 * (ymax + ymin)

        def calc_y(idx):
            return round((yc + (idx - (N - 1) / 2) * dy), 5)

        for idx, super_mode in enumerate(super_modes):
            y = calc_y(idx)
            self.draw_super_mode(super_mode, x, y, broken)

    def dump(self):
        LatexBoilerplate.dump(
            style_code=self.style_code,
            nodes_code=self.nodes_code,
            edges_code=self.edges_code,
        )


def test():
    # super_modes = [
    #     SuperMode(super_irrep="GM_{1}",
    #               sub_irreps=["GM_{1}^{+}"]),
    #     SuperMode(super_irrep="GM_{2}",
    #               sub_irreps=["GM_{2}^{+}", "GM_{3}^{-}"]),
    #     SuperMode(super_irrep="GM_{3}",
    #               sub_irreps=["GM_{1}^{+}", "GM_{1}^{-}", "GM_{2}^{-}"]),
    #     SuperMode(super_irrep="GM_{4}",
    #               sub_irreps=["GM_{1}^{+}", "GM_{1}^{-}", "GM_{2}^{-}",
    #                           "GM_{2}^{+}"]),
    #     ]

    vis = Visualizer()
    x = 0

    def draw(super_modes, xp=[0]):
        xp[0] += 4
        x = xp[0]

        # all_irreps = sorted(
        #     list(set(irrep
        #              for super_mode in super_modes
        #              for irrep in super_mode.sub_irreps))
        #     )
        # vis.color_map = {
        #     irrep: color for irrep, color in zip(all_irreps, PALETTE)}

        yminmax = [2, 10]
        vis.draw_super_modes(
            super_modes,
            x,
            *yminmax,
        )

        yminmax = [-12, -6]
        vis.draw_super_modes(super_modes, x, *yminmax, broken=True)

    draw(
        [
            SuperMode("GM_{1}^{+}", ["GM_{1}^{+}"]),
            SuperMode("GM_{2}^{+}", ["GM_{1}^{+}"]),
            SuperMode("GM_{2}^{+}", ["GM_{1}^{+}"]),
            SuperMode("GM_{1}^{+}", ["GM_{1}^{+}"]),
        ]
    )

    draw(
        [
            SuperMode(
                "R_{1}R_{1}",
                [
                    "C_{1}^{-}C_{1}^{+}",
                    "C_{1}^{-}C_{1}^{+}",
                    "C_{3}^{-}C_{1}^{+}",
                    "C_{3}^{-}C_{1}^{+}",
                ],
            ),
        ]
    )

    draw(
        [
            SuperMode(
                "S_{1}S_{1}",
                [
                    "D_{1}^{+}D_{1}^{+}",
                    "D_{1}^{-}D_{1}^{-}",
                    "D_{1}^{+}D_{1}^{+}",
                    "D_{1}^{-}D_{1}^{-}",
                ],
            ),
        ]
    )

    draw(
        [
            SuperMode("T_{1}", ["Y_{1}^{+}", "Y_{1}^{-}", "Y_{2}^{-}"]),
            SuperMode("T_{1}", ["Y_{1}^{+}", "Y_{1}^{-}", "Y_{2}^{-}"]),
        ]
    )

    draw(
        [
            SuperMode("Y_{1}", ["B_{1}^{-}B_{1}^{+}"]),
            SuperMode("Y_{1}", ["B_{1}^{-}B_{1}^{+}"]),
        ]
    )

    draw(
        [
            SuperMode("Z_{1}^{+}Z_{2}^{-}", ["A_{1}^{-}A_{1}^{+}"]),
            SuperMode("Z_{1}^{-}Z_{2}^{+}", ["A_{1}^{-}A_{1}^{+}"]),
        ]
    )

    draw(
        [
            SuperMode("X_{1}^{+}X_{2}^{-}", ["Z_{1}^{-}Z_{1}^{+}"]),
            SuperMode("X_{1}^{-}X_{2}^{+}", ["Z_{1}^{-}Z_{1}^{+}"]),
        ]
    )

    draw(
        [
            SuperMode(
                "U_{1}^{-}U_{1}^{-}", ["E_{1}^{-}E_{1}^{-}", "E_{1}^{-}E_{1}^{-}"]
            ),
            SuperMode(
                "U_{2}^{-}U_{2}^{-}", ["E_{1}^{-}E_{1}^{-}", "E_{1}^{-}E_{1}^{-}"]
            ),
        ]
    )

    vis.dump()


if __name__ == "__main__":
    test()
