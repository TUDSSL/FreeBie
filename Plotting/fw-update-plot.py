import csv
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import seaborn as sns; sns.set()
import math
import pandas as pd
import argparse
from matplotlib.patches import ConnectionPatch

minimalBarWidth = 0.0005
minimalBarWidthFullRange = 0.200

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
parser.add_argument("-fa", required=True, type=str, help="Analog filename")
parser.add_argument("-fd", required=True, type=str, help="Digital filename")
parser.add_argument("--pretrimsecs", required=False, type=float, default=0, help="Optional pretrim")
parser.add_argument("-wstart", required=True, type=float, default=0, help="Zoom window start")
parser.add_argument("-wsize", required=True, type=float, default=0, help="Zoom window size")
parser.add_argument("-o", required=False, type=str, default="fig-end-to-end.svg", help="Output file name")

args = parser.parse_args()

csvFileAnalog = args.fa
csvFileDigital = args.fd
pretrimSecs = args.pretrimsecs
detailedWindowStart = args.wstart
detailedWindowSize = args.wsize
outputFile = args.o

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
net_checkpoint_time_dig_df_final = dfDigital[['Time [s]','Net Checkpoint']]
app_checkpoint_time_dig_df_final = dfDigital[['Time [s]','App Checkpoint']]
network_activity_dig_df_final = dfDigital[['Time [s]','BLE Peripheral']]
on_time_dig_df_final = dfDigital[['Time [s]','Sync','VCC']]
sync_dig_df_final = dfDigital[['Time [s]','Sync']]
fw_update_df_finished = dfDigital[['Time [s]','FW Content']]

central_network_df_final = dfDigital[['Time [s]','BLE Central']]
peripheral_network_df_final = dfDigital[['Time [s]','BLE Peripheral']]

central_array = toBarArray(central_network_df_final,'Time [s]','BLE Central', finaltime)
peripheral_array = toBarArray(peripheral_network_df_final,'Time [s]','BLE Peripheral', finaltime)

checkpoint_array = toBarArray(checkpoint_time_dig_df_final,'Time [s]','Checkpoint', finaltime)
net_checkpoint_array = toBarArray(net_checkpoint_time_dig_df_final,'Time [s]','Net Checkpoint', finaltime)
app_checkpoint_array = toBarArray(app_checkpoint_time_dig_df_final,'Time [s]','App Checkpoint', finaltime)
network_array = toBarArray(network_activity_dig_df_final,'Time [s]','BLE Peripheral', finaltime)
on_time_array = toBarArrayOnTime(on_time_dig_df_final,'Time [s]','Sync', 'VCC', finaltime, minimalBarWidth)
on_time_wide_array = toBarArrayOnTime(on_time_dig_df_final,'Time [s]','Sync', 'VCC', finaltime, minimalBarWidthFullRange)
sync_array = toSyncArray(sync_dig_df_final, 'Time [s]','Sync', finaltime)
fw_update_array = toUpdateFinishedArray(fw_update_df_finished, 'Time [s]', 'FW Content', finaltime)

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
fig = plt.figure(constrained_layout=True , figsize = (17.5*cm, 5*cm))
gs = fig.add_gridspec(10, 6)
axs = []


axs.append(fig.add_subplot(gs[0:-3, 0:6]))
axs.append(fig.add_subplot(gs[-2, 0:6]))
# axs.append(fig.add_subplot(gs[0:2, 6:8]))
# axs.append(fig.add_subplot(gs[2, 6:8]))

# axs.append(fig.add_subplot(gs[-1, 0:6]))
# axs.append(fig.add_subplot(gs[-2, 0:6]))

axs.append(fig.add_subplot(gs[-1, 0:2]))
axs.append(fig.add_subplot(gs[-1, 2:4]))
axs.append(fig.add_subplot(gs[-1, 4:6]))




