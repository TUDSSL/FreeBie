/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Template sample application for the following profiles:
 *            Core, Device Information
 *
 *  Copyright (c) 2011-2019 Arm Ltd. All Rights Reserved.
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
#include "util/bstream.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "hci_api.h"
#include "dm_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "app_db.h"
#include "app_ui.h"
#include "app_hw.h"
#include "app_main.h"
#include "svc_ch.h"
#include "svc_core.h"
#include "gatt/gatt_api.h"
#include "tipc/tipc_api.h"
#include "anpc/anpc_api.h"
#include "smartwatch_api.h"
#include "template_api.h"

#ifdef CHECKPOINT_TIMING
  #include "platform.h"
  #include "ext-rtc.h"
  #include "bf_sync.h"
#endif

#include "icons.h"

/**************************************************************************************************
  Macros
**************************************************************************************************/

// /*! WSF message event starting value */
#define TEMPLATE_MSG_START               0xA0

// /* Default Running Speed and Cadence Measurement period (seconds) */
// #define FIT_DEFAULT_RSCM_PERIOD        1

// /*! WSF message event enumeration */
enum
{
  TEMPLATE_SMARTWATCH_TIMER_IND = TEMPLATE_MSG_START
};

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! Application message type */
typedef union
{
  wsfMsgHdr_t     hdr;
  dmEvt_t         dm;
  attsCccEvt_t    ccc;
  attEvt_t        att;
} templateMsg_t;

/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! \brief application control block */
static struct
{
  // uint16_t          hdlMasterList[APP_DB_HDL_LIST_LEN];   /*! Cached handle list in master role */
  uint16_t          hdlSlaveList[APP_DB_HDL_LIST_LEN];    /*! Cached handle list in slave role */
  wsfHandlerId_t    handlerId;
  uint8_t           discState;
} watchCb;

/**************************************************************************************************
  Configurable Parameters
**************************************************************************************************/

/*! configurable parameters for advertising */
static const appAdvCfg_t templateAdvCfg =
{
  {    0,     0,     0},                  /*! Advertising durations in ms */
  { 3200,     0,     0}                   /*! Advertising intervals in 0.625 ms units */
};

/*! configurable parameters for slave */
static const appSlaveCfg_t templateSlaveCfg =
{
  TEMPLATE_CONN_MAX,                           /*! Maximum connections */
};

/*! configurable parameters for security */
// static const appSecCfg_t fitSecCfg =
// {
//   DM_AUTH_BOND_FLAG | DM_AUTH_SC_FLAG,    /*! Authentication and bonding flags */
//   0,                                      /*! Initiator key distribution flags */
//   DM_KEY_DIST_LTK,                        /*! Responder key distribution flags */
//   FALSE,                                  /*! TRUE if Out-of-band pairing data is present */
//   TRUE                                    /*! TRUE to initiate security upon connection */
// };
static const appSecCfg_t templateSecCfg =
{
  0,                                      /*! Authentication and bonding flags */
  0,                                      /*! Initiator key distribution flags */
  DM_KEY_DIST_LTK,                        /*! Responder key distribution flags */
  FALSE,                                  /*! TRUE if Out-of-band pairing data is present */
  FALSE                                   /*! TRUE to initiate security upon connection */
};

/*! configurable parameters for connection parameter update */
// static const appUpdateCfg_t fitUpdateCfg =
// {
//   6000,                                   /*! Connection idle period in ms before attempting
//                                               connection parameter update; set to zero to disable */
//   640,                                    /*! Minimum connection interval in 1.25ms units */
//   800,                                    /*! Maximum connection interval in 1.25ms units */
//   0,                                      /*! Connection latency */
//   900,                                    /*! Supervision timeout in 10ms units */
//   5                                       /*! Number of update attempts before giving up */
// };
// parameters tested with Android and nRF Connect
// this is the maximum that we can do - with Google Pixel 3a running Android 11
// static const appUpdateCfg_t templateUpdateCfgLow =
// {
//   1,                                      /*! Connection idle period in ms before attempting
//                                               connection parameter update; set to zero to disable */
//   3199,                                   /*! Minimum connection interval in 1.25ms units */
//   3199,                                   /*! Maximum connection interval in 1.25ms units */
//   3,                                      /*! Connection latency */
//   3200,                                   /*! Supervision timeout in 10ms units */
//   0x7F                                    /*! Number of update attempts before giving up */
// };
static const appUpdateCfg_t templateUpdateCfgLow =
{
  1,                                      /*! Connection idle period in ms before attempting
                                              connection parameter update; set to zero to disable */
  3199,                                   /*! Minimum connection interval in 1.25ms units */
  3199,                                   /*! Maximum connection interval in 1.25ms units */
  3,                                      /*! Connection latency */
  3200,                                   /*! Supervision timeout in 10ms units */
  0x7F                                    /*! Number of update attempts before giving up */
};

