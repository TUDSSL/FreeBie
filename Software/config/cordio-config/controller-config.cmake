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

if (NOT BB_CLK_RATE_HZ)
set(BB_CLK_RATE_HZ 1000000)
endif()

# Suggest UART transport for controller; platform may select alternate
set(CHCI_TR CHCI_TR_UART=1)

set(RTOS baremetal) # baremetal cmsis_rtx win32

# Controller
if(INIT_CENTRAL)
list(APPEND CFG_DEV "INIT_BROADCASTER" "INIT_PERIPHERAL" "INIT_OBSERVER INIT_CENTRAL")
elseif(INIT_PERIPHERAL)
list(APPEND CFG_DEV "INIT_BROADCASTER" "INIT_PERIPHERAL")
elseif(INIT_BROADCASTER)
list(APPEND CFG_DEV "INIT_BROADCASTER")
endif()
if(INIT_ENCRYPTED)
list(APPEND CFG_DEV "INIT_ENCRYPTED")
endif()

# RTOS
if(NOT DEBUG)
list(APPEND CFG_DEV "WSF_BUF_FREE_CHECK_ASSERT=0")
list(APPEND CFG_DEV "WSF_BUF_STATS=0")
list(APPEND CFG_DEV "WSF_CS_STATS=0")
list(APPEND CFG_DEV "WSF_ASSERT_ENABLED=0")
else()
list(APPEND CFG_DEV "WSF_BUF_FREE_CHECK_ASSERT=1")
list(APPEND CFG_DEV "WSF_BUF_STATS=1")
list(APPEND CFG_DEV "WSF_CS_STATS=1")
list(APPEND CFG_DEV "WSF_ASSERT_ENABLED=1")
endif()
if(TOKEN)
list(APPEND CFG_DEV "WSF_TOKEN_ENABLED=1")
endif()
if(TRACE)
list(APPEND CFG_DEV "WSF_TRACE_ENABLED=1")
endif()
