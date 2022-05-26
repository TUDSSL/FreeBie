import csv
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import matplotlib.dates as mdates
import numpy as np
import seaborn as sns; sns.set()
import math
import pandas as pd
import argparse
from matplotlib.dates import DateFormatter
from datetime import datetime, timedelta

minimalBarWidth = 0# 0.1 * 100

def toBarArray(pdArray, nametime, namevalue, finaltime):
    barArray = []
    starttime = 0
    started = False
    for index, row in pdArray.iterrows():
        if(row[namevalue] >= 1.6 and not started):
            started = True
            starttime = row[nametime]
            # print(str(starttime) + " started")
            time=starttime
        elif(row[namevalue] < 0.2 and index != 0 and started):
            barwidth = row[nametime] - starttime
            if(barwidth < minimalBarWidth):
                barwidth = minimalBarWidth
            barArray.append([starttime, barwidth]) # widen sync pulse a bit
            starttime = 0
            started = False
            # print(str(starttime) + " stopped")
        else:
            time = row[nametime]
    if(starttime != 0):
        print(starttime, finaltime)
        barArray.append([starttime, float(pdArray[-1][nametime]) - starttime])
    return barArray



parser = argparse.ArgumentParser(description='Plotting script')
parser.add_argument("-f", required=True, type=str, help="input file")
parser.add_argument("-fanalog", required=True, type=str, help="analog input file")
parser.add_argument("-o", required=False, type=str, default="ouput.pdf", help="Output file name")
parser.add_argument("--averages", required=False, type=int, default=0, help="Optional averages")

args = parser.parse_args()

inputFile = args.f
inputFileAnalog = args.fanalog
outputFile = args.o
averages = args.averages
pretrimSecs = 0

df = pd.read_csv(inputFile,header=None, usecols=[0,1,2,3,4,5], names=['TimeMS', 'TimeBootNS', 'SensorName', 'Data0', 'Data1', 'Data2'])

# select only the light data
df = df[df["SensorName"] == "OPT3007 Light"]

recordingStartingTime = df["TimeMS"][1]
recordingEndedTime = df["TimeMS"].iloc[-1]

# convert ms since epoch to plotable time
df["TimeMS"] = pd.to_datetime(df["TimeMS"], unit='ms')
#print(df)
print("Recording started at " + pd.to_datetime(recordingStartingTime, unit='ms').strftime("%Y-%m-%d %H:%M:%S.%f"))
print("Recording ended at " + pd.to_datetime(recordingEndedTime, unit='ms').strftime("%Y-%m-%d %H:%M:%S.%f"))
# Digital stuff
dfAnalog = pd.read_csv(inputFileAnalog)
# saved_column = df.column_name #you can also use df['column_name']
finaltime = float(dfAnalog.tail(1)['Time [s]'].astype(float))
print("Total data length analog = " + str(finaltime) + " seconds")

dfAnalog = dfAnalog[dfAnalog['Time [s]'] > pretrimSecs]
dfAnalog['Time [s]'] = dfAnalog['Time [s]'].subtract(pretrimSecs)

dfAnalog = dfAnalog.reset_index()
# print("Pre-trimmed " + str(pretrimSecs) + " seconds")

dfAnalog['Time [s]'] = dfAnalog['Time [s]'].add(recordingStartingTime/1000)

AnalogRecordingStartingTime = dfAnalog["Time [s]"][1]
AnalogRecordingEndedTime = dfAnalog["Time [s]"].iloc[-1]

print("Analog Recording started at " + pd.to_datetime(AnalogRecordingStartingTime, unit='s').strftime("%Y-%m-%d %H:%M:%S.%f"))
print("Analog Recording ended at " + pd.to_datetime(AnalogRecordingEndedTime, unit='s').strftime("%Y-%m-%d %H:%M:%S.%f"))

vcc_dut_final = dfAnalog[['Time [s]','VCC']]


