import csv
import matplotlib.pyplot as plt
import matplotlib
import numpy as np
import seaborn as sns; sns.set()
import math
import pandas as pd
import argparse

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
            barArray.append([starttime, barwidth]) # widen sync pulse a bit
            starttime = 0
            started = False
            # print(str(starttime) + " stopped")
    if(starttime != 0):
        print(starttime, finaltime)
        barArray.append([starttime, finaltime - starttime])
    return barArray

parser = argparse.ArgumentParser(description='Plotting script')
parser.add_argument("-fdsmart", required=True, type=str, help="Digital filename")
parser.add_argument("-fdfw", required=True, type=str, help="Digital filename")
parser.add_argument("-o", required=False, type=str, default="fig-ckpt-time.pdf", help="Output file name")

args = parser.parse_args()

csvFileDigitalFw = args.fdfw
csvFileDigitalSmart = args.fdsmart
outputFile = args.o

#pretrim is fixed for the 300lux plot!

pretrimSecs=92.0

# Digital stuff
dfDigitalFirmware = pd.read_csv(csvFileDigitalFw)
# saved_column = df.column_name #you can also use df['column_name']
finaltime = float(dfDigitalFirmware.tail(1)['Time [s]'].astype(float))
print("Total data length digital = " + str(finaltime) + " seconds")

dfDigitalFirmware = dfDigitalFirmware[dfDigitalFirmware['Time [s]'] > pretrimSecs]
dfDigitalFirmware['Time [s]'] = dfDigitalFirmware['Time [s]'].subtract(pretrimSecs)
print("Pre-trimmed " + str(pretrimSecs) + " seconds")
dfDigitalFirmware = dfDigitalFirmware.reset_index()

checkpoint_time_dig_df_final = dfDigitalFirmware[['Time [s]','Checkpoint']]
net_checkpoint_time_dig_df_final = dfDigitalFirmware[['Time [s]','Net Checkpoint']]
app_checkpoint_time_dig_df_final = dfDigitalFirmware[['Time [s]','App Checkpoint']]

checkpoint_array = toBarArray(checkpoint_time_dig_df_final,'Time [s]','Checkpoint', finaltime)
net_checkpoint_array = toBarArray(net_checkpoint_time_dig_df_final,'Time [s]','Net Checkpoint', finaltime)
app_checkpoint_array = toBarArray(app_checkpoint_time_dig_df_final,'Time [s]','App Checkpoint', finaltime)

# Go through all checkpoints if net or app overlap remove it from the checkpoint time to result in os checkpoint time
for combinedcheckpoint in checkpoint_array:
    for netcheckpoint in net_checkpoint_array:
        if netcheckpoint[0] >= combinedcheckpoint[0] and netcheckpoint[0] < (combinedcheckpoint[0] + combinedcheckpoint[1]):
            combinedcheckpoint[1] = combinedcheckpoint[1] - netcheckpoint[1]
    for appcheckpoint in app_checkpoint_array:
        if appcheckpoint[0] >= combinedcheckpoint[0] and appcheckpoint[0] < (combinedcheckpoint[0] + combinedcheckpoint[1]):
            combinedcheckpoint[1] = combinedcheckpoint[1] - appcheckpoint[1]

osCheckpoints = np.array(checkpoint_array[1::2]) * 1000 # Elements from list1 starting from 1 iterating by 2
osRestore = np.array(checkpoint_array[::2]) * 1000 # Elements from list1 starting from 0 iterating by 2

netCheckpoints = np.array(net_checkpoint_array[1::2]) * 1000 # Elements from list1 starting from 1 iterating by 2
netRestore = np.array(net_checkpoint_array[::2]) * 1000 # Elements from list1 starting from 0 iterating by 2

appCheckpoints = np.array(app_checkpoint_array[1::2]) * 1000 # +0.110 # Just to make it visible
appRestore = np.array(app_checkpoint_array[::2]) * 1000 # Elements from list1 starting from 0 iterating by 2

avgFullCheckpointTime = np.average(osCheckpoints[:,1]) + np.average(netCheckpoints[:,1]) + np.average(appCheckpoints[:,1])
avgAppOnlyCheckpoint = np.average(osCheckpoints[:,1]) + np.average(appCheckpoints[:,1])

print("fwUpdate: Full restore = " + str(avgFullCheckpointTime) + 'ms average')
print("fwUpdate: app only restore = " + str(avgAppOnlyCheckpoint) + 'ms average')
print("fwUpdate: speedup compared to full(naive) restore = " + str((avgFullCheckpointTime/avgAppOnlyCheckpoint -1 ) * 100) + '% on average')

print(np.average(appCheckpoints[:,1]))
firmwarecheckpointlist = [osCheckpoints[:,1].tolist(), netCheckpoints[:,1].tolist(), appCheckpoints[:,1].tolist()]
firmwaredf = pd.DataFrame(firmwarecheckpointlist).transpose()
firmwaredf.columns=['OS','Network','Application']
firmwaredf.insert(0, "TestApplication", 'Firmware update')
print(firmwaredf)





pretrimSecs=95.0

# Digital stuff
dfDigitalSmart = pd.read_csv(csvFileDigitalSmart)
# saved_column = df.column_name #you can also use df['column_name']
finaltime = float(dfDigitalSmart.tail(1)['Time [s]'].astype(float))
print("Total data length digital = " + str(finaltime) + " seconds")