static const appUpdateCfg_t templateUpdateCfgNormal =
{
  1,                                      /*! Connection idle period in ms before attempting
                                              connection parameter update; set to zero to disable */
  800,                                    /*! Minimum connection interval in 1.25ms units */
  800,                                    /*! Maximum connection interval in 1.25ms units */
  0,                                      /*! Connection latency */
  2000,                                   /*! Supervision timeout in 10ms units */
  0x7F                                    /*! Number of update attempts before giving up */
};

/*! heart rate measurement configuration */
// static const hrpsCfg_t fitHrpsCfg =
// {
//   2000      /*! Measurement timer expiration period in ms */
// };

static const smartwatchCfg_t templateSmartwatchCfg =
{
  59999  /*! Smartwatch timer expiration period in ms */
};

/*! SMP security parameter configuration */
// static const smpCfg_t fitSmpCfg =
// {
//   500,                                    /*! 'Repeated attempts' timeout in msec */
//   SMP_IO_NO_IN_NO_OUT,                    /*! I/O Capability */
//   7,                                      /*! Minimum encryption key length */
//   16,                                     /*! Maximum encryption key length */
//   1,                                      /*! Attempts to trigger 'repeated attempts' timeout */
//   0,                                      /*! Device authentication requirements */
//   64000,                                  /*! Maximum repeated attempts timeout in msec */
//   64000,                                  /*! Time msec before attemptExp decreases */
//   2                                       /*! Repeated attempts multiplier exponent */
// };
static const smpCfg_t templateSmpCfg =
{
  500,                                    /*! 'Repeated attempts' timeout in msec */
  SMP_IO_NO_IN_NO_OUT,                    /*! I/O Capability */
  7,                                      /*! Minimum encryption key length */
  16,                                     /*! Maximum encryption key length */
  1,                                      /*! Attempts to trigger 'repeated attempts' timeout */
  0,                                      /*! Device authentication requirements */
  64000,                                  /*! Maximum repeated attempts timeout in msec */
  64000,                                  /*! Time msec before attemptExp decreases */
  2                                       /*! Repeated attempts multiplier exponent */
};

/*! Configurable parameters for service and characteristic discovery */
static const appDiscCfg_t watchDiscCfg =
{
  FALSE,                                  /*! TRUE to wait for a secure connection before initiating discovery */
  FALSE                                   /*! TRUE to fall back on database hash to verify handles when no bond exists. */
};

static const appCfg_t watchAppCfg =
{
  // TRUE,                                   /*! TRUE to abort service discovery if service not found */
  FALSE,                                   /*! TRUE to abort service discovery if service not found */
  // TRUE                                    /*! TRUE to disconnect if ATT transaction times out */
  FALSE                                   /*! TRUE to disconnect if ATT transaction times out */
};

/**************************************************************************************************
  Advertising Data
**************************************************************************************************/

/*! advertising data, discoverable mode */
static const uint8_t templateAdvDataDisc[] =
{
  /*! flags */
  2,                                      /*! length */
  DM_ADV_TYPE_FLAGS,                      /*! AD type */
  DM_FLAG_LE_GENERAL_DISC |               /*! flags */
  DM_FLAG_LE_BREDR_NOT_SUP,

  /*! service UUID list */
  5,                                      /*! length */
  DM_ADV_TYPE_16_SOLICIT,                 /*! AD type */
  UINT16_TO_BYTES(ATT_UUID_CURRENT_TIME_SERVICE),
  UINT16_TO_BYTES(ATT_UUID_ALERT_NOTIF_SERVICE),

  /*! device name */
  9,                                      /*! length */
  DM_ADV_TYPE_LOCAL_NAME,                 /*! AD type */
  'T',
  'e',
  'm',
  'p',
  'l',
  'a',
  't',
  'e'
};

/*! scan data, discoverable mode */
// static const uint8_t templateScanDataDisc[] =
// {
//   /*! tx power */
//   2,                                      /*! length */
//   DM_ADV_TYPE_TX_POWER,                   /*! AD type */
//   0                                       /*! tx power */
// };

