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

if(DEBUG)
target_compile_definitions(${PROJECT_NAME}
PRIVATE DEBUG
)
endif()

if(LFXO_NON_BLOCKING_START)
target_compile_definitions(${PROJECT_NAME}
PRIVATE LFXO_NON_BLOCKING_START
)
endif()

if(COMPARE_NO_CHECKPOINT)
target_compile_definitions(${PROJECT_NAME}
PRIVATE COMPARE_NO_CHECKPOINT
)
endif()

if(USE_AMBIQ_LFCLK)
target_compile_definitions(${PROJECT_NAME}
PRIVATE USE_AMBIQ_LFCLK
)
endif()

if(NO_SCAN_RSP)
target_compile_definitions(${PROJECT_NAME}
PRIVATE NO_SCAN_RSP
)
endif()

if(DEBUG_SYSTEMOFF)
target_compile_definitions(${PROJECT_NAME}
PRIVATE DEBUG_SYSTEMOFF
)
endif()

if(TRACE_TIMING)
target_compile_definitions(${PROJECT_NAME}
PRIVATE TRACE_TIMING
)
endif()

if(LCD_PINS_OCCUPIED)
target_compile_definitions(${PROJECT_NAME}
PRIVATE LCD_PINS_OCCUPIED
)
endif()

if(RECOVER_CONNECTIONS)
target_compile_definitions(${PROJECT_NAME}
PRIVATE RECOVER_CONNECTIONS
)
endif()