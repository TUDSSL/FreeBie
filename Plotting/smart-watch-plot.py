import csv
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns; sns.set()
import math
import pandas as pd
import argparse

minimalBarWidth = 2.0
minimalBarWidthFullRange = 0.300

def toBarArray(pdArray, nametime, namevalue, finaltime):
    barArray = []
    starttime = 0
    started = False
    for index, row in pdArray.iterrows():
        if(row[namevalue] == 1 and not started):
            started = True
            starttime = row[nametime]
            # print(str(starttime) + " started")
        elif(row[namevalue] == 0 and index != 0 and started):
            barwidth = row[nametime] - starttime
            if(barwidth < minimalBarWidth):
                barwidth = minimalBarWidth
            barArray.append([starttime, barwidth]) # widen sync pulse a bit
            starttime = 0
            started = False
            # print(str(starttime) + " stopped")
    if(starttime != 0):
        print(starttime, finaltime)
        barArray.append([starttime, finaltime - starttime])
    return barArray

def toSyncArray(pdArray, nametime, syncname, finaltime):
    barArray = []
    starttime = 0
    started = False
    sync = False
    lasttriggeredtime = -999
    for index, row in pdArray.iterrows():
        if(row[syncname] == 1 and not started and ((row[nametime] - lasttriggeredtime) > 0.2)):
            started = True
            # starttime = row[nametime]
            # print(str(starttime) + " started")
        if(row[syncname] == 1 and sync):
            barArray.append([starttime, row[nametime] - starttime + 0.001]) # widen sync pulse a bit
            lasttriggeredtime = row[nametime]
            starttime = 0
            started = False
            sync = False
        elif(row[syncname] == 0 and index != 0 and started):
            starttime = row[nametime]
            sync = True
            # print(str(starttime) + " stopped")
    if(starttime != 0):
        print(starttime, finaltime)
        barArray.append([starttime, finaltime - starttime])
    return barArray

def toBarArrayOnTime(pdArray, nametime, syncname, vccname, finaltime, minbarsize):
    barArray = []
    starttime = 0
    lasttriggeredtime = -999;
    started = False
    ontimeCounter = 0;
    for index, row in pdArray.iterrows():
        if(row[vccname] == 1 and not started):
            correctEdge=False
            for i in range(index, pdArray.index[-1]):
                if(pdArray.iloc[i][syncname] == 1):
                    # print("found sync edge" + str(pdArray.iloc[i][nametime]) )
                    correctEdge=True
                    break
                elif(pdArray.iloc[i][nametime] - row[nametime] > 0.02):
                    break
            if(correctEdge):
                started = True
                starttime = row[nametime]
                # print(str(starttime) + " started")
                ontimeCounter += 1
        elif(row[vccname] == 1 and row[syncname] == 0 and index != 0 and started and ((row[nametime] - starttime) > 0.025)):
            if(ontimeCounter != 1):
                barwidth = row[nametime] - starttime
                if(barwidth < minbarsize):
                    barwidth = minbarsize
                barArray.append([starttime, barwidth]) # widen sync pulse a bit
                lasttriggeredtime = row[nametime]
                starttime = 0
                started = False
                # print(str(row[nametime]) + " stopped")
            else:
                ontimeCounter += 1;
    if(starttime != 0):
        print(starttime, finaltime)
        barArray.append([starttime, finaltime - starttime])
    return barArray

def toPointArray(pdArray, nametime, namevalue, finaltime):
    barArray = []
    starttime = 0
    for index, row in pdArray.iterrows():
        if(row[namevalue] == 1):
            barArray.append(row[nametime])
    return barArray


parser = argparse.ArgumentParser(description='Plotting script')
parser.add_argument("-fa", required=True, type=str, help="Analog filename")
parser.add_argument("-fd", required=True, type=str, help="Digital filename")
parser.add_argument("--pretrimsecs", required=False, type=float, default=0, help="Optional pretrim")
parser.add_argument("-wstart", required=True, type=float, default=0, help="Zoom window start")
parser.add_argument("-wsize", required=True, type=float, default=0, help="Zoom window size")
parser.add_argument("-o", required=False, type=str, default="fig-end-to-end.svg", help="Output file name")
parser.add_argument("-skipxlabel", required=False, type=int, default=0, help="Output file name")

args = parser.parse_args()

csvFileAnalog = args.fa
csvFileDigital = args.fd
pretrimSecs = args.pretrimsecs
detailedWindowStart = args.wstart
detailedWindowSize = args.wsize
outputFile = args.o
skipxlabel = args.skipxlabel

detailedWindowStartTrimmed = detailedWindowStart - pretrimSecs
detailedWindowEndTrimmed = detailedWindowStartTrimmed + detailedWindowSize


# Digital stuff
dfAnalog = pd.read_csv(csvFileAnalog)
# saved_column = df.column_name #you can also use df['column_name']
finaltime = float(dfAnalog.tail(1)['Time [s]'].astype(float))
print("Total data length analog = " + str(finaltime) + " seconds")

