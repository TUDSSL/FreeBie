python smart-watch-plot.py -fa ../Measurements/converted/smart-watch-300lux-analog.csv -fd ../Measurements/converted/smart-watch-300lux-digital.csv --pretrimsecs 94 -wstart 157.70 -wsize 0.18 -o smart-watch-300lux.pdf -skipxlabel 1 &
python smart-watch-plot.py -fa ../Measurements/converted/smart-watch-600lux-analog.csv -fd ../Measurements/converted/smart-watch-600lux-digital.csv --pretrimsecs 44.48 -wstart 76 -wsize 0.17 -o smart-watch-600lux.pdf -skipxlabel 1 &
python smart-watch-plot.py -fa ../Measurements/converted/smart-watch-10klux-analog.csv -fd ../Measurements/converted/smart-watch-10klux-digital.csv --pretrimsecs 13.6 -wstart 47.25 -wsize 0.10 -o smart-watch-10klux.pdf &

python fw-update-plot.py -fa ../Measurements/converted/fw-update-600lux-analog.csv -fd ../Measurements/converted/fw-update-600lux-digital.csv --pretrimsecs 55 -wstart 65.81 -wsize 0.06 -o fw-update-600lux.pdf &

python connection-compare.py -fa1 ../Measurements/converted/template-non-intermittent-200lx-analog.csv -fd1 ../Measurements/converted/template-non-intermittent-200lx-digital.csv -fa2 ../Measurements/converted/template-intermittent-200lx-analog.csv -fd2 ../Measurements/converted/template-intermittent-200lx-digital.csv --pretrimsecs1 20 --pretrimsecs2 45 -o connection-cordio.pdf &

python power-consumption.py -f ../Measurements/converted/power-consumption.csv &

python measure-ckpt-res-times.py -fdfw ../Measurements/converted/fw-update-300lux-digital.csv -fdsmart ../Measurements/converted/smart-watch-300lux-digital.csv &

python recovery-plot.py -fa ../Measurements/converted/intermittent-recovery-2-analog.csv -fd ../Measurements/converted/intermittent-recovery-2-digital.csv  --pretrim 11 &

python 24h-plot.py -f ../Measurements/converted/logger-2021-11-20_09-36-46-549.csv -fanalog ../Measurements/converted/intermittent-24h-10x-downsample.csv --averages 100 -o 24h-plot-avg-100.pdf & 
wait