/**************************************************************************************************
  ATT Client Discovery Data
**************************************************************************************************/

/*! Discovery states:  enumeration of services to be discovered */
enum
{
  WATCH_DISC_SLAVE_GATT_SVC,      /* GATT service */
  WATCH_DISC_SLAVE_CTS_SVC,       /* Current Time service */
  WATCH_DISC_SLAVE_ANS_SVC,       /* Alert Notification service */
  WATCH_DISC_SLAVE_SVC_MAX        /* Discovery complete */
};

/*! the Client handle list, watchCb.hdlList[], is set as follows:
 *
 *  ------------------------------- <- WATCH_DISC_GATT_START
 *  | GATT handles                |
 *  |...                          |
 *  ------------------------------- <- WATCH_DISC_CTS_START
 *  | CTS handles                 |
 *  | ...                         |
 *  ------------------------------- <- WATCH_DISC_ANS_START
 *  | ANS handles                 |
 *  | ...                         |
 *  -------------------------------
 */

/*! Start of each service's handles in the the handle list */
#define WATCH_DISC_GATT_START           0
#define WATCH_DISC_CTS_START            (WATCH_DISC_GATT_START + GATT_HDL_LIST_LEN)
#define WATCH_DISC_ANS_START            (WATCH_DISC_CTS_START + TIPC_CTS_HDL_LIST_LEN)
#define WATCH_DISC_SLAVE_HDL_LIST_LEN   (WATCH_DISC_ANS_START + ANPC_ANS_HDL_LIST_LEN)

/*! Pointers into handle list for each service's handles */
static uint16_t *pWatchSlvGattHdlList = &watchCb.hdlSlaveList[WATCH_DISC_GATT_START];
uint16_t *pWatchCtsHdlList =     &watchCb.hdlSlaveList[WATCH_DISC_CTS_START];
uint16_t *pWatchAnsHdlList =     &watchCb.hdlSlaveList[WATCH_DISC_ANS_START];

/* sanity check:  make sure handle list length is <= app db handle list length */
// WSF_CT_ASSERT(WATCH_DISC_SLAVE_HDL_LIST_LEN <= APP_DB_HDL_LIST_LEN);

/**************************************************************************************************
  ATT Client Configuration Data
**************************************************************************************************/

/*
 * Data for configuration after service discovery
 */

/* Default value for CCC indications */
static const uint8_t watchCccIndVal[] = {UINT16_TO_BYTES(ATT_CLIENT_CFG_INDICATE)};

/* Default value for CCC notifications */
static const uint8_t watchCccNtfVal[] = {UINT16_TO_BYTES(ATT_CLIENT_CFG_NOTIFY)};

/* Default value for Client Supported Features (enable Robust Caching) */
static const uint8_t watchCsfVal[1] = { ATTS_CSF_ROBUST_CACHING };

/* ANS Control point value for "Enable New Alert Notification" */
static const uint8_t watchAncpEnNewVal[] = {CH_ANCP_ENABLE_NEW, CH_ALERT_CAT_ID_ALL};

/* ANS Control point value for "Notify New Alert Immediately" */
static const uint8_t watchAncpNotNewVal[] = {CH_ANCP_NOTIFY_NEW, CH_ALERT_CAT_ID_ALL};

/* ANS Control point value for "Enable Unread Alert Status Notification" */
static const uint8_t watchAncpEnUnrVal[] = {CH_ANCP_ENABLE_UNREAD, CH_ALERT_CAT_ID_ALL};

/* ANS Control point value for "Notify Unread Alert Status Immediately" */
static const uint8_t watchAncpNotUnrVal[] = {CH_ANCP_NOTIFY_UNREAD, CH_ALERT_CAT_ID_ALL};

