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

# Application
if (WDXS_INCLUDED)
list(APPEND CFG_DEV "WDXS_INCLUDED=1")
endif()

# Host
list(APPEND CFG_DEV "SEC_CMAC_CFG=1")
list(APPEND CFG_DEV "SEC_ECC_CFG=2")
list(APPEND CFG_DEV "SEC_CCM_CFG=1")
if (USE_EXACTLE EQUAL 1)
list(APPEND CFG_DEV "HCI_TR_EXACTLE=1")
else()
list(APPEND CFG_DEV "HCI_TR_UART=1")
endif()

# RTOS
if(DEBUG)
list(APPEND CFG_DEV "WSF_BUF_STATS=1")
list(APPEND CFG_DEV "WSF_ASSERT_ENABLED=1")
endif()
if (TRACE EQUAL 1)
list(APPEND CFG_DEV "WSF_TRACE_ENABLED=1")
endif()
