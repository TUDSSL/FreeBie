import csv
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import seaborn as sns; sns.set()
import math
import pandas as pd
import argparse

minimalBarWidth = 2.0
minimalBarWidthFullRange = 0.900

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

def toUpdateFinishedArray(pdArray, nametime, namevalue, finaltime):
    barArray = []
    starttime = 0
    started = False
    for index, row in pdArray.iterrows():
        if(row[namevalue] == 1 and not started):
            started = True
            starttime = row[nametime]
            # print(str(starttime) + " started")
        # elif(row[namevalue] == 0 and index != 0 and started):
        #     barwidth = row[nametime] - starttime
        #     if(barwidth < minimalBarWidth):
        #         barwidth = minimalBarWidth
        #     barArray.append([starttime, barwidth]) # widen sync pulse a bit
        #     starttime = 0
        #     started = False
        #     # print(str(starttime) + " stopped")
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
            barwidth = row[nametime] - starttime
            if(barwidth < minimalBarWidth):
                barwidth = minimalBarWidth
            barArray.append([starttime, barwidth]) # widen sync pulse a bit
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
parser.add_argument("-fa1", required=True, type=str, help="Analog filename")
parser.add_argument("-fd1", required=True, type=str, help="Digital filename")
parser.add_argument("-fa2", required=True, type=str, help="Analog filename")
parser.add_argument("-fd2", required=True, type=str, help="Digital filename")
parser.add_argument("--pretrimsecs1", required=False, type=float, default=0, help="Optional pretrim")
parser.add_argument("--pretrimsecs2", required=False, type=float, default=0, help="Optional pretrim")
parser.add_argument("-o", required=False, type=str, default="fig-end-to-end.pdf", help="Output file name")

args = parser.parse_args()

csvFileAnalog = [args.fa1,args.fa2]
csvFileDigital = [args.fd1, args.fd2]

pretrimSecs = [args.pretrimsecs1,args.pretrimsecs2]

outputFile = args.o

dfAnalog = []
dfDigital = []
finaltime = []

index = -1
for file in csvFileAnalog:
    index = index + 1
    dfAnalog.append(pd.read_csv(file))
    # saved_column = df.column_name #you can also use df['column_name']
    finaltime.append(float(dfAnalog[index].tail(1)['Time [s]'].astype(float)))
    print("Total data length analog = " + str(finaltime) + " seconds")

    dfAnalog[index] = dfAnalog[index][dfAnalog[index]['Time [s]'] > pretrimSecs[index]]
    dfAnalog[index]['Time [s]'] = dfAnalog[index]['Time [s]'].subtract(pretrimSecs[index])
    dfAnalog[index] = dfAnalog[index].reset_index()
    print("Pre-trimmed " + str(pretrimSecs[index]) + " seconds")

# convert to seperate array to process [index, time, value]


index = -1
for file in csvFileDigital:
    index = index + 1
    dfDigital.append(pd.read_csv(file))
    # saved_column = df.column_name #you can also use df['column_name']
    finaltimetemp = float(dfDigital[index].tail(1)['Time [s]'].astype(float))
    print("Total data length digital = " + str(finaltimetemp) + " seconds")

    dfDigital[index] = dfDigital[index][dfDigital[index]['Time [s]'] > pretrimSecs[index]]
    dfDigital[index]['Time [s]'] = dfDigital[index]['Time [s]'].subtract(pretrimSecs[index])
    # print("Pre-trimmed " + str(pretrimSecs[index]) + " seconds")
    dfDigital[index] = dfDigital[index].reset_index()

vbat_df_final0 = dfAnalog[0][['Time [s]','VBAT']]
vbat_df_final1 = dfAnalog[1][['Time [s]','VBAT']]

on_time_dig_df_final0 = dfDigital[0][['Time [s]','VCC']]
on_time_dig_df_final1 = dfDigital[1][['Time [s]','VCC']]

on_time_array0 = toBarArray(on_time_dig_df_final0,'Time [s]','VCC', finaltime[0])
on_time_array1 = toBarArray(on_time_dig_df_final1,'Time [s]','VCC', finaltime[1])

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

gs = fig.add_gridspec(10, 6)
axs = []
axs.append(fig.add_subplot(gs[0:-3, 0:6]))
axs.append(fig.add_subplot(gs[-2, 0:6]))
axs.append(fig.add_subplot(gs[-1, 0:6]))

lightgreen = '#b2df8a'
lightpurple = '#E1D5E7'
darkpurple = '#9673A6'
darkgreen = '#33a02c'
lightblue = '#a6cee3'
darkblue = '#1f78b4'
red = '#fb9a99'
lightred = '#F8CECC'

vbat_df_final1.rename(columns={'VBAT':'FreeBie'},inplace=True)
vbat_df_final1.plot(kind='line', x='Time [s]', y='FreeBie', ax=axs[0], linewidth = 0.8, color=darkblue)
vbat_df_final0.rename(columns={'VBAT':'Packetcraft'},inplace=True)
vbat_df_final0.plot(kind='line', x='Time [s]', y='Packetcraft', ax=axs[0], linewidth = 0.8, color=darkgreen)

axs[1].broken_barh(on_time_array0, (0, 1), facecolors=darkgreen, linewidth=0)
axs[2].broken_barh(on_time_array1, (0, 1), facecolors=darkblue, linewidth=0)


axs[0].set(xticklabels=[], xlabel="",   ylabel = "Storage (V)")
axs[1].set(xticklabels = [],yticks = [], ylabel = "", xlabel= "")
axs[2].set(yticks = [], ylabel = "System\nOperation", xlabel= "Time (s)")

axs[0].set_xlim([0, 1500])
axs[0].set_ylim([2, 2.7])
axs[1].set_xlim([0, 1500])
axs[2].set_xlim([0, 1500])


plt.tight_layout(pad=0.05)
fig.subplots_adjust(hspace=.25)

plt.savefig(outputFile)
# plt.show()
