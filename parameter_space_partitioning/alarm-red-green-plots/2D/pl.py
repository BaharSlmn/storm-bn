import argparse
import re
from fractions import Fraction

from sympy import latex

parser = argparse.ArgumentParser(description='Generate a tikz graph from a Parameter Lifting input file (coming out of storm, with two variables).')
parser.add_argument('--file', type=str, help='the input file')
args = parser.parse_args()

content = None
with open(args.file, 'r') as f:
    content = f.read()
lines = content.split("\n")

# Structure of the file:
# AllViolated: 1/10000<=prob1<=5001/20000,1/10000<=perr<=5001/20000;

axis_x = None
axis_y = None

output = ""

for line in lines:
    title_search = re.search('([\\w]*): ([0-9/]*)<=([\\w]*)<=([0-9/]*),([0-9/]*)<=([\\w]*)<=([0-9/]*);', line)
    if title_search == None:
        assert line == ""
        continue
    state = title_search.group(1)
    x0 = float(Fraction(title_search.group(2)))
    axis_x = title_search.group(3)
    x1 = float(Fraction(title_search.group(4)))
    y0 = float(Fraction(title_search.group(5)))
    axis_y = title_search.group(6)
    y1 = float(Fraction(title_search.group(7)))

    rectangle_style = None
    if state == "ExistsSat":
        rectangle_style = "pattern=dots,pattern color=green,preaction={fill,green!30},"
    elif state == "AllSat":
        rectangle_style = "pattern=crosshatch dots,pattern color=green,preaction={fill,green!30},"
    elif state == "ExistsViolated":
        rectangle_style = "pattern=north west lines,pattern color=red,preaction={fill,red!30},"
    elif state == "AllViolated":
        rectangle_style = "pattern=crosshatch,pattern color=red,preaction={fill,red!30},"
    elif state == "Unknown":
        rectangle_style = ""
    else:
        raise Exception("Unknown state: " + state)

    rectangle_style += "line width = 0mm"

    output += "\draw [{rectangle_style}] ({x0},{y0}) rectangle ({dx},{dy});".format(
        rectangle_style = rectangle_style,
        x0 = x0,
        y0 = y0,
        dx = x1,
        dy = y1
    ) + "\n"

f = open(f'alarm-red-green-plots/2D/base_tex_file.tex', 'r')
latex_content = f.read()
output = latex_content + """
\\begin{{tikzpicture}}
\\begin{{axis}}[
  axis lines=middle,
  every axis x label/.style=
    {{at={{(ticklabel cs: 0.5,0)}}, anchor=north}},
  every axis y label/.style=
    {{at={{(ticklabel cs: 0.5,0)}}, anchor=east}},
  xmin=0,xmax=1,ymin=0,ymax=1,
  xtick distance=0.2,
  ytick distance=0.2,
  xlabel={axis_x},
  ylabel={axis_y},
  title={{}}
]
""".format(
    axis_x = axis_x,
    axis_y = axis_y,
) + output + """
\\end{axis}
\\end{tikzpicture}
\\end{document}
"""
print(output)
