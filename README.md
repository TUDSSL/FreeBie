# Battery-free Bluetooth That Works
<img src="https://github.com/TUDSSL/FreeBie/blob/master/Images/app.jpg" width="500" height="250">

## Abstract
We present an architecture for intermittently-powered wireless communication systems that does not require any changes to the official protocol specification. Our core idea is to save the intermediate state of the wireless protocol to non-volatile memory within each connection interval. The protocol state is then deterministically restored at a predefined (harvested energy-dependent) time, which follows the connection interval. As a case study for our architecture, we introduce FreeBie: a battery-free intermittently-powered Bluetooth Low Energy (BLE) mote. To the best of our knowledge FreeBie is the first battery-free active wireless system that sustains bi-directional communication on intermittent harvested energy. The strength of our architecture is articulated by FreeBie consuming at least 9.5 times less power during device inactivity periods than a state-of-the-art BLE device.

<!-- ![Intermittency Introduction](https://github.com/TUDSSL/FreeBie/blob/master/Images/intro.svg) -->
<!-- ![System Diagram](https://github.com/TUDSSL/FreeBie/blob/master/Images/diagram.svg) -->

## Artifact
We describe the process to reproduce the results presented in the paper.

### Repository structure
The diagram below describes structure and purpose of the most important directories of the FreeBie project.
* * *
 * Hardware
   * design: KiCad design files
   * schematic: PDF schematic
   * manufacturing: Bill of materials, Gerber files (per version)
 * Software
   * bin: Location of compiled application binaries
   * build: Location of build files
   * ckpt-app: Application process files per application
     * fw-update
     * smartwatch
     * template
   * ckpt-os: OS process files
   * config: CMake and project configuration files
   * libs: Drivers for external system components
   * platforms: Pin definitions
   * third-party
     * Cordio: Network process files
 * Evaluation
   * hosts
     * fw-update: Firmware for the firmware update host
     * smartwatch: Android app for smartwatch host
     * end-device
       * softdevice: Power comparison application
   * Plotting
     * plot.sh: Script generating all the plots in the paper
     * *.py: Figure-specific Python script invoked by plot.sh
 * Measurements
     * raw: Raw measurements
     * converted: Measurements converted to CSV format
     * traces: 24H trace and replay script
* * *

### Evaluation Setup
#### Code Generation and Programming Tools.

To compile FreeBie runtime we used the [GNU Arm Embedded Toolchain (9.3.1)](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) and [CMake (3.19.1)](https://cmake.org) to build the project. To program the MCU we used the [J-Link debug probe](https://www.segger.com/products/debug-probes/j-link/models/j-link-edu) with the accompanying J-Link software. We used Linux in order compile and evaluate FreeBie. In this artifact, only Linux instructions are provided, although (untested) Windows or Mac OS could also be used to build the system.

#### Trace Recording Tools.

We used [Saleae Logic Pro 16 logic analyzer](http://downloads.saleae.com/specs/Logic+Pro+16+Product+Fact+Sheet.pdf), to record on-device FreeBie signals. To capture and analyze over-the-air master/end-device BLE packets exchanges we used [Nordic Semiconductors nRF sniffer](https://www.nordicsemi.com/Products/Development-tools/nRF-Sniffer-for-Bluetooth-LE) and [Wireshark](https://www.wireshark.org}) as protocol analyzer.

#### Controlled Test Environment.

We put FreeBie at the bottom of a closed light box. A wirelessly-controlled [LED bulb](https://www.philips-hue.com/en-gb/p/hue-white-ambiance-1-pack-e27/8718699673147) was installed at the top of the box to create repeatable and controlled light source. A [luminosity meter](https://www.uni-trend.com/html/product/Environmental/Environmental_Tester/Mini/UT383.html) was placed next to FreeBie mote to measure the exact luminosity projecting onto FreeBie's solar panels.

#### Power Measurement Process.

Connection event as well as the sleep power consumption for the [Packetcraft BLE network stack](https://github.com/packetcraft-inc/stacks), Nordic Semiconductors BLE SoftDevice and our intermittently-powered BLE stack were measured using a [STMicroelectronics X-NUCLEO-LPM01A power measurement board](https://www.st.com/en/evaluation-tools/x-nucleo-lpm01a.html). FreeBie's power consumption whilst the processor domain is off was measured with [Keithley 2450 SMU](https://download.tek.com/datasheet/1KW-60904-2_2450_Datasheet_072021.pdf). Power consumption during sleep and power off was measured at 1.8V. Power consumption of Packetcraft and SoftDevice is measured on the NRF52840 development kit, FreeBie power measurements are measured on the FreeBie mote. The BLE connection event power consumption for Packetcraft and SoftDevice was measured at a 1.8V, while BLE connection event for FreeBie was measured at 2.67V. We measure the connection event after the device has been running for five minutes, selecting a connection event with an empty PDU. The consumed energy results presented in Figure 12 Consumed Energy are computed as the sum of the energy consumed during the selected connection event and during the idle period (power off for FreeBie and sleep for Packetcraft and SoftDevice.).

#### FreeBie Hardware.

In order to reproduce the results of the paper a FreeBie mote is required. In the *Hardware* folder of the repository, all files to reproduce the mote are provided, such as the design files, Gerber files and the bill of materials.

### Building the FreeBie Software

In order to build the FreeBie software, from the *Software/build* directory the following commands need to be executed.

```bash
$ cmake ..
$ make install
```

As a result of these operations, the project is compiled and the resulting programmable binaries of the applications are moved to the *Software/bin* directory. Next, the compiled binary of the application that is being tested should be flashed to the FreeBie mote.

#### Programming the FreeBie Mote.

In order to assist with programming the FreeBie mote during intermittent operation, a button is placed next to the programming header on the mote. When this button is pressed during startup of the MCU, the MCU remains in a perpetual loop, allowing the device to be programmed. Note that this button only works during the start up of the MCU and during operation the mote might be unable to be flashed as the power to the MCU may be switched off.

## Results Reproduction

### Smartwatch Application

In order to reproduce the results related to the smartwatch application please make sure that the smartwatch binary is programmed to the FreeBie mote first.

#### Building the Android Application.

In order to build the Android application, open the project (located in *evaluation/hosts/smartwatch*) in the Android Studio and then proceed with building the APK. Then a *.apk* file should be available in the *evaluation/hosts/smartwatch/build/outputs/apk* directory of the project. The APK must then be installed on the phone through Android Studio or transferred to the phone and installed manually. When opening the app for the first time, it will ask for permissions that must be granted in order for the app to be able to function. For more information please refer to the [Android Developer website]().

#### Measurement.

First the logic analyzer is connected to the testpoints on the FreeBie mote. These testpoints provide the tracing information about the system state used to generate Figure 11 FreeBie smartwatch operation. For the measurement, the programmed FreeBie mote with the smartwatch binary is placed in the light box. The phone should be placed within range of the FreeBie mote for the devices to be able to connect. Next the variable light should be set to output the amount of light specified in the desired scenario to be replicated.

When the light is switched on the FreeBie mote starts harvesting energy and when enough energy is harvested the FreeBie mote should automatically switch on. The smartphone should then automatically connect to the FreeBie mote and time should appear on the LCD screen.

#### 24 hour experiment.

For the 24 hour experiment the same procedure as before is used, but instead of a static lighting, a script (available in the *Measurements/traces* directory) is used to replay a prerecorded luminosity trace of a person wearing a smartwatch for 24 hours. In order to execute the Python script, first the dependencies have to be installed, assuming Python is already installed. For this the following command needs to be executed.

```bash
$ cd Measurements/traces
$ pip install -r requirements.txt
```

Second the tool communicating with the light has to be installed and configured. Please follow the instructions on the website of the [tool](https://github.com/bahamas10/hue-cli). After the software is able to control the light, the script can be executed, replaying 24H of the prerecorded light trace.

```bash
$ python replay_lux.py -f \
  logger-2021-11-20_09-36-46-549.csv -skips 5
```

### Firmware Update Application

We first note that in order to reproduce the results presented in the smartwatch application please make sure the *fw-update* binary is programmed to the FreeBie mote.

#### Building and Flashing the Host Firmware.

First plug in the development kit and make sure that is is powered and detected. Then from the main project directory execute the following command.

```bash
$ cd Evaluation/hosts/          \
     fw-update/  \
     nRF5_SDK_17.0.2/           \
     examples/ble_central/      \
     ble_app_custom_c/          \
     pca10056/s140/armgcc
$ make
$ make flash_softdevice
$ make flash
```

The development kit should now be flashed with the correct firmware. Turn the development kit off and on. Now it is ready to start the firmware update as soon as the FreeBie mote is detected.

#### Measurement.

In order to generate the Evaluation of firmware update plot in FreeBie Evaluation section, the logic analyzer is connected to the test points on the FreeBie mote. These test points provide the tracing information about the system state used to generate Figure 13 Evaluation of firmware update. For the measurement, the programmed FreeBie mote with the *fw-update* binary is placed in the light box. The development kit, acting as firmware update host, should be placed within range of the FreeBie mote for the devices to be able to connect. For the firmware update application we did not connect the display to the FreeBie mote.

When the light is switched on the FreeBie mote starts harvesting energy and when enough energy is harvested the FreeBie mote should automatically switch on. The firmware update host should connect to the FreeBie mote and the firmware update should start automatically.

### Reference Comparison

In order to produce figures 8, 9 and 12 we used a minimal working example (named *template*) of our software that only actively uses the network process and the OS process. In this application after the BLE connection is established and the database discovery has been completed, only empty packets are sent. Hence, this forms a point of reference for other experiments.

#### Template Configuration.

From within the *CMakefile* file of *template*, checkpointing can be switched off by commenting the following lines:
```CMake
set(CHECKPOINTING 1)
set(CHECKPOINTING_TIMING 1)
```
Without checkpointing enabled this example serves as the Packetcraft comparison point and is representative of traditional operation; with checkpointing enabled---as FreeBie. When the checkpointing is active, a connection update request to the selected parameters is sent after connecting. To force the same behavior for the non-checkpointing version, the line *set(COMPARE_NO_CHECKPOINT 1)* needs to be uncommented. When modifying the CMakefiles, the code has to be recompiled with the same steps as described in Section Building the FreeBie Software.

#### Intermittent versus Non-intermittent.

In order to reproduce the results in Figure 8 the FreeBie mote is again placed in a light box at the specified light level and the information about the system state are recorded by the logic analyzer. The experiment is ran for both checkpointing enabled and checkpointing disabled. The non-checkpointed version runs out of energy and not be able to recover the connection as the system state is lost.

#### Connection Recovery.

For the results in Figure 9, the FreeBie mote is not powered by the solar panels but through an external power supply connected to the test point *VSol* on the back of the FreeBie mote. In this case *template* with connection recovery enabled (*set(RECOVER_CONNECTIONS 1)*) and checkpointing enabled is used. After a connection to the FreeBie mote has been established, the power to the device is disabled until the device suffers from a power failure. Then power is resumed and the device attempts recovery.

#### Power Measurement.

For the results in Figure 12 *template* is used with checkpointing disabled, serving as the Packetcraft comparison point; with checkpointing enabled---as FreeBie. As the comparison point of SoftDevice the code provided in *Evaluation/end-device/softdevice* is used. From the main project directory the code can be compiled and flashed using the following commands.

```bash
$ cd Evaluation/end-device/softdevice/ \
     examples/ble_peripheral/ \
     ble_app_pwr_profiling/ \
     pca10056/s140/armgcc
$ make
$ make flash_softdevice
$ make flash
```

Please refer to the Evaluation Setup Section for more information about the power measurement process.

### Plotting

By executing the *plot.sh* script in the *Plotting* directory all of the plots can be generated. Please note that due to the size of the 24 hour plot this might take more than 15 minutes and requires a system with at least 16GB of RAM. All plotting scripts use Python and assume Python is installed on the system; all Python dependencies can be installed via the *requirements.txt* file using the following commands.

Since the measurements files are large they are only included in the version of the artifact hosted on [Zenodo](TODOLINK)

```bash
$ cd Plotting
$ pip install -r requirements.txt
$ plot.sh
```

## License
The Project adopts the MIT license unless noted otherwise, As Packetcraft licenses its software stack under the Apache 2.0 license our code in the folder Software also adopts this license unless noted otherwise.
