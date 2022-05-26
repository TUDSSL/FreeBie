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

target_sources(${PROJECT_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util/app_error_handler_gcc.c
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util/app_error.c
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util/app_util_platform.c
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util/nrf_assert.c
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util/sdk_mapped_flags.c
)

target_sources(${PROJECT_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/segger_rtt/SEGGER_RTT_printf.c
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/segger_rtt/SEGGER_RTT.c
 )

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/segger_rtt
  PRIVATE ${CMAKE_SOURCE_DIR}/third-party/cordio/thirdparty/nordic-bsp/components/libraries/util
)