dictionary = {
    """</math>""": r"$}",
    """<math xmlns:mml="http://www.w3.org/1998/Math/{MathML}">""": r"{$",
    """<math xmlns:mml="http://www.w3.org/1998/Math/{MathML}" altimg="si152.svg">""": r"{$",
    """<math xmlns:mml="http://www.w3.org/1998/Math/{MathML}" display="inline">""": r"{$",
    """</mfrac>""": r"}",
    """<mfrac>""": r"\frac{",
    """</mi>""": r"",
    """<mi />""": r"",
    """<mi>""": r"",
    """<mi mathvariant="bold">""": r"",
    """<mi mathvariant="italic">""": r"",
    """<mi mathvariant="normal" />""": r"",
    """<mi mathvariant="normal">""": r"",
    """</mmultiscripts>""": r"",
    """<mmultiscripts>""": r"",
    """</mn>""": r"",
    """<mn />""": r"",
    """<mn>""": r"",
    """</mo>""": r"",
    """<mo>""": r"",
    """<mprescripts />""": r"",
    """</mrow>""": r"}",
    """<mrow />""": r"",
    """<mrow>""": r"{",
    """<mspace width="0.16em" />""": r"\;",
    """<mspace width="4pt" />""": r"\;",
    """</mstyle>""": r"",
    """<mstyle scriptlevel="0" displaystyle="false">""": r"",
    """</msub>""": r"}",
    """<msub>""": r"_{",
    """</msup>""": r"^{",
    """<msup>""": r"}",
    """</mtext>""": r"}",
    """<mtext>""": r"\textrm{",
    """<mtext mathvariant="normal">""": r"\textrm{",
    """<none />""": r"",
    """</sub>""": r"}",
    """<sub>""": r"\textsubscript{",
    """</sup>""": r"}",
    """<sup>""": r"\textsuperscript{",
}

import sys

text = sys.stdin.read()
for key in dictionary:
    text = text.replace(key, dictionary[key])
print(text)