dfAnalog["Time [s]"] = pd.to_datetime(dfAnalog["Time [s]"], unit='s')
# print(dfAnalog)
checkpoint_array = toBarArray(vcc_dut_final,'Time [s]','VCC', finaltime)

test = pd.DataFrame(checkpoint_array, columns =['Time', 'Duration'])
test["Time"] = pd.to_datetime(test["Time"], unit='s')

for index, row in test.iterrows():
    print(timedelta(seconds = row["Duration"]))
    test.loc[index, "Duration"] = timedelta(seconds = row["Duration"])

checkpoint_array = test.values.tolist()
print(test)
print(checkpoint_array)

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
         'ps.fonttype': 42,
          # 'date.autoformatter.minute' : '%H:%M:%S'
         }

plt.style.use('seaborn-whitegrid')
plt.rcParams.update(new_rc_params)

cm = 1/2.54  # centimeters in inches
fig = plt.figure(constrained_layout=True , figsize = (8.5*cm, 5*cm))
# fig, ax = plt.subplots(constrained_layout=True , figsize = (8.5*cm,5*cm))
gs = fig.add_gridspec(9, 6)
axs = []
axs.append(fig.add_subplot(gs[0:-3, 0:6]))
axs.append(fig.add_subplot(gs[-3:-1, 0:6]))

lightgreen = '#b2df8a'
lightpurple = '#E1D5E7'
darkpurple = '#9673A6'
darkgreen = '#33a02c'
lightblue = '#a6cee3'
darkblue = '#1f78b4'
red = '#fb9a99'
lightred = '#F8CECC'

hours = mdates.HourLocator(interval = 3)

# sns.lineplot(x='TimeMS', y='Data0',ax=axs[0], markers= False, data=df, linewidth = 0.8)
axs[0].axhline(y=300, color='r', linestyle='-', label='minimum viable')

axs[0].set(yscale="symlog")

df['rolling_average'] = df.Data0.rolling(averages).mean()
sns.lineplot(x='TimeMS', y='rolling_average',ax=axs[0], color=darkpurple, markers= False, data=df, linewidth = 0.8)

# sns.lineplot(x='Time [s]', y='VCC',ax=axs[1], color=darkblue, markers= False, data=dfAnalog, linewidth = 0.8)

# sns.lineplot(x='Time [s]', y='VCC',ax=axs[1], markers= False, data=vcc_dut_final, linewidth = 0.8, color=darkblue)
axs[1].broken_barh(checkpoint_array, (0, 1), facecolors=darkblue, linewidth=0)
# xticks = ax.get_xticks()
# ax.set_xticklabels([pd.to_datetime(tm, unit='ms').strftime('%Y-%m-%d\n %H:%M') for tm in xticks],
#  rotation=50)


myFmt = mdates.DateFormatter('%H:%M')
axs[0].xaxis.set_major_formatter(myFmt)
axs[0].xaxis.set_major_locator(hours)

axs[1].xaxis.set_major_formatter(myFmt)
axs[1].xaxis.set_major_locator(hours)
# print(axs[0].get_xticklabels())

axs[0].set_ylim([0, 100000])
# print(dfAnalog['Time [s]'])
axs[0].set_xlim([pd.to_datetime(recordingStartingTime, unit='ms') , pd.to_datetime(AnalogRecordingEndedTime, unit='s')])
axs[1].set_xlim([pd.to_datetime(recordingStartingTime, unit='ms') , pd.to_datetime(AnalogRecordingEndedTime, unit='s')])

axs[0].set(xticklabels=[]) # remove tick labels

axs[0].set_xlabel('')
axs[0].set_ylabel('Luminescence (Lux)')

axs[1].set_ylabel('On/Off', rotation='horizontal', va="center",labelpad=20)


# axs[1].set_ylim([0, 2])
axs[1].set_ylim([0, 1])

axs[1].set(yticks = [])


plt.legend()

plt.tight_layout(pad=0.05)

plt.savefig(outputFile)
#plt.show()