# axs.append(fig.add_subplot(gs[-2, -2:]))
lightgreen = '#b2df8a'
lightpurple = '#E1D5E7'
darkpurple = '#9673A6'
darkgreen = '#33a02c'
lightblue = '#a6cee3'
darkblue = '#1f78b4'
red = '#fb9a99'
lightred = '#F8CECC'
darkgray = '#666666'
lightgray= '#F5F5F5'
gridgray='#cccccc'

max1 = 18.0
max2 = 6.0

sns.lineplot(x='Time [s]', y='VBAT', markers= False, data=vbat_df_final, ax=axs[0],markersize=2, linewidth = 0.8)
axs[0].set(xticklabels=[], xlabel="",   ylabel = "Storage (V)")
box1 = axs[0].get_position()

# vout first
# axs[1].broken_barh(fw_update_array, (0.1, 0.8), facecolors=lightblue, linewidth=0)
axs[1].broken_barh(on_time_wide_array, (0, 1), facecolors=lightgreen, linewidth=0)
axs[1].set(yticks = [], ylabel = "On/Off", xlabel= "Time (s)")
# axs[1].set_ylabel('On/Off', rotation='horizontal', va="center",labelpad=20)

print(box1)
box2 = axs[1].get_position()
# axs[1].set_position([box2.x0, box2.y0 - 0.1,
#                   box2.x1, box2.y1 -0.1])

axs[2].set(yticks = [], ylabel = "System\nOperation", xlabel= "Time (s)")
# axs[2].set(xticklabels=[], yticklabels=[], xlabel="", ylabel="")
# axs[3].set(yticks = [], xlabel= "Time (s)")

axs[2].set(yticks = [], xlabel= "(A) Application Only")
axs[2].broken_barh(on_time_array, (0, 1), facecolors=lightgreen, linewidth=0)
axs[2].broken_barh(checkpoint_array, (0.1, 0.8), facecolors=darkpurple, linewidth=0)
axs[2].broken_barh(net_checkpoint_array, (0.1, 0.8), facecolors=lightblue, linewidth=0)
axs[2].broken_barh(app_checkpoint_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)
axs[2].broken_barh(sync_array, (0.1, 0.8), facecolors=darkgray, linewidth=0)
axs[2].broken_barh(network_array, (0.1, 0.8), facecolors=darkblue, linewidth=0)
axs[2].set_xlim([199.813 - pretrimSecs, 199.860 - pretrimSecs])
axs[2].ticklabel_format(useOffset=False)

axs[3].set(yticks = [], xlabel= "(B) Network Only")
axs[3].broken_barh(on_time_array, (0, 1), facecolors=lightgreen, linewidth=0)
axs[3].broken_barh(checkpoint_array, (0.1, 0.8), facecolors=darkpurple, linewidth=0)
axs[3].broken_barh(net_checkpoint_array, (0.1, 0.8), facecolors=lightblue, linewidth=0)
axs[3].broken_barh(app_checkpoint_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)
axs[3].broken_barh(sync_array, (0.1, 0.8), facecolors=darkgray, linewidth=0)
axs[3].broken_barh(network_array, (0.1, 0.8), facecolors=darkblue, linewidth=0)
axs[3].set_xlim([119.813 - pretrimSecs, 119.860 - pretrimSecs])
axs[3].ticklabel_format(useOffset=False)

axs[4].set(yticks = [], xlabel= "(C) Combined")
axs[4].broken_barh(on_time_array, (0, 1), facecolors=lightgreen, linewidth=0)
axs[4].broken_barh(checkpoint_array, (0.1, 0.8), facecolors=darkpurple, linewidth=0)
axs[4].broken_barh(net_checkpoint_array, (0.1, 0.8), facecolors=lightblue, linewidth=0)
axs[4].broken_barh(app_checkpoint_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)
axs[4].broken_barh(sync_array, (0.1, 0.8), facecolors=darkgray, linewidth=0)
axs[4].broken_barh(network_array, (0.1, 0.8), facecolors=darkblue, linewidth=0)
axs[4].set_xlim([229.813 - pretrimSecs, 229.862 - pretrimSecs])
axs[4].ticklabel_format(useOffset=False)

axs[0].set_xlim([0, 180])
axs[1].set_xlim([0, 180])

