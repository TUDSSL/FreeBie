/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Custom.
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
#include "util/bstream.h"
#include "att_api.h"
#include "app_api.h"
#include "app_hw.h"
#include "cus_api.h" 
#include "svc_custom.h"

/**************************************************************************************************
  Local Variables
**************************************************************************************************/
#define FIT_MSG_START               0xA0

// Same as in fit_main.c, but it wont compile if I include fit_api.h.
enum
{
  FIT_CUSTOM_ACCEL_TIMER_IND = FIT_MSG_START,
  FIT_CUSTOM_LUX_TIMER_IND
  // FIT_HR_TIMER_IND = FIT_MSG_START       /*! Heart rate measurement timer expired */
  // FIT_BATT_TIMER_IND,                     /*! Battery measurement timer expired */
  // FIT_RUNNING_TIMER_IND,                   /*! Running speed and cadence measurement timer expired */
  
};

/*! \brief Connection control block */
typedef struct
{
  dmConnId_t    connId;               /*! \brief Connection ID */
  bool_t        hrmToSend;            /*! \brief heart rate measurement ready to be sent on this channel */
} customConn_t;

static struct 
{
  customConn_t    conn[DM_CONN_MAX];    /* \brief connection control block */
  wsfTimer_t      measTimer;            /* \brief periodic measurement timer */
  appCustom_t     data;            /* \brief heart rate measurement */
  customCfg_t     cfg;         /* \brief configurable parameters */
  bool_t          txReady;              /* \brief TRUE if ready to send notifications */
  uint8_t         flags;                /* \brief heart rate measurement flags */
} customCb;

/*************************************************************************************************/
/*!
 *  \brief  Return TRUE if no connections with active measurements.
 *
 *  \return TRUE if no connections active.
 */
/*************************************************************************************************/
static bool_t customNoConnActive(void)
{
  customConn_t    *pConn = customCb.conn;
  uint8_t       i;

  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE)
    {
      return FALSE;
    }
  }
  return TRUE;
}
/*************************************************************************************************/
/*!
 *  \brief  Setup to send measurements on active connections.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void customSetupToSend(void)
{
  customConn_t    *pConn = customCb.conn;
  uint8_t       i;

  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE)
    {
      pConn->hrmToSend = TRUE;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Find next connection with measurement to send.
 *
 *  \param  cccIdx  Heart rate measurement CCC descriptor index.
 *
 *  \return Connection control block.
 */
/*************************************************************************************************/
static customConn_t *customFindNextToSend(uint8_t cccIdx)
{
  customConn_t    *pConn = customCb.conn;
  uint8_t       i;

  for (i = 0; i < DM_CONN_MAX; i++, pConn++)
  {
    if (pConn->connId != DM_CONN_ID_NONE && pConn->hrmToSend)
    {
      if (AttsCccEnabled(pConn->connId, cccIdx))
      {
        return pConn;
      }
    }
  }
  return NULL;
}
/*************************************************************************************************/
/*!
 *  \brief  Build a heart rate measurement characteristic.
 *
 *  \param  connId   DM connection identifier.
 *  \param  pBuf     Pointer to buffer to hold the built heart rate measurement characteristic.
 *  \param  pHrm     Heart rate measurement values.
 *
 *  \return Length of pBuf in bytes.
 */
/*************************************************************************************************/
static uint8_t customBuildHrm(dmConnId_t connId, uint8_t **pBuf, appCustom_t *pCustom)
{
  uint8_t   *pCustomData;
  uint8_t   flags = pCustom->flags;
  uint8_t   i;
  uint16_t  *pInterval;
  uint8_t   len = 1; /* Start with 2 for flags and 1 Byte accel value */
  uint8_t   maxLen = AttGetMtu(connId) - ATT_VALUE_NTF_LEN;

  /* Adjust length if necessary */
  if (len > maxLen)
  {
    len = maxLen;
  }

  /* Allocate buffer */
  if ((*pBuf = (uint8_t *)WsfBufAlloc(len)) != NULL)
  {
    /* Add data to buffer */
    pCustomData = *pBuf;

    /* flags */
    UINT8_TO_BSTREAM(pCustomData, pCustom->accel);

    /* Subtract 2 for flags and 1 Byte Heart Rate measurement */
    len -= 1;

    /* return length */
    return (uint8_t)(pCustomData - *pBuf);
  }

  return 0;
}
/*************************************************************************************************/
/*!
 *  \brief  Send heart rate measurement notification
 *
 *  \return None.
 */
