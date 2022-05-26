/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Stack initialization for medc.
 *
 *  Copyright (c) 2016-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc. 
 * 
 *  Copyright (c) 2022 Delft University of Technology.
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#include "wsf_types.h"
#include "wsf_os.h"
#include "util/bstream.h"

#include "medc/medc_api.h"

#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "svc_dis.h"
#include "svc_core.h"
#include "sec_api.h"
#include "hci_defs.h"

/*************************************************************************************************/
/*!
 *  \brief      Initialize stack.
 *
 *  \return     None.
 */
/*************************************************************************************************/
void StackInitMedc(void)
{
  wsfHandlerId_t handlerId;

  SecInit();
  SecAesInit();
  SecCmacInit();
  SecEccInit();

  handlerId = WsfOsSetNextHandler(HciHandler);
  HciHandlerInit(handlerId);

  handlerId = WsfOsSetNextHandler(DmHandler);
  DmDevVsInit(0);
  DmConnInit();
  DmScanInit();
  DmConnMasterInit();
  DmSecInit();
  DmSecLescInit();
  DmPrivInit();
  DmHandlerInit(handlerId);

  L2cInit();
  L2cMasterInit();

  handlerId = WsfOsSetNextHandler(AttHandler);
  AttHandlerInit(handlerId);
  AttsInit();
  AttsIndInit();
  AttcInit();

  handlerId = WsfOsSetNextHandler(SmpHandler);
  SmpHandlerInit(handlerId);
  SmpiInit();
  SmpiScInit();
  HciSetMaxRxAclLen(100);

  handlerId = WsfOsSetNextHandler(AppHandler);
  AppHandlerInit(handlerId);

  handlerId = WsfOsSetNextHandler(MedcHandler);
  MedcHandlerInit(handlerId);
}
