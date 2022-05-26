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

list(APPEND CFG_DEV "CHECKPOINT")

# Checkpointing source folders
set(CHECKPOINT_DEPENDENCIES
    libs/tdc/checkpoint
    libs/tdc/checkpoint/arch/arm
    libs/tdc/checkpoint/arch/arm/reg
    libs/tdc/checkpoint/arch/arm/reg
    libs/tdc/checkpoint/code-id
    libs/tdc/checkpoint/bss
    libs/tdc/checkpoint/data
    libs/tdc/checkpoint/stack
    libs/tdc/checkpoint/heap
    libs/tdc/checkpoint/network
    libs/tdc/checkpoint/application
    config/checkpoint
    )
if(CHECKPOINTING_TIMING)
list(APPEND CHECKPOINT_DEPENDENCIES libs/tdc/checkpoint/timing)
list(APPEND CFG_DEV "CHECKPOINT_TIMING")
endif()

# Make a directory list for the glob
foreach(dep ${CHECKPOINT_DEPENDENCIES})
    list(APPEND CHECKPOINT_DEP_SOURCES "${CMAKE_SOURCE_DIR}/${dep}/*.[cs]")
endforeach()

# List source files to be compiled
file(GLOB CHECKPOINT_SOURCES
    ${CHECKPOINT_DEP_SOURCES}
    )

# Add the checkpointing sources to the project
target_sources(${PROJECT_NAME}
    PRIVATE ${CHECKPOINT_SOURCES}
    )

# Include checkpointing directories
foreach(dep ${CHECKPOINT_DEPENDENCIES})
    target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}/${dep}/
        )
endforeach()

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/third-party/cordio/platform/targets/nordic/build/startup_gcc_nrf52840.c")

# Overwrite the cordio linker file
set(LD_FILE ${CMAKE_SOURCE_DIR}/config/nrf52840-chpt.ld)

# Always regenerate the code ID used to (in)validate checkpoint restores
set(CODE_ID_SRC "libs/tdc/checkpoint/code-id/code_id.c")

add_custom_target(${PROJECT_NAME}-rebuild-code-id ALL
                  COMMAND ${CMAKE_COMMAND} -E touch
                  ${CMAKE_SOURCE_DIR}/${CODE_ID_SRC})

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-rebuild-code-id)
