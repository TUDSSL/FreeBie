import csv
import numpy as np
import math
import pandas as pd
import argparse
import subprocess
import time
from alive_progress import alive_bar

from bisect import bisect_left
import shlex
import pause

calibrationLuxTable = [14, # 1%
16,
21,
26,
35,
42,
60,
71,
92,
105,
130,
148,
180,
211,
232,
271,
296,
335,
366,
410,
442,
491,
529,
581,
641,
681,
743,
786,
852,
900,
970,
1021,
1100,
1154,
1236,
1294,
1382,
1471,
1534,
1628,
1692,
1805,
1873,
1980,
2053,
2164,
2242,
2357,
2434,
2558,
2681,
2764,
2894,
2980,
3114,
3205,
3345,
3438,
3582,
3678,
3824,
3924,
4078,
4233,
4339,
4497,
4607,
4774,
4885,
5053,
5172,
5345,
5463,
5645,
5830,
5954,
6140,
6264,
6455,
6590,
6790,
6922,
7126,
7262,
7467,
7609,
7824,
8038,
8182,
8400,
8550,
8780,
8924,
9160,
9295,
9531,
9687,
9932,
10090,
10340 # 100%
]

# https://stackoverflow.com/questions/12141150/from-list-of-integers-get-number-closest-to-a-given-value
def take_closest(myList, myNumber):
    """
    Assumes myList is sorted. Returns index of value closest to myNumber.

    If two numbers are equally close, return the smallest number.
    """
    pos = bisect_left(myList, myNumber)
    if pos == 0:
        return 0
    if pos == len(myList):
        return len(myList)
    before = myList[pos - 1]
    after = myList[pos]
    if after - myNumber < myNumber - before:
        return pos
    else:
        return pos-1

def translateLuxToPercent(lux):
    return take_closest(calibrationLuxTable, lux) + 1

parser = argparse.ArgumentParser(description='Plotting script')
parser.add_argument("-f", required=True, type=str, help="input file")
parser.add_argument("-fc", required=True, type=str, help="hue starting config file")
parser.add_argument("-o", required=False, type=str, default="ouput.pdf", help="Output file name")

parser.add_argument("-skips", type=int, default=0, help="hue starting config file")

args = parser.parse_args()

inputFile = args.f
hueConfigFile = args.fc
outputFile = args.o

numSkips = args.skips

df = pd.read_csv(inputFile,header=None, usecols=[0,1,2,3,4,5], names=['TimeMS', 'TimeBootNS', 'SensorName', 'Data0', 'Data1', 'Data2'])

# select only the light data
df = df[df["SensorName"] == "OPT3007 Light"]

# print(df)
startingTime = df["TimeMS"].iloc[0]
print("Starting time: " + str(startingTime))

startingHueConfigCommand = "hue lights 1 state < " + hueConfigFile

print('Preparing setup...')
print("hue lights on")
# shellCommand = shlex.split()
subprocess.run("hue lights on", stdout=subprocess.PIPE, shell=True)
time.sleep(1)
print(startingHueConfigCommand)
# shellCommand = shlex.split()
subprocess.run(startingHueConfigCommand, stdout=subprocess.PIPE, shell=True)
time.sleep(1)

currentTime = startingTime
print('Starting payback...')

localTimeMs = round(time.time())

skips = numSkips
with alive_bar(len(df.index)) as bar:
    for index, row in df.iterrows():
        if(skips):
            skips = skips -1
            bar()
        else:
            skips = numSkips
            # Sleep until sample time
            # print("Next event in: " + str(row["TimeMS"] - currentTime))

            pause.until(localTimeMs + (row["TimeMS"] - startingTime)/1000)

            # time.sleep((row["TimeMS"] - currentTime)/1000)
            # currentTime = row["TimeMS"]

            # Send out the command
            # print("Current lux: "+ str(row["Data0"]))
            command = "hue lights 1 =" + str(translateLuxToPercent(row["Data0"])) + "%"
            # print(command)
            subprocess.run(command, stdout=subprocess.PIPE, shell=True)
            bar()
