/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Custom service server.
 * 
 *  Copyright (c) 2012-2018 Arm Ltd.
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

#include <string.h>
#include "wsf_types.h"
#include "wsf_assert.h"
#include "wsf_buf.h"
#include "wsf_trace.h"
#include "wsf_msg.h"
#include "wsf_os.h"
#include "util/bstream.h"
#include "att_api.h"
#include "app_api.h"
#include "app_hw.h"
#include "cus_api.h" 
#include "svc_custom.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/
/*! \brief Connection control block */
static struct 
{
  dmConnId_t connId;
  wsfHandlerId_t handlerId;
  uint8_t cpCccIdx;
  uint8_t riCccIdx;
  bool cpToSend;
  bool cpTxReady;
  uint8_t sentCp;
  bool riToSend;
  bool riTxReady;
  uint8_t sentRi;
} customCb;

uint8_t custom_control_point;
uint8_t custom_firmware_len;
uint8_t custom_request_index;
uint8_t custom_firmware_section_buffer[8];

void customSetupToSendCp() {
  customCb.cpToSend = true;
}

void customSetupToSendRi() {
  customCb.riToSend = true;
}

void customRegisterCpCccIdx(uint8_t cccIdx) {
  customCb.cpCccIdx = cccIdx;
}

void customRegisterRiCccIdx(uint8_t cccIdx) {
  customCb.riCccIdx = cccIdx;
}

/*************************************************************************************************/
/*!
 *  \brief  Send notification
 *
 *  \return None.
 */
/*************************************************************************************************/
void customSendCpNtf() {
  if (customCb.connId != DM_CONN_ID_NONE
      && customCb.cpToSend
      && customCb.cpTxReady
      && customCb.sentCp != custom_control_point
      && AttsCccEnabled(customCb.connId, customCb.cpCccIdx)) {
    AttsHandleValueNtf(customCb.connId, CUSTOM_CP_HDL, 1, &custom_control_point);
    customCb.sentCp = custom_control_point;
    customCb.cpTxReady = false;
    customCb.cpToSend = false;
  }
}

void customSendRiNtf() {
  if (customCb.connId != DM_CONN_ID_NONE
      && customCb.riToSend
      && customCb.riTxReady
      && customCb.sentRi != custom_request_index
      && AttsCccEnabled(customCb.connId, customCb.riCccIdx)) {
    AttsHandleValueNtf(customCb.connId, CUSTOM_RI_HDL, 1, &custom_request_index);
    customCb.sentRi = custom_request_index;
    customCb.riTxReady = false;
    customCb.riToSend = false;
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Handle a received ATT handle value confirm.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void customHandleValueCnf(attEvt_t *pMsg)
{
  if (pMsg->hdr.status == ATT_SUCCESS && pMsg->handle == CUSTOM_CP_HDL) {
    customCb.cpTxReady = true;
    customSendCpNtf();
  }

  else if (pMsg->hdr.status == ATT_SUCCESS && pMsg->handle == CUSTOM_RI_HDL) {
    customCb.riTxReady = true;
    customSendRiNtf();
  }
}

void CustomInit(wsfHandlerId_t handlerId)
{
  customCb.handlerId = handlerId;
  customCb.connId = DM_CONN_ID_NONE;
  customCb.cpCccIdx = 0;
  customCb.riCccIdx = 0;
  customCb.cpToSend = false;
  customCb.cpTxReady = false;
  customCb.sentCp = 0xff;
  customCb.riToSend = false;
  customCb.riTxReady = false;
  customCb.sentRi = 0xff;

  custom_control_point = 0;
  custom_firmware_len = 0;
  custom_request_index = 0;
  memset(custom_firmware_section_buffer, 0xFF, sizeof(custom_firmware_section_buffer));
}

/*************************************************************************************************/
/*!
 *  \brief  Process received WSF message.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomProcMsg(wsfMsgHdr_t *pMsg)
{
  if (pMsg->event == DM_CONN_OPEN_IND)
  {
    dmConnId_t openConnId = AppConnIsOpen();
    if (openConnId != DM_CONN_ID_NONE) {
      customCb.connId = openConnId;
      customCb.cpTxReady = true;
      customCb.riTxReady = true;
    }
  }
  else if (pMsg->event == ATTS_HANDLE_VALUE_CNF)
  {
    customHandleValueCnf((attEvt_t *) pMsg);
  }
}

uint8_t customReadCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                                    uint16_t offset, attsAttr_t *pAttr)
{
  switch (handle) {
    case CUSTOM_CP_HDL:
      *(pAttr->pValue) = custom_control_point;
      break;

    case CUSTOM_RI_HDL:
      *(pAttr->pValue) = custom_request_index;
      break;

    default:
      break;
  }
  return ATT_SUCCESS;
}

uint8_t customWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                                    uint16_t offset, uint16_t len, uint8_t *pValue,
                                    attsAttr_t *pAttr)
{
  wsfMsgHdr_t *pMsg;

  switch (handle) {
    case CUSTOM_CP_HDL:
      custom_control_point = *pValue;
      customCb.sentCp = custom_control_point;  // a hacky bug fix
      if ((pMsg = WsfMsgAlloc(sizeof(wsfMsgHdr_t))) != NULL)
      {
        pMsg->event = CUS_FW_CP_UPDATED;
        WsfMsgSend(customCb.handlerId, pMsg);
      }
      break;
    
    case CUSTOM_FL_HDL:
      custom_firmware_len = *pValue;
      if ((pMsg = WsfMsgAlloc(sizeof(wsfMsgHdr_t))) != NULL)
      {
        pMsg->event = CUS_FW_LEN_RECEIVED;
        WsfMsgSend(customCb.handlerId, pMsg);
      }
      break;

    case CUSTOM_FS_HDL:
      if (len == 8) {
        memcpy(custom_firmware_section_buffer, pValue, len);
        if ((pMsg = WsfMsgAlloc(sizeof(wsfMsgHdr_t))) != NULL)
        {
          pMsg->event = CUS_FW_SECTION_RECEIVED;
          WsfMsgSend(customCb.handlerId, pMsg);
        }
      }
      break;

    default:
      break;
  }
  return ATT_SUCCESS;
}