dfDigitalSmart = dfDigitalSmart[dfDigitalSmart['Time [s]'] > pretrimSecs]
dfDigitalSmart['Time [s]'] = dfDigitalSmart['Time [s]'].subtract(pretrimSecs)
print("Pre-trimmed " + str(pretrimSecs) + " seconds")
dfDigitalSmart = dfDigitalSmart.reset_index()

checkpoint_time_dig_df_final = dfDigitalSmart[['Time [s]','Checkpoint']]
network_on_time = dfDigitalSmart[['Time [s]','BLE Peripheral']]
app_on_time = dfDigitalSmart[['Time [s]','App']]

checkpoint_array = toBarArray(checkpoint_time_dig_df_final,'Time [s]','Checkpoint', finaltime)
network_on_time_array = toBarArray(network_on_time,'Time [s]','BLE Peripheral', finaltime)
app_on_time_array = toBarArray(app_on_time,'Time [s]','App', finaltime)

index = 0
prevcheckpoint = 0

checkpointAppOnly = []
checkpointNetOnly = []
checkpointCombined = []

for combinedcheckpoint in checkpoint_array:
    if ((index == 1) and prevcheckpoint != 0):
        index = 0
        appcycle = False
        networkCycle = False
        for network in network_on_time_array:
            if network[0] >= prevcheckpoint[0] and network[0] < (combinedcheckpoint[0] + combinedcheckpoint[1]):
                #network cycle
                networkCycle = True
        for app in app_on_time_array:
            if app[0] >= prevcheckpoint[0] and app[0] < (combinedcheckpoint[0] + combinedcheckpoint[1]):
                #app cycle
                appcycle = True;
        if(appcycle and networkCycle):
            # print("Combined cycle start:" + str(prevcheckpoint[0]) + "length: " + str(prevcheckpoint[1]))
            checkpointCombined.append(combinedcheckpoint[1])
        elif(appcycle):
            # print("Application cycle start:" + str(prevcheckpoint[0]) + "length: " + str(prevcheckpoint[1]))
            checkpointAppOnly.append(combinedcheckpoint[1])
        elif(networkCycle):
            # print("Network cycle start:" + str(prevcheckpoint[0]) + "length: " + str(prevcheckpoint[1]))
            checkpointNetOnly.append(combinedcheckpoint[1])
    else:
        index = 1
        prevcheckpoint = combinedcheckpoint



combinedCheckpoints = np.array(checkpoint_array[1::2]) # Elements from list1 starting from 1 iterating by 2
combinedRestore = np.array(checkpoint_array[::2])  # Elements from list1 starting from 0 iterating by 2

checkpointCombinednp = np.array(checkpointCombined)  # Elements from list1 starting from 1 iterating by 2
checkpointAppOnlynp = np.array(checkpointAppOnly)  # Elements from list1 starting from 0 iterating by 2
checkpointNetOnlynp = np.array(checkpointNetOnly)  # Elements from list1 starting from 0 iterating by 2

combinedAverage = np.average(checkpointCombinednp)* 1000
appOnlyAverage = np.average(checkpointAppOnly) * 1000
netOnlyAverage = np.average(checkpointNetOnly) * 1000

print(combinedAverage, appOnlyAverage, netOnlyAverage)
networkAverage = combinedAverage - appOnlyAverage
appAverage = combinedAverage - netOnlyAverage
osAverage = combinedAverage - appAverage - networkAverage
print("Network " + str(networkAverage) + "App " + str(appAverage) + "OS " + str(osAverage))

avgwatchFullCheckpointTime = networkAverage + appAverage + osAverage
avgwatchAppOnlyCheckpoint = osAverage + appAverage
avgwatchNetworkOnlyCheckpoint = osAverage + networkAverage

print("Smartwatch: Full restore = " + str(avgwatchFullCheckpointTime) + 'ms average')
print("Smartwatch: app only restore = " + str(avgwatchAppOnlyCheckpoint) + 'ms average')
print("Smartwatch: Net only restore = " + str(avgwatchNetworkOnlyCheckpoint) + 'ms average')
print("Smartwatch: app speedup compared to full(naive) restore = " + str((avgwatchFullCheckpointTime/avgwatchAppOnlyCheckpoint -1 ) * 100) + '% on average')
print("Smartwatch: net speedup compared to full(naive) restore = " + str((avgwatchFullCheckpointTime/avgwatchNetworkOnlyCheckpoint -1 ) * 100) + '% on average')

smartcheckpointlist = [osAverage, networkAverage,appAverage]
# print(smartcheckpointlist)
smartdf = pd.DataFrame(smartcheckpointlist).transpose()
smartdf.columns=['OS','Network','Application']
smartdf.insert(0, "TestApplication", 'Smartwatch')
print(smartdf)



df = firmwaredf.append(smartdf, ignore_index = True, sort=False)
tidy = df.melt(id_vars='TestApplication').rename(columns=str.title)
# print(tidy.to_string())



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
fig = plt.figure(constrained_layout=True , figsize = (8.5*cm, 3.5*cm))


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

ax = sns.barplot(
            data=tidy,x='Testapplication',y='Value',ci=None,hue='Variable')
ax.set(xlabel='', ylabel='Checkpoint time (ms)')

for l in ax.lines:
    print(l.get_linewidth())
    plt.setp(l,linewidth=0.5)

ax.set_ylim([0, 8])
#
# remove label on legend
ax.legend_.set_title(None)
plt.tight_layout(pad=0.05)
plt.savefig(outputFile)
# plt.show()
