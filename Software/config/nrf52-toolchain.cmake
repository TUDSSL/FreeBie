#/*************************************************************************************************/
#/*!
# *  Copyright (c) 2022 Delft University of Technology.
# *  Jasper de Winkel, Haozhe Tang and Przemyslaw Pawelczak
# *
# *  Licensed under the Apache License, Version 2.0 (the "License");
# *  you may not use this file except in compliance with the License.
# *  You may obtain a copy of the License at
# *
# *      http://www.apache.org/licenses/LICENSE-2.0
# *
# *  Unless required by applicable law or agreed to in writing, software
# *  distributed under the License is distributed on an "AS IS" BASIS,
# *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# *  See the License for the specific language governing permissions and
# *  limitations under the License.
# */
#/*************************************************************************************************/

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(DEVICE "nrf52" CACHE STRING "")
set(CPU "cortex-m4" CACHE STRING "")

set(OUTPUT_SUFFIX ".elf" CACHE STRING "")

set(CMAKE_C_COMPILER    "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER  "arm-none-eabi-g++")
set(CMAKE_AR            "arm-none-eabi-ar")
set(CMAKE_LINKER        "arm-none-eabi-ld")
set(CMAKE_NM            "arm-none-eabi-nm")
set(CMAKE_OBJCOPY       "arm-none-eabi-objcopy")
set(CMAKE_OBJDUMP       "arm-none-eabi-objdump")
set(CMAKE_STRIP         "arm-none-eabi-strip")
set(CMAKE_RANLIB        "arm-none-eabi-ranlib")
set(CMAKE_SIZE          "arm-none-eabi-size")

# General compiler flags
add_compile_options(
    -mthumb -mlittle-endian # -mabi=aapcs
    -mcpu=${CPU}
    -Dgcc
    )

# Device linker flags
add_link_options(
    -mthumb # -mabi=aapcs
    -mcpu=${CPU}
    -specs=nano.specs -specs=nosys.specs
    )
