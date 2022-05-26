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

project(fw-update VERSION 1.0.0 LANGUAGES C ASM)

# Options
set(DEBUG 1)
set(USE_LTO 0)
set(TRACE 1)
set(BT_VER 8)
set(USE_EXACTLE 1)
set(INIT_PERIPHERAL 1)
# set(INIT_BROADCASTER 1)
# set(INIT_ENCRYPTED 1)
set(CHECKPOINTING 1)
set(CHECKPOINTING_TIMING 1)

set(BOARD PCA10056) # PCA10056 or PCA10040
set(BB_CLK_RATE_HZ 32768)
# set(USE_AMBIQ_LFCLK 1)
# set(NO_SCAN_RSP 1)

# set(DEBUG_SYSTEMOFF 1)

set(TRACE_TIMING 1)
# set(RECOVER_CONNECTIONS 1)

#--------------------------------------------------------------------------------------------------
#     Configuration
#--------------------------------------------------------------------------------------------------
list(APPEND CFG_DEV "BT_VER=${BT_VER}")

if(CHECKPOINTING_TIMING)
    set(DEPENDENCIES
        config
        libs/fram
        libs/debug
        libs/tdc/sync
        libs/external-rtc
        libs/twi_interface
        platforms
        third-party/cordio/thirdparty/nordic-bsp/components/libraries/delay
        ckpt-app/${PROJECT_NAME}
        )
else()
    set(DEPENDENCIES
        config
        libs/fram
        libs/debug
        # libs/bf-sync
        # libs/external-rtc
        # libs/twi_interface
        platforms
        third-party/cordio/thirdparty/nordic-bsp/components/libraries/delay
        ckpt-app/${PROJECT_NAME}
        )
endif(CHECKPOINTING_TIMING)

# Setup a list of source directories for the glob
foreach(dep ${DEPENDENCIES})
    list(APPEND DEP_SOURCES "${CMAKE_SOURCE_DIR}/${dep}/*.[cs]")
endforeach()

# List source files to be compiled
file(GLOB SOURCES
    "${PROJECT_SOURCE_DIR}/*.[cs]"
    ${DEP_SOURCES}
    )

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES})

if(USE_EXACTLE)
include(${CMAKE_SOURCE_DIR}/config/cordio-config/controller-config.cmake)
endif()

include(${CMAKE_SOURCE_DIR}/config/cordio-config/platform-config.cmake)
include(${CMAKE_SOURCE_DIR}/config/cordio-config/ble-apps-config.cmake)

include(${CMAKE_SOURCE_DIR}/config/cordio-config/BLE42-sources.cmake)

include(${CMAKE_SOURCE_DIR}/config/cordio-config/wsf-sources.cmake)
include(${CMAKE_SOURCE_DIR}/config/cordio-config/platform-sources.cmake)

if(DEBUG)
include(${CMAKE_SOURCE_DIR}/config/debug-print.cmake)
endif()

# Checkpointing must be last
if(CHECKPOINTING)
include(${CMAKE_SOURCE_DIR}/config/checkpoint/checkpointing.cmake)
else()
# Overwrite the cordio linker file
set(LD_FILE ${CMAKE_SOURCE_DIR}/config/nrf52840-fw-update-no-chpt.ld)
endif()

# Change target suffix to have <name>.out
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${OUTPUT_SUFFIX})

# Defines (-D)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE HWREV=4
    PRIVATE ${CFG_DEV}
)

include(${CMAKE_SOURCE_DIR}/config/defines.cmake)

#--------------------------------------------------------------------------------------------------
#     Compilation flags
#--------------------------------------------------------------------------------------------------

# Compiler options for this project
target_compile_options(${PROJECT_NAME}
    #PUBLIC -mfpu=fpv4-sp-d16 -mfloat-abi=hard
    PRIVATE -std=c99
    PRIVATE -Wall -Wextra -Wno-unused-parameter -Wshadow -Wno-expansion-to-defined #-Wundef
    PRIVATE -ffunction-sections -fdata-sections
    PRIVATE -fno-common -fomit-frame-pointer
    )

if(USE_LTO)
target_compile_options(${PROJECT_NAME}
    PRIVATE -flto
)
endif()

if(DEBUG)
target_compile_options(${PROJECT_NAME}
    PRIVATE -O0 -g
)
else()
target_compile_options(${PROJECT_NAME}
    PRIVATE -Os -g
)
endif()

# Include directories of libraries and externals
foreach(dep ${DEPENDENCIES})
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}/${dep}/
        PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/controller/include/common
        PRIVATE ${PROJECT_SOURCE_DIR}/config
        PRIVATE ${PROJECT_SOURCE_DIR}
        )
endforeach()

# Linker options for this project
target_link_options(${PROJECT_NAME}
    #PUBLIC -mfpu=fpv4-sp-d16 -mfloat-abi=hard
    PRIVATE -Wl,--gc-sections
    PRIVATE -Wl,-Map=${PROJECT_NAME}.map
    PRIVATE -T ${LD_FILE}
    PRIVATE -L${CMAKE_SOURCE_DIR}/config
    )

target_link_libraries(${PROJECT_NAME}
    gcc
    c
    m
    )

# Print size of binary after linking
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}${OUTPUT_SUFFIX}
    )

# generate the binary file and the intel hex file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}${OUTPUT_SUFFIX} ${PROJECT_NAME}.bin
    )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}${OUTPUT_SUFFIX} ${PROJECT_NAME}.hex
    )

# Install executable in Botoks/bin
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