/* List of characteristics to configure after service discovery */
static const attcDiscCfg_t watchDiscSlaveCfgList[] =
{
  /* Read:  CTS Current time */
  {NULL, 0, (TIPC_CTS_CT_HDL_IDX + WATCH_DISC_CTS_START)},

  /* Read:  CTS Local time information */
  {NULL, 0, (TIPC_CTS_LTI_HDL_IDX + WATCH_DISC_CTS_START)},

  /* Read:  CTS Reference time information */
  {NULL, 0, (TIPC_CTS_RTI_HDL_IDX + WATCH_DISC_CTS_START)},

  /* Read:  ANS Supported new alert category */
  {NULL, 0, (ANPC_ANS_SNAC_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Read:  ANS Supported unread alert category */
  {NULL, 0, (ANPC_ANS_SUAC_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  GATT service changed ccc descriptor */
  {watchCccIndVal, sizeof(watchCccIndVal), (GATT_SC_CCC_HDL_IDX + WATCH_DISC_GATT_START)},

  /* Write:  GATT client supported features */
  {watchCsfVal, sizeof(watchCsfVal), (GATT_CSF_HDL_IDX + WATCH_DISC_GATT_START) },

  /* Write:  CTS Current time ccc descriptor */
  {watchCccNtfVal, sizeof(watchCccNtfVal), (TIPC_CTS_CT_CCC_HDL_IDX + WATCH_DISC_CTS_START)},

  /* Write:  ANS New alert ccc descriptor */
  {watchCccNtfVal, sizeof(watchCccNtfVal), (ANPC_ANS_NA_CCC_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Unread alert status ccc descriptor */
  {watchCccNtfVal, sizeof(watchCccNtfVal), (ANPC_ANS_UAS_CCC_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Enable New Alert Notification" */
  {watchAncpEnNewVal, sizeof(watchAncpEnNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Notify New Alert Immediately" */
  {watchAncpNotNewVal, sizeof(watchAncpNotNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Enable Unread Alert Status Notification" */
  {watchAncpEnUnrVal, sizeof(watchAncpEnUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Notify Unread Alert Status Immediately" */
  {watchAncpNotUnrVal, sizeof(watchAncpNotUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)}
};

/* Characteristic configuration list length */
#define WATCH_DISC_SLAVE_CFG_LIST_LEN   (sizeof(watchDiscSlaveCfgList) / sizeof(attcDiscCfg_t))

/* sanity check:  make sure configuration list length is <= handle list length */
// WSF_CT_ASSERT(WATCH_DISC_SLAVE_CFG_LIST_LEN <= WATCH_DISC_SLAVE_HDL_LIST_LEN);

/*
 * Data for configuration on connection setup
 */

/* List of characteristics to configure on connection setup */
static const attcDiscCfg_t watchDiscConnCfgList[] =
{
  /* Read:  ANS Supported new alert category */
  {NULL, 0, (ANPC_ANS_SNAC_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Read:  ANS Supported unread alert category */
  {NULL, 0, (ANPC_ANS_SUAC_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Enable New Alert Notification" */
  {watchAncpEnNewVal, sizeof(watchAncpEnNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Notify New Alert Immediately" */
  {watchAncpNotNewVal, sizeof(watchAncpNotNewVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Enable Unread Alert Status Notification" */
  {watchAncpEnUnrVal, sizeof(watchAncpEnUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)},

  /* Write:  ANS Control point "Notify Unread Alert Status Immediately" */
  {watchAncpNotUnrVal, sizeof(watchAncpNotUnrVal), (ANPC_ANS_ANCP_HDL_IDX + WATCH_DISC_ANS_START)}
};

/* Characteristic configuration list length */
#define WATCH_DISC_CONN_CFG_LIST_LEN   (sizeof(watchDiscConnCfgList) / sizeof(attcDiscCfg_t))

/* sanity check:  make sure configuration list length is <= handle list length */
// WSF_CT_ASSERT(WATCH_DISC_CONN_CFG_LIST_LEN <= WATCH_DISC_SLAVE_HDL_LIST_LEN);

/**************************************************************************************************
  ATT Server Data
  Client Characteristic Configuration Descriptors
**************************************************************************************************/

/*! enumeration of client characteristic configuration descriptors */
// enum
// {
//   FIT_GATT_SC_CCC_IDX,                    /*! GATT service, service changed characteristic */
//   FIT_HRS_HRM_CCC_IDX,                    /*! Heart rate service, heart rate monitor characteristic */
//   FIT_BATT_LVL_CCC_IDX,                   /*! Battery service, battery level characteristic */
//   FIT_RSCS_SM_CCC_IDX,                   /*! Runninc speed and cadence measurement characteristic */
//   FIT_NUM_CCC_IDX
// };
enum
{
  TEMPLATE_GATT_SC_CCC_IDX,                    /*! GATT service, service changed characteristic */
  TEMPLATE_NUM_CCC_IDX
};

/*! client characteristic configuration descriptors settings, indexed by above enumeration */
// static const attsCccSet_t fitCccSet[FIT_NUM_CCC_IDX] =
// {
//   /* cccd handle          value range               security level */
//   {GATT_SC_CH_CCC_HDL,    ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE},   /* FIT_GATT_SC_CCC_IDX */
//   {HRS_HRM_CH_CCC_HDL,    ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE},   /* FIT_HRS_HRM_CCC_IDX */
//   {BATT_LVL_CH_CCC_HDL,   ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE},   /* FIT_BATT_LVL_CCC_IDX */
//   {RSCS_RSM_CH_CCC_HDL,   ATT_CLIENT_CFG_NOTIFY,    DM_SEC_LEVEL_NONE}    /* FIT_RSCS_SM_CCC_IDX */
// };
static const attsCccSet_t templateCccSet[TEMPLATE_NUM_CCC_IDX] =
{
  /* cccd handle          value range               security level */
  {GATT_SC_CH_CCC_HDL,    ATT_CLIENT_CFG_INDICATE,  DM_SEC_LEVEL_NONE}    /* TEMPLATE_GATT_SC_CCC_IDX */
};

/**************************************************************************************************
  Global Variables
**************************************************************************************************/

/*! WSF handler ID */
wsfHandlerId_t templateHandlerId;

wsfTimer_t        smartwatchTimer;            /*! \brief periodic smartwatch timer */

/*************************************************************************************************/
/*!
 *  \brief  Application DM callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateDmCback(dmEvt_t *pDmEvt)
{
  dmEvt_t *pMsg;
  uint16_t len;

  len = DmSizeOfEvt(pDmEvt);

  if ((pMsg = WsfMsgAlloc(len)) != NULL)
  {
    memcpy(pMsg, pDmEvt, len);
    WsfMsgSend(templateHandlerId, pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Application ATT callback.
 *
 *  \param  pEvt    ATT callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateAttCback(attEvt_t *pEvt)
{
  attEvt_t *pMsg;

  if ((pMsg = WsfMsgAlloc(sizeof(attEvt_t) + pEvt->valueLen)) != NULL)
  {
    memcpy(pMsg, pEvt, sizeof(attEvt_t));
    pMsg->pValue = (uint8_t *) (pMsg + 1);
    memcpy(pMsg->pValue, pEvt->pValue, pEvt->valueLen);
    WsfMsgSend(templateHandlerId, pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Application ATTS client characteristic configuration callback.
 *
 *  \param  pDmEvt  DM callback event
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateCccCback(attsCccEvt_t *pEvt)
{
  attsCccEvt_t  *pMsg;
  appDbHdl_t    dbHdl;

  /* If CCC not set from initialization and there's a device record and currently bonded */
  if ((pEvt->handle != ATT_HANDLE_NONE) &&
      ((dbHdl = AppDbGetHdl((dmConnId_t) pEvt->hdr.param)) != APP_DB_HDL_NONE) &&
      AppCheckBonded((dmConnId_t)pEvt->hdr.param))
  {
    /* Store value in device database. */
    AppDbSetCccTblValue(dbHdl, pEvt->idx, pEvt->value);
  }

  if ((pMsg = WsfMsgAlloc(sizeof(attsCccEvt_t))) != NULL)
  {
    memcpy(pMsg, pEvt, sizeof(attsCccEvt_t));
    WsfMsgSend(templateHandlerId, pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Process CCC state change.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateProcCccState(templateMsg_t *pMsg)
{
  APP_TRACE_INFO3("ccc state ind value:%d handle:%d idx:%d", pMsg->ccc.value, pMsg->ccc.handle, pMsg->ccc.idx);
}

/*************************************************************************************************/
/*!
 *  \brief  Perform UI actions on connection close.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateClose(templateMsg_t *pMsg)
{
  // AppAdvStart(APP_MODE_AUTO_INIT);
}

/*************************************************************************************************/
/*!
 *  \brief  Set up advertising and other procedures that need to be performed after
 *          device reset.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateSetup(templateMsg_t *pMsg)
{
  /* set advertising and scan response data for discoverable mode */
  AppAdvSetData(APP_ADV_DATA_DISCOVERABLE, sizeof(templateAdvDataDisc), (uint8_t *) templateAdvDataDisc);
  // AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, sizeof(templateScanDataDisc), (uint8_t *) templateScanDataDisc);
  AppAdvSetData(APP_SCAN_DATA_DISCOVERABLE, 0, NULL);

  /* set advertising and scan response data for connectable mode */
  // AppAdvSetData(APP_ADV_DATA_CONNECTABLE, 0, NULL);
  AppAdvSetData(APP_ADV_DATA_CONNECTABLE, sizeof(templateAdvDataDisc), (uint8_t *) templateAdvDataDisc);
  AppAdvSetData(APP_SCAN_DATA_CONNECTABLE, 0, NULL);

#ifdef NO_SCAN_RSP
  /* blank whilte list to filter all scan requests */
  DmDevWhiteListClear();
  DmDevSetFilterPolicy(DM_FILT_POLICY_MODE_ADV, HCI_ADV_FILT_SCAN);
#endif

  /* start advertising; Discoverable mode */
  // AppSetAdvType(DM_ADV_NONCONN_UNDIRECT);
  AppAdvStart(APP_MODE_AUTO_INIT);
}

/*************************************************************************************************/
/*!
 *  \brief  Process a received ATT read response, notification, or indication when connected in
 *          the slave role.
 *
 *  \param  pMsg    Pointer to ATT callback event message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchSlaveValueUpdate(attEvt_t *pMsg)
{
  if (pMsg->hdr.status == ATT_SUCCESS)
  {
    /* determine which profile the handle belongs to; start with most likely */

    // /* alert notification */
    // if (AnpcAnsValueUpdate(pWatchAnsHdlList, pMsg) == ATT_SUCCESS)
    // {
    //   return;
    // }

    // /* current time */
    // if (TipcCtsValueUpdate(pWatchCtsHdlList, pMsg) == ATT_SUCCESS)
    // {
    //   return;
    // }

    /* GATT */
    if (GattValueUpdate(pWatchSlvGattHdlList, pMsg) == ATT_SUCCESS)
    {
      return;
    }
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Discovery callback.
 *
 *  \param  connId    Connection identifier.
 *  \param  status    Service or configuration status.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void watchDiscCback(dmConnId_t connId, uint8_t status)
{
  switch(status)
  {
    case APP_DISC_INIT:
      /* set handle list when initialization requested */
      AppDiscSetHdlList(connId, WATCH_DISC_SLAVE_HDL_LIST_LEN, watchCb.hdlSlaveList);
      break;

    case APP_DISC_READ_DATABASE_HASH:
      /* Read peer's database hash */
      AppDiscReadDatabaseHash(connId);
      break;

    case APP_DISC_SEC_REQUIRED:
      /* request security */
      AppSlaveSecurityReq(connId);
      break;

    case APP_DISC_START:
      /* initialize discovery state */
      /* discover GATT service - slave role */
      watchCb.discState = WATCH_DISC_SLAVE_GATT_SVC;
      GattDiscover(connId, pWatchSlvGattHdlList);

      break;

    case APP_DISC_FAILED:
    case APP_DISC_CMPL:
      /* next discovery state */
      watchCb.discState++;
      if (watchCb.discState == WATCH_DISC_SLAVE_CTS_SVC)
      {
        /* discover current time service */
        TipcCtsDiscover(connId, pWatchCtsHdlList);
      }
      else if (watchCb.discState == WATCH_DISC_SLAVE_ANS_SVC)
      {
        /* discover alert notification service */
        AnpcAnsDiscover(connId, pWatchAnsHdlList);
      }
      else
      {
        /* discovery complete */
        AppDiscComplete(connId, APP_DISC_CMPL);

        {
          hciConnSpec_t connSpec;
          connSpec.connIntervalMin = pAppUpdateCfg->connIntervalMin;
          connSpec.connIntervalMax = pAppUpdateCfg->connIntervalMax;
          connSpec.connLatency = pAppUpdateCfg->connLatency;
          connSpec.supTimeout = pAppUpdateCfg->supTimeout;
          connSpec.minCeLen = 0;
          connSpec.maxCeLen = 0xffff;
          DmConnUpdate(AppConnIsOpen(), &connSpec);
        }

        /* start configuration */
        AppDiscConfigure(connId, APP_DISC_CFG_START, WATCH_DISC_SLAVE_CFG_LIST_LEN,
                          (attcDiscCfg_t *) watchDiscSlaveCfgList,
                          WATCH_DISC_SLAVE_HDL_LIST_LEN, watchCb.hdlSlaveList);
      }
      break;

    case APP_DISC_CFG_START:
      /* start configuration */
      AppDiscConfigure(connId, APP_DISC_CFG_START, WATCH_DISC_SLAVE_CFG_LIST_LEN,
                        (attcDiscCfg_t *) watchDiscSlaveCfgList,
                        WATCH_DISC_SLAVE_HDL_LIST_LEN, watchCb.hdlSlaveList);
      break;

    case APP_DISC_CFG_CMPL:
      AppDiscComplete(connId, status);
      break;

    case APP_DISC_CFG_CONN_START:
      if (DmConnRole(connId) == DM_ROLE_SLAVE)
      {
        /* start connection setup configuration */
        AppDiscConfigure(connId, APP_DISC_CFG_CONN_START, WATCH_DISC_CONN_CFG_LIST_LEN,
                         (attcDiscCfg_t *) watchDiscConnCfgList,
                         WATCH_DISC_CONN_CFG_LIST_LEN, watchCb.hdlSlaveList);
      }
      break;

    default:
      break;
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Process messages from the event handler.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
static void templateProcMsg(templateMsg_t *pMsg)
{
  uint8_t uiEvent = APP_UI_NONE;

  switch(pMsg->hdr.event)
  {
    case TEMPLATE_SMARTWATCH_TIMER_IND:
      SmartwatchProcMsg(&pMsg->hdr);
      break;

    case ATTC_READ_RSP:
    case ATTC_HANDLE_VALUE_NTF:
    case ATTC_HANDLE_VALUE_IND:
      SmartwatchProcMsg((wsfMsgHdr_t *)pMsg);
      watchSlaveValueUpdate((attEvt_t *) pMsg);
      break;

    case ATTS_HANDLE_VALUE_CNF:
      break;

    case ATTS_CCC_STATE_IND:
      templateProcCccState(pMsg);
      break;

    case DM_RESET_CMPL_IND:
      AttsCalculateDbHash();
      DmSecGenerateEccKeyReq();
    #ifdef CHECKPOINT_TIMING
      extRtcSetup();
      // Sync external rtc clk to internal rtc clk
      bfSyncSetupSync();
    #endif
      templateSetup(pMsg);
      SmartwatchStart(TEMPLATE_SMARTWATCH_TIMER_IND);
      uiEvent = APP_UI_RESET_CMPL;
      break;

    case DM_ADV_SET_START_IND:
      uiEvent = APP_UI_ADV_SET_START_IND;
      break;

    case DM_ADV_SET_STOP_IND:
      uiEvent = APP_UI_ADV_SET_STOP_IND;
      break;

     case DM_ADV_START_IND:
      uiEvent = APP_UI_ADV_START;
      break;

    case DM_ADV_STOP_IND:
      uiEvent = APP_UI_ADV_STOP;
      break;

    case DM_CONN_OPEN_IND:
      SmartwatchProcMsg((wsfMsgHdr_t *)pMsg);
      uiEvent = APP_UI_CONN_OPEN;
      break;

    case DM_CONN_CLOSE_IND:
      SmartwatchProcMsg((wsfMsgHdr_t *)pMsg);
      templateClose(pMsg);
      uiEvent = APP_UI_CONN_CLOSE;
      break;

    case DM_SEC_PAIR_CMPL_IND:
      DmSecGenerateEccKeyReq();
      uiEvent = APP_UI_SEC_PAIR_CMPL;
      break;

    case DM_SEC_PAIR_FAIL_IND:
      DmSecGenerateEccKeyReq();
      uiEvent = APP_UI_SEC_PAIR_FAIL;
      break;

    case DM_SEC_ENCRYPT_IND:
      uiEvent = APP_UI_SEC_ENCRYPT;
      break;

    case DM_SEC_ENCRYPT_FAIL_IND:
      uiEvent = APP_UI_SEC_ENCRYPT_FAIL;
      break;

    case DM_SEC_AUTH_REQ_IND:
      AppHandlePasskey(&pMsg->dm.authReq);
      break;

    case DM_SEC_ECC_KEY_IND:
      DmSecSetEccKey(&pMsg->dm.eccMsg.data.key);
      break;

    case DM_SEC_COMPARE_IND:
      AppHandleNumericComparison(&pMsg->dm.cnfInd);
      break;

    case DM_PRIV_CLEAR_RES_LIST_IND:
      APP_TRACE_INFO1("Clear resolving list status 0x%02x", pMsg->hdr.status);
      break;

    case DM_HW_ERROR_IND:
      uiEvent = APP_UI_HW_ERROR;
      break;

    default:
      break;
  }

  if (uiEvent != APP_UI_NONE)
  {
    AppUiAction(uiEvent);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Application handler init function called during system initialization.
 *
 *  \param  handlerID  WSF handler ID.
 *
 *  \return None.
 */
/*************************************************************************************************/
void applyConnParamNormal() {
  pAppUpdateCfg = (appUpdateCfg_t *) &templateUpdateCfgNormal;
}

void applyConnParamLow() {
  pAppUpdateCfg = (appUpdateCfg_t *) &templateUpdateCfgLow;
}

void TemplateHandlerInit(wsfHandlerId_t handlerId)
{
  APP_TRACE_INFO0("TemplateHandlerInit");

  /* store handler ID */
  templateHandlerId = handlerId;
  watchCb.handlerId = handlerId;

  /* Set configuration pointers */
  pAppAdvCfg = (appAdvCfg_t *) &templateAdvCfg;
  pAppSlaveCfg = (appSlaveCfg_t *) &templateSlaveCfg;
  pAppSecCfg = (appSecCfg_t *) &templateSecCfg;
  applyConnParamLow();
  pAppDiscCfg = (appDiscCfg_t *) &watchDiscCfg;
  pAppCfg = (appCfg_t *) &watchAppCfg;

  /* Set stack configuration pointers */
  pSmpCfg = (smpCfg_t *) &templateSmpCfg;

  /* Initialize application framework */
  AppSlaveInit();
  AppServerInit();
  AppDiscInit();

  /* initialize heart rate profile sensor */
  // HrpsInit(handlerId, (hrpsCfg_t *) &fitHrpsCfg);
  // HrpsSetFlags(fitHrmFlags);

  SmartwatchInit(handlerId, (smartwatchCfg_t *) &templateSmartwatchCfg);
}

/*************************************************************************************************/
/*!
 *  \brief  WSF event handler for application.
 *
 *  \param  event   WSF event mask.
 *  \param  pMsg    WSF message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TemplateHandler(wsfEventMask_t event, wsfMsgHdr_t *pMsg)
{
  if (pMsg != NULL)
  {
    APP_TRACE_INFO1("Template got evt %d", pMsg->event);

    /* process ATT messages */
    if (pMsg->event >= ATT_CBACK_START && pMsg->event <= ATT_CBACK_END)
    {
      /* process discovery-related ATT messages */
      AppDiscProcAttMsg((attEvt_t *) pMsg);

      /* process server-related ATT messages */
      AppServerProcAttMsg(pMsg);
    }
    /* process DM messages */
    else if (pMsg->event >= DM_CBACK_START && pMsg->event <= DM_CBACK_END)
    {
      /* process advertising and connection-related messages */
      AppSlaveProcDmMsg((dmEvt_t *) pMsg);

      /* process security-related messages */
      AppSlaveSecProcDmMsg((dmEvt_t *) pMsg);

      /* process discovery-related messages */
      AppDiscProcDmMsg((dmEvt_t *) pMsg);
    }

    /* perform profile and user interface-related operations */
    templateProcMsg((templateMsg_t *) pMsg);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Start the application.
 *
 *  \return None.
 */
/*************************************************************************************************/
void TemplateStart(void)
{
  /* Register for stack callbacks */
  DmRegister(templateDmCback);
  DmConnRegister(DM_CLIENT_ID_APP, templateDmCback);
  AttRegister(templateAttCback);
  AttConnRegister(AppServerConnCback);
  AttsCccRegister(TEMPLATE_NUM_CCC_IDX, (attsCccSet_t *) templateCccSet, templateCccCback);

  /* Register for app framework callbacks */
  // AppUiBtnRegister(fitBtnCback);

  /* Register for app framework discovery callbacks */
  AppDiscRegister(watchDiscCback);

  /* Initialize attribute server database */
  SvcCoreGattCbackRegister(GattReadCback, GattWriteCback);
  SvcCoreAddGroup();
  // SvcDisAddGroup();
  // SvcBattCbackRegister(BasReadCback, NULL);
  // SvcBattAddGroup();

  /* Set Service Changed CCCD index. */
  GattSetSvcChangedIdx(TEMPLATE_GATT_SC_CCC_IDX);

  /* Set running speed and cadence features */
  // RscpsSetFeatures(RSCS_ALL_FEATURES);

  /* Reset the device */
  DmDevReset();
}
