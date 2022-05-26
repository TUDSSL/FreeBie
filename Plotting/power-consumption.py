import csv
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import seaborn as sns; sns.set()
import math
import pandas as pd
import argparse

parser = argparse.ArgumentParser(description='Plotting script')
parser.add_argument("-f", required=True, type=str, help="filename")
parser.add_argument("-o", required=False, type=str, default="fig-power-consumption.pdf", help="Output file name")

args = parser.parse_args()

csvFile = args.f
outputFile = args.o

df = pd.read_csv(csvFile)
print(df)
tidy = df.melt(id_vars='conn-int').rename(columns=str.title)
print(tidy)

plt.style.use('seaborn-whitegrid')
new_rc_params = {
        # 'text.usetex': True,
         #'svg.fonttype': 'none',
         # 'text.latex.preamble': r'\usepackage{sourcesanspro}',
         'font.size': 9,
         'font.family': 'sans-serif',
         'font.sans-serif' : 'Source Sans Pro',
         'axes.labelsize': 9,
         'axes.titlesize':9,
         # 'text.fontsize': 10,
         'legend.fontsize': 7,
         'xtick.labelsize': 7,
         'ytick.labelsize': 7,
         'mathtext.fontset': 'custom',
         'mathtext.rm': 'Source Sans Pro',
         'mathtext.it': 'Source Sans Pro:italic',
         'mathtext.bf': 'Source Sans Pro:bold',
         'pdf.fonttype': 42,
         'ps.fonttype': 42
         }

plt.rcParams.update(new_rc_params)

cm = 1/2.54  # centimeters in inches
fig = plt.figure(constrained_layout=True , figsize = (8.5*cm, 4*cm))


lightgreen = '#b2df8a'
lightpurple = '#E1D5E7'
darkpurple = '#9673A6'
darkgreen = '#33a02c'
lightblue = '#a6cee3'
darkblue = '#1f78b4'
red = '#fb9a99'
lightred = '#F8CECC'

colors = [lightpurple, lightgreen,lightblue,darkblue]
sns.set_palette(sns.color_palette(colors))

# df = pd.DataFrame({
#     'Factor': ['Growth', 'Value'],
#     'Weight': [0.10, 0.20],
#     'Variance': [0.15, 0.35]
# })
#
#
# print (df)
# print (tidy)

ax = sns.barplot(x="Conn-Int",
            y="Value",
            hue="Variable",
            data=tidy)
ax.set(xlabel='Connection parameters', ylabel='Energy consumed (uJ)')

ax.set_ylim([0, 1000])

# remove label on legend
ax.legend_.set_title(None)
plt.tight_layout(pad=0.05)
plt.savefig(outputFile)
# plt.show()
