/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Example Heart Rate service implementation.
 *
 *  Copyright (c) 2011-2018 Arm Ltd.
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
#include "att_api.h"
#include "wsf_trace.h"
#include "util/bstream.h"
#include "svc_ch.h"
#include "svc_custom.h"
#include "svc_cfg.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/
#define CUSTOM_BASE_UUID        0x13, 0x30, 0xa0, 0x3d, 0x85, 0xa0, 0xf4, 0x4a, 0xd4, 0xc1, 0x44, 0x39, 0x6d, 0x94

#define CUSTOM_UUID_DATA_ACCEL  UINT16_TO_BYTES(0x0001),CUSTOM_BASE_UUID
#define CUSTOM_UUID_DATA_LUX    UINT16_TO_BYTES(0x0002),CUSTOM_BASE_UUID

#define CUSTOM_UUID_SERVICE     UINT16_TO_BYTES(0x1001),CUSTOM_BASE_UUID

/**************************************************************************************************
 Service variables
**************************************************************************************************/

/* Custom service declaration */
static const uint8_t custom_service[] = {CUSTOM_UUID_SERVICE};
static const uint16_t custom_service_length = sizeof(custom_service); 

/* Custom measurement characteristic */
static const uint8_t custom_char_x[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(CUSTOM_DAT_ACCEL_HDL), CUSTOM_UUID_DATA_ACCEL};
static const uint16_t custom_char_x_length = sizeof(custom_char_x);
static const uint8_t custom_Uuid_x[] = {CUSTOM_UUID_DATA_ACCEL};

/* Custom measurement */
static const uint8_t custom_val_x[] = {0};
static const uint16_t custom_val_x_length = sizeof(custom_val_x);

/* Custom measurement characteristic */
static const uint8_t custom_char_y[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(CUSTOM_DAT_LUX_HDL), CUSTOM_UUID_DATA_LUX};
static const uint16_t custom_char_y_length = sizeof(custom_char_y);
static const uint8_t custom_Uuid_y[] = {CUSTOM_UUID_DATA_LUX};

/* Custom measurement */
static const uint8_t custom_val_y[] = {0};
static const uint16_t custom_val_y_length = sizeof(custom_val_y);

/* Custom measurement characteristic */
// static const uint8_t custom_char_z[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(CUSTOM_DAT_Z_HDL), CUSTOM_UUID_DATA};
// static const uint16_t custom_char_z_length = sizeof(custom_char_z);



/* Custom measurement */
// static const uint8_t custom_val_z[] = {0};
// static const uint16_t custom_val_z_length = sizeof(custom_val_z);

/* Custom client characteristic configuration */
static uint8_t custom_ChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t custom_ChCcc_length = sizeof(custom_ChCcc);


static const attsAttr_t CustomList[] =
{
  {
    attPrimSvcUuid,
    (uint8_t *) custom_service,
    (uint16_t *) &custom_service_length,
    sizeof(custom_service),
    0,
    ATTS_PERMIT_READ
  },
  {
    attChUuid,
    (uint8_t *) custom_char_x,
    (uint16_t *) &custom_char_x_length,
    sizeof(custom_char_x),
    0,
    ATTS_PERMIT_READ
  },
  {
    custom_Uuid_x,
    (uint8_t *) custom_val_x,
    (uint16_t *) &custom_val_x_length,
    sizeof(custom_val_x),
    ATTS_SET_UUID_128,
    0
  },
  {
    attCliChCfgUuid,
    (uint8_t *) custom_ChCcc,
    (uint16_t *) &custom_ChCcc_length,
    sizeof(custom_ChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
  },
  { 
    attChUuid,
    (uint8_t *) custom_char_y,
    (uint16_t *) &custom_char_y_length,
    sizeof(custom_char_y),
    0,
    ATTS_PERMIT_READ
  },
  { 
    custom_Uuid_y,
    (uint8_t *) custom_val_y,
    (uint16_t *) &custom_val_y_length,
    sizeof(custom_val_y),
    ATTS_SET_UUID_128,
    0
  },
  { 
    attCliChCfgUuid,
    (uint8_t *) custom_ChCcc,
    (uint16_t *) &custom_ChCcc_length,
    sizeof(custom_ChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
  }
};

static attsGroup_t custom_group =
{
  NULL,
  (attsAttr_t *) CustomList,
  NULL,
  NULL,
  CUSTOM_START_HDL,
  CUSTOM_END_HDL
};

/*************************************************************************************************/
/*!
 *  \brief  Add the services to the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCustomAddGroup(void)
{
  AttsAddGroup(&custom_group);
}

/*************************************************************************************************/
/*!
 *  \brief  Remove the services from the attribute server.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCustomRemoveGroup(void)
{
  AttsRemoveGroup(CUSTOM_START_HDL);
}

/*************************************************************************************************/
/*!
 *  \brief  Register callbacks for the service.
 *
 *  \param  readCback   Read callback function.
 *  \param  writeCback  Write callback function.
 *
 *  \return None.
 */
/*************************************************************************************************/
void SvcCustomCbackRegister(attsReadCback_t readCback, attsWriteCback_t writeCback)
{
  custom_group.readCback = readCback;
  custom_group.writeCback = writeCback;
}