/*************************************************************************************************/
static void customSendHrmNtf(dmConnId_t connId, uint16_t flag)
{
  if (flag == CUSTOM_DAT_ACCEL_HDL)
  {
    AttsHandleValueNtf(connId, CUSTOM_DAT_ACCEL_HDL, 1, &customCb.data.accel);
  }
  else if (flag == CUSTOM_DAT_LUX_HDL)
  {
    AttsHandleValueNtf(connId, CUSTOM_DAT_LUX_HDL, 1, &customCb.data.lux);
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
  customConn_t  *pConn;

  if (pMsg->hdr.status == ATT_SUCCESS && pMsg->handle == CUSTOM_DAT_ACCEL_HDL) 
  {
    customCb.txReady = TRUE;

    /* find next connection to send (note ccc idx is stored in timer status) */
    if ((pConn = customFindNextToSend(customCb.measTimer.msg.status)) != NULL)
    {
      customSendHrmNtf(pConn->connId, CUSTOM_DAT_ACCEL_HDL);
      customCb.txReady = FALSE;
      pConn->hrmToSend = FALSE;
    }
  }

  else if (pMsg->hdr.status == ATT_SUCCESS && pMsg->handle == CUSTOM_DAT_LUX_HDL)
  {
    customCb.txReady = TRUE;

    /* find next connection to send (note ccc idx is stored in timer status) */
    if ((pConn = customFindNextToSend(customCb.measTimer.msg.status)) != NULL)
    {
      customSendHrmNtf(pConn->connId, CUSTOM_DAT_LUX_HDL);
      customCb.txReady = FALSE;
      pConn->hrmToSend = FALSE;
    }
  }
}
/*************************************************************************************************/
/*!
 *  \brief  This function is called by the application when the periodic measurement
 *          timer expires.
 *
 *  \param  pMsg     Event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void customMeasTimerExp(wsfMsgHdr_t *pMsg)
{
  customConn_t  *pConn;

  /* if there are active connections */
  if (pMsg->event == FIT_CUSTOM_ACCEL_TIMER_IND && customNoConnActive() == FALSE)
  //if (customNoConnActive() == FALSE)
  {

    /* set up heart rate measurement to be sent on all connections */
    customSetupToSend();

    /* if ready to send measurements */
    if (customCb.txReady)
    {
      /* find next connection to send (note ccc idx is stored in timer status) */
      if ((pConn = customFindNextToSend(pMsg->status)) != NULL)
      {
        customSendHrmNtf(pConn->connId, CUSTOM_DAT_ACCEL_HDL);
        customCb.txReady = FALSE;
        pConn->hrmToSend = FALSE;
      }
    }

    /* restart timer */
    WsfTimerStartMs(&customCb.measTimer, customCb.cfg.period);

    /* increment energy expended for test/demonstration purposes */
    customCb.data.accel++;
  }

  if (pMsg->event == FIT_CUSTOM_LUX_TIMER_IND && customNoConnActive() == FALSE)
  {
    /* set up heart rate measurement to be sent on all connections */
    customSetupToSend();

    /* if ready to send measurements */
    if (customCb.txReady)
    {
      /* find next connection to send (note ccc idx is stored in timer status) */
      if ((pConn = customFindNextToSend(pMsg->status)) != NULL)
      {
        customSendHrmNtf(pConn->connId, CUSTOM_DAT_LUX_HDL);
        customCb.txReady = FALSE;
        pConn->hrmToSend = FALSE;
      }
    }

    /* restart timer */
    WsfTimerStartMs(&customCb.measTimer, customCb.cfg.period);

    /* increment energy expended for test/demonstration purposes */
    customCb.data.lux++;
  }
}
/*************************************************************************************************/
/*!
 *  \brief  Initialize the Heart Rate profile sensor.
 *
 *  \param  handerId    WSF handler ID of the application using this service.
 *  \param  pCfg        Configurable parameters.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomInit(wsfHandlerId_t handlerId, customCfg_t *pCfg)
{
  customCb.measTimer.handlerId = handlerId;
  customCb.cfg = *pCfg;
}


/*************************************************************************************************/
/*!
 *  \brief  Start periodic heart rate measurement.  This function starts a timer to perform
 *          periodic measurements.
 *
 *  \param  connId      DM connection identifier.
 *  \param  timerEvt    WSF event designated by the application for the timer.
 *  \param  hrmCccIdx   Index of heart rate CCC descriptor in CCC descriptor handle table.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomMeasStart(dmConnId_t connId, uint8_t timerEvt, uint8_t customCccIdx)
{
  /* if this is first connection */
  if (customNoConnActive())
  {
    /* initialize control block */
    customCb.measTimer.msg.event = timerEvt;
    customCb.measTimer.msg.status = customCccIdx;

    /* start timer */
    WsfTimerStartMs(&customCb.measTimer, customCb.cfg.period);
  }

  /* set conn id */
  customCb.conn[connId - 1].connId = connId;
}

/*************************************************************************************************/
/*!
 *  \brief  Stop periodic heart rate measurement.
 *
 *  \param  connId      DM connection identifier.
 *
 *  \return None.
 */
/*************************************************************************************************/
void CustomMeasStop(dmConnId_t connId)
{
  /* clear connection */
  customCb.conn[connId - 1].connId = DM_CONN_ID_NONE;
  customCb.conn[connId - 1].hrmToSend = FALSE;

  /* if no remaining connections */
  if (customNoConnActive())
  {
    /* stop timer */
    WsfTimerStop(&customCb.measTimer);
  }
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
    customCb.txReady = TRUE;
  }
  else if (pMsg->event == ATTS_HANDLE_VALUE_CNF)
  {
    customHandleValueCnf((attEvt_t *) pMsg);
  }
  else if (pMsg->event == customCb.measTimer.msg.event)
  {
    customMeasTimerExp(pMsg);
  }
}
