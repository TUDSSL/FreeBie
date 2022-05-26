/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Heart Rate profile sensor.
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
#ifndef CUS_API_H
#define CUS_API_H

#include <stdint.h>
#include <stdbool.h>
#include "wsf_timer.h"
#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup CUSTOM_PROFILE
 *  \{ */

/**************************************************************************************************
  Data Types
**************************************************************************************************/
// /*! WSF message event starting value */
#define CUS_MSG_START               0xC0

// /*! WSF message event enumeration */
enum
{
  CUS_FW_LEN_RECEIVED = CUS_MSG_START,
  CUS_FW_CP_UPDATED,
  CUS_FW_SECTION_RECEIVED
};

extern uint8_t custom_control_point;
extern uint8_t custom_firmware_len;
extern uint8_t custom_request_index;
extern uint8_t custom_firmware_section_buffer[8];

void customRegisterCpCccIdx(uint8_t cccIdx);
void customRegisterRiCccIdx(uint8_t cccIdx);
void customSetupToSendCp();
void customSetupToSendRi();
void customSendCpNtf();
void customSendRiNtf();

/*************************************************************************************************/
/*!
 *  \brief  Initialize the profile.
 *
 *  \param  handlerId    WSF handler ID of the application using this service.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomInit(wsfHandlerId_t handlerId);

/*************************************************************************************************/
/*!
 *  \brief  Process received WSF message.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomProcMsg(wsfMsgHdr_t *pMsg);

/*! \brief Attribute group read callback
 *
 * This is the attribute server read callback.  It is executed on an attribute
 * read operation if bitmask ATTS_SET_READ_CBACK is set in the settings field
 * of the attribute structure.
 * For a read operation, if the operation is successful the function must set
 * pAttr->pValue to the data to be read.  In addition, if the attribute is
 * variable length then pAttr->pLen must be set as well.
 *
 *  \param connId    DM connection ID.
 *  \param handle    Attribute handle.
 *  \param operation Operation type.
 *  \param offset    Read data offset.
 *  \param pAttr     Pointer to attribute structure.
 *
 *  \return status of the operation. \ref ATT_SUCCESS if successful.
 */
uint8_t customReadCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                                    uint16_t offset, attsAttr_t *pAttr);

/*! \brief Attribute group write callback
 *
 * This is the attribute server write callback.  It is executed on an
 * attribute write operation if bitmask ATTS_SET_WRITE_CBACK is set
 * in the settings field of the attribute structure.
 *
 *  \param connId    DM connection ID.
 *  \param handle    Attribute handle.
 *  \param operation Operation type.
 *  \param offset    Write data offset.
 *  \param len       Length of data to write.
 *  \param pValue    Pointer to data to write.
 *  \param pAttr     Pointer to attribute structure.
 *
 *  \return status of operation.  \ref ATT_SUCCESS if successful.
 */
uint8_t customWriteCback(dmConnId_t connId, uint16_t handle, uint8_t operation,
                                    uint16_t offset, uint16_t len, uint8_t *pValue,
                                    attsAttr_t *pAttr);

#ifdef __cplusplus
};
#endif

#endif /* CUS_API_H */
