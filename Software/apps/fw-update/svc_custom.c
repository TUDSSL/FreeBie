/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Custom service implementation.
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
 Service variables
**************************************************************************************************/
const uint8_t customCpUuid[ATT_128_UUID_LEN] = {CUSTOM_UUID_CP};  /* Control Point Characteristic UUID */
const uint8_t customFlUuid[ATT_128_UUID_LEN] = {CUSTOM_UUID_FL};  /* Firmware Length Characteristic UUID */
const uint8_t customRiUuid[ATT_128_UUID_LEN] = {CUSTOM_UUID_RI};  /* Request Index Characteristic UUID */
const uint8_t customFsUuid[ATT_128_UUID_LEN] = {CUSTOM_UUID_FS};  /* Firmware Section Characteristic UUID */

/* Custom service declaration */
static const uint8_t custom_service[] = {CUSTOM_UUID_SERVICE};
static const uint16_t custom_service_length = sizeof(custom_service); 

/* Control Point characteristic */
static const uint8_t custom_char_cp[] = {ATT_PROP_READ | ATT_PROP_WRITE | ATT_PROP_NOTIFY, UINT16_TO_BYTES(CUSTOM_CP_HDL), CUSTOM_UUID_CP};
static const uint16_t custom_char_cp_length = sizeof(custom_char_cp);

/* Control Point */
static const uint8_t custom_val_cp[] = {0};
static const uint16_t custom_val_cp_length = sizeof(custom_val_cp);

/* Control Point client characteristic configuration */
static uint8_t custom_val_cp_ChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t custom_val_cp_ChCcc_length = sizeof(custom_val_cp_ChCcc);

/* Firmware Length characteristic */
static const uint8_t custom_char_fl[] = {ATT_PROP_WRITE, UINT16_TO_BYTES(CUSTOM_FL_HDL), CUSTOM_UUID_FL};
static const uint16_t custom_char_fl_length = sizeof(custom_char_fl);

/* Firmware Length */
static const uint8_t custom_val_fl[] = {0};
static const uint16_t custom_val_fl_length = sizeof(custom_val_fl);

/* Request Index characteristic */
static const uint8_t custom_char_ri[] = {ATT_PROP_READ | ATT_PROP_NOTIFY, UINT16_TO_BYTES(CUSTOM_RI_HDL), CUSTOM_UUID_RI};
static const uint16_t custom_char_ri_length = sizeof(custom_char_ri);

/* Request Index */
static const uint8_t custom_val_ri[] = {0};
static const uint16_t custom_val_ri_length = sizeof(custom_val_ri);

/* Request Index client characteristic configuration */
static uint8_t custom_val_ri_ChCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t custom_val_ri_ChCcc_length = sizeof(custom_val_ri_ChCcc);

/* Firmware Section characteristic */
static const uint8_t custom_char_fs[] = {ATT_PROP_WRITE, UINT16_TO_BYTES(CUSTOM_FS_HDL), CUSTOM_UUID_FS};
static const uint16_t custom_char_fs_length = sizeof(custom_char_fs);

/* Firmware Section */
static const uint8_t custom_val_fs[] = {0, 0, 0, 0, 0, 0, 0, 0};
static const uint16_t custom_val_fs_length = sizeof(custom_val_fs);

static const attsAttr_t CustomList[] =
{
  /* Custom service declaration */
  {
    attPrimSvcUuid,
    (uint8_t *) custom_service,
    (uint16_t *) &custom_service_length,
    sizeof(custom_service),
    0,
    ATTS_PERMIT_READ
  },
  /* Control Point characteristic */
  {
    attChUuid,
    (uint8_t *) custom_char_cp,
    (uint16_t *) &custom_char_cp_length,
    sizeof(custom_char_cp),
    0,
    ATTS_PERMIT_READ
  },
  /* Control Point */
  {
    customCpUuid,
    (uint8_t *) custom_val_cp,
    (uint16_t *) &custom_val_cp_length,
    sizeof(custom_val_cp),
    (ATTS_SET_UUID_128 | ATTS_SET_WRITE_CBACK | ATTS_SET_READ_CBACK),
    (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
  },
  /* Control Point client characteristic configuration */
  {
    attCliChCfgUuid,
    (uint8_t *) custom_val_cp_ChCcc,
    (uint16_t *) &custom_val_cp_ChCcc_length,
    sizeof(custom_val_cp_ChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
  },
  /* Firmware Length characteristic */
  { 
    attChUuid,
    (uint8_t *) custom_char_fl,
    (uint16_t *) &custom_char_fl_length,
    sizeof(custom_char_fl),
    0,
    ATTS_PERMIT_READ
  },
  /* Firmware Length */
  { 
    customFlUuid,
    (uint8_t *) custom_val_fl,
    (uint16_t *) &custom_val_fl_length,
    sizeof(custom_val_fl),
    (ATTS_SET_UUID_128 | ATTS_SET_WRITE_CBACK),
    ATTS_PERMIT_WRITE
  },
  /* Request Index characteristic */
  {
    attChUuid,
    (uint8_t *) custom_char_ri,
    (uint16_t *) &custom_char_ri_length,
    sizeof(custom_char_ri),
    0,
    ATTS_PERMIT_READ
  },
  /* Request Index */
  {
    customRiUuid,
    (uint8_t *) custom_val_ri,
    (uint16_t *) &custom_val_ri_length,
    sizeof(custom_val_ri),
    (ATTS_SET_UUID_128 | ATTS_SET_READ_CBACK),
    ATTS_PERMIT_READ
  },
  /* Request Index client characteristic configuration */
  {
    attCliChCfgUuid,
    (uint8_t *) custom_val_ri_ChCcc,
    (uint16_t *) &custom_val_ri_ChCcc_length,
    sizeof(custom_val_ri_ChCcc),
    ATTS_SET_CCC,
    (ATTS_PERMIT_READ | ATTS_PERMIT_WRITE)
  },
  /* Firmware Section characteristic */
  { 
    attChUuid,
    (uint8_t *) custom_char_fs,
    (uint16_t *) &custom_char_fs_length,
    sizeof(custom_char_fs),
    0,
    ATTS_PERMIT_READ
  },
  /* Firmware Section */
  { 
    customFsUuid,
    (uint8_t *) custom_val_fs,
    (uint16_t *) &custom_val_fs_length,
    sizeof(custom_val_fs),
    (ATTS_SET_UUID_128 | ATTS_SET_WRITE_CBACK),
    ATTS_PERMIT_WRITE
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