axs[0].set_ylim([2, 2.7])
# axs[1].annotate('',
#             xy=(229.813 - pretrimSecs, -0.5), xycoords='data',
#             xytext=(155, 3), textcoords='data',
#             arrowprops={'arrowstyle': '-', 'ls': 'dashed', 'color': 'black'}, va='center')


# line = matplotlib.lines.Line2D((100,100),(1000,1000))
# fig.lines = line

# axs[4].set(yticks = [], ylabel = "Central", xlabel= "")
# axs[5].set(yticks = [], ylabel = "Peripheral", xlabel= "Time (s)")

# axs[4].broken_barh(central_array, (0.1, 0.8), facecolors=lightblue, linewidth=0)
# axs[5].broken_barh(peripheral_array, (0.1, 0.8), facecolors=darkgreen, linewidth=0)

# axs[4].set_xlim([0, 140])
# axs[5].set_xlim([0, 140])

# 20k lux
# axs[0].set_xlim([0, 120])
# axs[1].set_xlim([0, 120])
#
# axs[2].set_xlim([48.0, 51])
# axs[3].set_xlim([48.0, 51])

# 40k lux


# axs[2].set_xlim([detailedWindowStartTrimmed, detailedWindowEndTrimmed])
# axs[3].set_xlim([detailedWindowStartTrimmed, detailedWindowEndTrimmed])
# fig.suptitle('Firmware-Update | 600 lux', fontsize=16)

plt.tight_layout(pad=0.05)
# fig.subplots_adjust(hspace=.5)

trans = axs[1].get_xaxis_transform() # x in data untis, y in axes fraction

con = ConnectionPatch(xyA=(229.813 - pretrimSecs, -0.3), coordsA=axs[1].transData,
                      xyB=(229.85 - pretrimSecs,2), coordsB=axs[4].transData, arrowstyle= '-', ls= 'dashed', color=gridgray,linewidth=0.5)
fig.add_artist(con)
con = ConnectionPatch(xyA=(229.85 - pretrimSecs,2), coordsA=axs[4].transData,
                      xyB=(229.813 - pretrimSecs,1.2), coordsB=axs[4].transData, arrowstyle= '-', ls= 'dashed', color=gridgray,linewidth=0.5)
fig.add_artist(con)
con = ConnectionPatch(xyA=(229.862 - pretrimSecs, -0.3), coordsA=axs[1].transData,
                      xyB=(229.862 - pretrimSecs,1.2), coordsB=axs[4].transData, arrowstyle= '-', ls= 'dashed', color=gridgray,linewidth=0.5)
fig.add_artist(con)


axs[1].annotate('(C)',
            xy=(229.813 - pretrimSecs, 0.1), xycoords=trans,
            xytext=(229.813 - pretrimSecs, -2.0), textcoords=trans,
            arrowprops={'arrowstyle': '->', 'color': 'black','linewidth':0.5}, horizontalalignment='center', va='center')
axs[1].annotate('(B)',
            xy=(119.813 - pretrimSecs, 0.1), xycoords=trans,
            xytext=(119.813 - pretrimSecs, -2.0), textcoords=trans,
            arrowprops={'arrowstyle': '->', 'color': 'black', 'linewidth':0.5}, horizontalalignment='center', va='center')
axs[1].annotate('(A) - Update Finished',
            xy=(199.813 - pretrimSecs, 0.1), xycoords=trans,
            xytext=(199.813 - pretrimSecs, -2.0), textcoords=trans,
            arrowprops={'arrowstyle': '->', 'color': 'black','linewidth':0.5}, horizontalalignment='center', va='center')

# axs[1].annotate('Update finished',
#             xy=(199.813 - pretrimSecs, 1), xycoords=trans,
#             xytext=(199.813 - pretrimSecs, 1.5), textcoords=trans,
#             arrowprops={'arrowstyle': '->', 'ls': 'dashed', 'color': 'black'}, horizontalalignment='center', va='center')



plt.savefig(outputFile)
# plt.show()