dfAnalog = dfAnalog[dfAnalog['Time [s]'] > pretrimSecs]
dfAnalog['Time [s]'] = dfAnalog['Time [s]'].subtract(pretrimSecs)
dfAnalog = dfAnalog.reset_index()
# print("Pre-trimmed " + str(pretrimSecs) + " seconds")

# convert to seperate array to process [index, time, value]


# Digital stuff
dfDigital = pd.read_csv(csvFileDigital)
# saved_column = df.column_name #you can also use df['column_name']
finaltime = float(dfDigital.tail(1)['Time [s]'].astype(float))
print("Total data length digital = " + str(finaltime) + " seconds")

dfDigital = dfDigital[dfDigital['Time [s]'] > pretrimSecs]
dfDigital['Time [s]'] = dfDigital['Time [s]'].subtract(pretrimSecs)
print("Pre-trimmed " + str(pretrimSecs) + " seconds")
dfDigital = dfDigital.reset_index()

vbat_df_final = dfAnalog[['Time [s]','VBAT']]
checkpoint_time_dig_df_final = dfDigital[['Time [s]','Checkpoint']]
network_activity_dig_df_final = dfDigital[['Time [s]','BLE Peripheral']]
on_time_dig_df_final = dfDigital[['Time [s]','Sync','VCC']]
sync_dig_df_final = dfDigital[['Time [s]','Sync']]


checkpoint_array = toBarArray(checkpoint_time_dig_df_final,'Time [s]','Checkpoint', finaltime)
network_array = toBarArray(network_activity_dig_df_final,'Time [s]','BLE Peripheral', finaltime)
on_time_array = toBarArrayOnTime(on_time_dig_df_final,'Time [s]','Sync', 'VCC', finaltime,minimalBarWidthFullRange)
sync_array = toSyncArray(sync_dig_df_final, 'Time [s]','Sync', finaltime)

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

plt.style.use('seaborn-whitegrid')
plt.rcParams.update(new_rc_params)

cm = 1/2.54  # centimeters in inches
if(skipxlabel):
    fig = plt.figure(constrained_layout=True , figsize = (8.5*cm, 2.8*cm))
else:
    fig = plt.figure(constrained_layout=True , figsize = (8.5*cm, 3*cm))

gs = fig.add_gridspec(9, 6)
axs = []
axs.append(fig.add_subplot(gs[0:-2, 0:6]))
axs.append(fig.add_subplot(gs[-1, 0:6]))

lightgreen = '#b2df8a'
lightpurple = '#E1D5E7'
darkpurple = '#9673A6'
darkgreen = '#33a02c'
lightblue = '#a6cee3'
darkblue = '#1f78b4'
red = '#fb9a99'
lightred = '#F8CECC'

sns.lineplot(x='Time [s]', y='VBAT', markers= False, data=vbat_df_final, ax=axs[0],linewidth = 0.8)
# vout first
axs[1].broken_barh(on_time_array, (0, 1), facecolors=darkgreen, linewidth=0)

# axs[1].broken_barh(checkpoint_array, (0.1, 0.8), facecolors=darkblue, linewidth=0)
# axs[1].broken_barh(jit_triggered_array, (0.1, 0.8), facecolors=lightblue, linewidth=0)
# axs[1].broken_barh(init_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)
#
# sns.lineplot(x='Time [s]', y='VBAT', markers= False, data=vbat_df_final, ax=axs[2],markersize=2, markeredgecolor=lightblue, markerfacecolor=lightblue)
# # vout first
# axs[3].broken_barh(on_time_array, (0, 1), facecolors=lightgreen, linewidth=0)
# axs[3].broken_barh(sync_array, (0.1, 0.8), facecolors=['#8f9805'], linewidth=0)
# # axs[3].broken_barh(init_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)
# axs[3].broken_barh(checkpoint_array, (0.1, 0.8), facecolors=darkblue, linewidth=0)
# axs[3].broken_barh(network_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)

axs[0].set(xticklabels=[], xlabel="",   ylabel = "Storage (V)")
# axs[1].set(yticks = [], xticklabels=[],   ylabel = "Checkpoints")
axs[1].set(yticks = [], ylabel = "On/Off", xlabel= "Time (s)")
axs[1].set_ylabel('On/Off', rotation='horizontal', va="center",labelpad=20)
# axs[2].set(xticklabels=[], yticklabels=[], xlabel="", ylabel="")
# axs[3].set(yticks = [], xlabel= "Time (s)")

# 20k lux
# axs[0].set_xlim([0, 120])
# axs[1].set_xlim([0, 120])
#
# axs[2].set_xlim([48.0, 51])
# axs[3].set_xlim([48.0, 51])

# 40k lux
axs[0].set_xlim([0, 140])
axs[1].set_xlim([0, 140])

axs[0].set_ylim([2, 2.7])


if(skipxlabel):
    axs[1].set(xlabel="")

# axs[2].set_xlim([detailedWindowStartTrimmed, detailedWindowEndTrimmed])
# axs[3].set_xlim([detailedWindowStartTrimmed, detailedWindowEndTrimmed])

# plt.tight_layout()
plt.tight_layout(pad=0.05)
fig.subplots_adjust(hspace=.25)
plt.savefig(outputFile)
# plt.show()
