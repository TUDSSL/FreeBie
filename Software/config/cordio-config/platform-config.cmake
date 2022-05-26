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

# Board
if (NOT BOARD)
set(BOARD PCA10056)
endif()

set(AUDIO_CAPE 0)
set(MESH_CAPE 0)
list(APPEND CFG_DEV "uECC_ASM=2")

# Chip
if (${BOARD} STREQUAL PCA10056)
set(CPU cortex-m4)
set(CHIP NRF52840_XXAA)
endif()
if (${BOARD} STREQUAL PCA10040)
set(CPU cortex-m4)
set(CHIP NRF52832_XXAA)
endif()
if (${BOARD} STREQUAL NRF6832)
set(CPU cortex-m4)
set(CHIP NRF52832_XXAA)
endif()

# Options
if(NOT BAUD)
set(BAUD 1000000)
endif()
if(NOT HWFC)
set(HWFC 1)
endif()
if(NOT CODEC)
set(CODEC BLUEDROID)
endif()

#--------------------------------------------------------------------------------------------------
#     Configuration
#--------------------------------------------------------------------------------------------------

# Core
list(APPEND CFG_DEV "${CHIP}")
if (CHIP STREQUAL NRF51422)
list(APPEND CFG_DEV "NRF51")
endif()

# Peripherals
if(BB_CLK_RATE_HZ)
list(APPEND CFG_DEV "BB_CLK_RATE_HZ=${BB_CLK_RATE_HZ}")
endif()
list(APPEND CFG_DEV "BB_ENABLE_INLINE_ENC_TX=1")
list(APPEND CFG_DEV "UART_BAUD=${BAUD}")
list(APPEND CFG_DEV "UART_HWFC=${HWFC}")
list(APPEND CFG_DEV "${CHCI_TR}")

# Board
list(APPEND CFG_DEV "BOARD_${BOARD}=1")
list(APPEND CFG_DEV "AUDIO_CAPE=${AUDIO_CAPE}")
list(APPEND CFG_DEV "MESH_CAPE=${MESH_CAPE}")

# Audio
list(APPEND CFG_DEV "CODEC_${CODEC}=1")

#--------------------------------------------------------------------------------------------------
#     Sources
#--------------------------------------------------------------------------------------------------

# Linker file
if (CHIP STREQUAL NRF52840_XXAA)
set(LD_FILE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/nrf52840.ld)
endif()
if (CHIP STREQUAL NRF52832_XXAA)
set(LD_FILE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/nrf52832.ld)
endif()
if (CHIP STREQUAL NRF51422)
set(LD_FILE ${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/nrf51822.ld)
endif()
