#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "nordic_common.h"
#include "app_error.h"
#include "ble_db_discovery.h"
#include "app_timer.h"
#include "app_util.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_hci.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "ble_cus_c.h"
#include "nrf_ble_gatt.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_ble_scan.h"

#include "nrfx_gpiote.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define SCAN_INTERVAL                   0x00A0                              /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW                     0x0098                              /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_DURATION                   0x0000                              /**< Timout when scanning. 0x0000 disables timeout. */

//#define MIN_CONNECTION_INTERVAL         MSEC_TO_UNITS(1000, UNIT_1_25_MS)    /**< Determines minimum connection interval in milliseconds. */
//#define MAX_CONNECTION_INTERVAL         MSEC_TO_UNITS(1000, UNIT_1_25_MS)     /**< Determines maximum connection interval in milliseconds. */
//#define SLAVE_LATENCY                   0                                   /**< Determines slave latency in terms of connection events. */
//#define SUPERVISION_TIMEOUT             MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Determines supervision time-out in units of 10 milliseconds. */

#define MIN_CONNECTION_INTERVAL         1600    /**< Determines minimum connection interval in milliseconds. */
#define MAX_CONNECTION_INTERVAL         1600     /**< Determines maximum connection interval in milliseconds. */
#define SLAVE_LATENCY                   1                                   /**< Determines slave latency in terms of connection events. */
#define SUPERVISION_TIMEOUT             3200     /**< Determines supervision time-out in units of 10 milliseconds. */


#define APP_BLE_CONN_CFG_TAG    1                                       /**< Tag that refers to the BLE stack configuration set with @ref sd_ble_cfg_set. The default tag is @ref BLE_CONN_CFG_TAG_DEFAULT. */
#define APP_BLE_OBSERVER_PRIO   3                                       /**< BLE observer priority of the application. There is no need to modify this value. */

#define CUS_SERVICE_UUID_TYPE   BLE_UUID_TYPE_VENDOR_BEGIN              /**< UUID type for the Custom Service (vendor specific). */


BLE_CUS_C_DEF(m_ble_cus_c);                                             /**< BLE Custom Service (CUS) client instance. */
NRF_BLE_GATT_DEF(m_gatt);                                               /**< GATT module instance. */
BLE_DB_DISCOVERY_DEF(m_db_disc);                                        /**< Database discovery module instance. */
NRF_BLE_SCAN_DEF(m_scan);                                               /**< Scanning Module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                        /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);

static uint16_t m_ble_cus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT; /**< Maximum length of data (in bytes) that can be transmitted to the peer. */

/**< Scan parameters requested for scanning and connection. */
static ble_gap_scan_params_t const m_scan_param =
        {
                .active        = 0x01,
#if (NRF_SD_BLE_API_VERSION > 7)
                .interval_us   = SCAN_INTERVAL * UNIT_0_625_MS,
                .window_us     = SCAN_WINDOW * UNIT_0_625_MS,
#else
                .interval      = SCAN_INTERVAL,
                .window        = SCAN_WINDOW,
#endif // (NRF_SD_BLE_API_VERSION > 7)
                .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
                .timeout       = SCAN_DURATION,
                .scan_phys     = BLE_GAP_PHY_1MBPS,
        };

static const ble_gap_conn_params_t conn_params = {
        .min_conn_interval = MIN_CONNECTION_INTERVAL,
        .max_conn_interval = MAX_CONNECTION_INTERVAL,
        .slave_latency = SLAVE_LATENCY,
        .conn_sup_timeout = SUPERVISION_TIMEOUT
};

//// dummy firmware
//static const uint8_t firmware[] = {
//0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
//0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
//};

// firmware 1: pure delay
//static const uint8_t firmware[] = {
//    0x82, 0xB0, 0x20, 0x23, 0x8D, 0xF8, 0x07, 0x30,
//    0x06, 0xE0, 0x9D, 0xF8, 0x07, 0x30, 0xDB, 0xB2,
//    0x01, 0x3B, 0xDB, 0xB2, 0x8D, 0xF8, 0x07, 0x30,
//    0x9D, 0xF8, 0x07, 0x30, 0xDB, 0xB2, 0x00, 0x2B,
//    0xF3, 0xD1, 0x00, 0xBF, 0x00, 0xBF, 0x02, 0xB0,
//    0x70, 0x47, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
//};

// firmware 2: same delay then toggle P0.16
static const uint8_t firmware[] = {
    0x84, 0xB0, 0x4F, 0xF4, 0x80, 0x33, 0x03, 0x93,
    0x20, 0x23, 0x8D, 0xF8, 0x07, 0x30, 0x06, 0xE0,
    0x9D, 0xF8, 0x07, 0x30, 0xDB, 0xB2, 0x01, 0x3B,
    0xDB, 0xB2, 0x8D, 0xF8, 0x07, 0x30, 0x9D, 0xF8,
    0x07, 0x30, 0xDB, 0xB2, 0x00, 0x2B, 0xF3, 0xD1,
    0x4F, 0xF0, 0xA0, 0x43, 0xD3, 0xF8, 0x04, 0x35,
    0x02, 0x93, 0x02, 0x9B, 0xDA, 0x43, 0x4F, 0xF0,
    0xA0, 0x41, 0x03, 0x9B, 0x13, 0x40, 0xC1, 0xF8,
    0x08, 0x35, 0x4F, 0xF0, 0xA0, 0x41, 0x02, 0x9A,
    0x03, 0x9B, 0x13, 0x40, 0xC1, 0xF8, 0x0C, 0x35,
    0x00, 0xBF, 0x04, 0xB0, 0x70, 0x47, 0xff, 0xff
};

static const uint8_t firmware_len = sizeof(firmware) / (sizeof(uint8_t) * 8);

uint8_t fwu_control_point;
uint8_t fwu_request_index;

/**@brief CUS UUID. */
static ble_uuid_t const m_cus_uuid =
{
    .uuid = BLE_UUID_CUS_SERVICE,
    .type = CUS_SERVICE_UUID_TYPE
};


/**@brief Function for handling asserts in the SoftDevice.
 *
 * @details This function is called in case of an assert in the SoftDevice.
 *
 * @warning This handler is only an example and is not meant for the final product. You need to analyze
 *          how your product is supposed to react in case of assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing assert call.
 * @param[in] p_file_name  File name of the failing assert call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for handling the Custom Service Client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void cus_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function to start scanning. */
static void scan_start(void)
{
    ret_code_t ret;

    ret = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(ret);

    ret = bsp_indication_set(BSP_INDICATE_SCANNING);
    APP_ERROR_CHECK(ret);
}


/**@brief Function for handling Scanning Module events.
 */
static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    ret_code_t err_code;

    switch(p_scan_evt->scan_evt_id)
    {
         case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
         {
              err_code = p_scan_evt->params.connecting_err.err_code;
              APP_ERROR_CHECK(err_code);
         } break;

         case NRF_BLE_SCAN_EVT_CONNECTED:
         {
              ble_gap_evt_connected_t const * p_connected =
                               p_scan_evt->params.connected.p_connected;
             // Scan is automatically stopped by the connection.
             NRF_LOG_INFO("Connecting to target %02x%02x%02x%02x%02x%02x",
                      p_connected->peer_addr.addr[0],
                      p_connected->peer_addr.addr[1],
                      p_connected->peer_addr.addr[2],
                      p_connected->peer_addr.addr[3],
                      p_connected->peer_addr.addr[4],
                      p_connected->peer_addr.addr[5]
                      );
         } break;

         case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
         {
             NRF_LOG_INFO("Scan timed out.");
             scan_start();
         } break;

         default:
             break;
    }
}


/**@brief Function for initializing the scanning and setting the filters.
 */
static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;
    init_scan.p_scan_param = &m_scan_param;
    init_scan.p_conn_param = &conn_params;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &m_cus_uuid);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_UUID_FILTER, false);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling database discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function forwards the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_cus_c_on_db_disc_evt(&m_ble_cus_c, p_evt);
}


/**@brief Function for handling control point updated by the Custom Service.
 */
static void ble_cus_control_point_updated(ble_cus_c_t * p_ble_cus_c, uint8_t * p_data, uint16_t data_len) {
    ret_code_t err_code;

    NRF_LOG_DEBUG("Control Point updated.");
    NRF_LOG_HEXDUMP_DEBUG(p_data, data_len);

    if (data_len != 1) {
        err_code = NRF_ERROR_INVALID_LENGTH;
        APP_ERROR_CHECK(err_code);
    }

    //if (*p_data == 0x00) {
    //  // firmware update finished
    //  err_code = sd_ble_gap_disconnect(p_ble_cus_c->conn_handle,
    //                                   BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    //  APP_ERROR_CHECK(err_code);
    //}
}

/**@brief Function for handling request index received by the Custom Service.
 */
static void ble_cus_request_index_received(ble_cus_c_t * p_ble_cus_c, uint8_t * p_data, uint16_t data_len) {
    ret_code_t err_code;

    NRF_LOG_DEBUG("Request Index received.");
    NRF_LOG_HEXDUMP_DEBUG(p_data, data_len);

    if (data_len != 1) {
        err_code = NRF_ERROR_INVALID_LENGTH;
        APP_ERROR_CHECK(err_code);
    }

    if (*p_data < firmware_len) {
        // send the requested firmware section
        uint8_t *firmware_section = (uint8_t*)firmware + (*p_data * 8);
        err_code = ble_cus_c_fs_send(p_ble_cus_c, firmware_section, 8);
        APP_ERROR_CHECK(err_code);    
    }
}

/**@brief Callback handling Custom Service (CUS) client events.
 *
 * @details This function is called to notify the application of CUS client events.
 *
 * @param[in]   p_ble_cus_c   CUS client handle. This identifies the CUS client.
 * @param[in]   p_ble_cus_evt Pointer to the CUS client event.
 */

/**@snippet [Handling events from the ble_cus_c module] */
static void ble_cus_c_evt_handler(ble_cus_c_t * p_ble_cus_c, ble_cus_c_evt_t const * p_ble_cus_evt)
{
    ret_code_t err_code;

    switch (p_ble_cus_evt->evt_type)
    {
        case BLE_CUS_C_EVT_DISCOVERY_COMPLETE:
            NRF_LOG_INFO("Discovery complete.");
            err_code = ble_cus_c_handles_assign(p_ble_cus_c, p_ble_cus_evt->conn_handle, &p_ble_cus_evt->handles);
            APP_ERROR_CHECK(err_code);

            // enable control point notification
            err_code = ble_cus_c_cp_notif_enable(p_ble_cus_c);
            APP_ERROR_CHECK(err_code);

            // enable request index notification
            err_code = ble_cus_c_ri_notif_enable(p_ble_cus_c);
            APP_ERROR_CHECK(err_code);

            // send firmware length
            err_code = ble_cus_c_fl_send(p_ble_cus_c, (uint8_t*)&firmware_len, 1);
            APP_ERROR_CHECK(err_code);

            // write 0x01 to control point, initiate firmware update
            fwu_control_point = 0x01;
            err_code = ble_cus_c_cp_send(p_ble_cus_c, &fwu_control_point, 1);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_INFO("Initiated firmware update.");
            break;

        case BLE_CUS_C_EVT_CP_UPDATED:
            ble_cus_control_point_updated(p_ble_cus_c, p_ble_cus_evt->p_data, p_ble_cus_evt->data_len);
            break;

        case BLE_CUS_C_EVT_RECEIVED_REQ_IDX:
            ble_cus_request_index_received(p_ble_cus_c, p_ble_cus_evt->p_data, p_ble_cus_evt->data_len);
            break;

        case BLE_CUS_C_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            //scan_start();
            break;
    }
}
/**@snippet [Handling events from the ble_cus_c module] */


/**
 * @brief Function for handling shutdown events.
 *
 * @param[in]   event       Shutdown type.
 */
static bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    ret_code_t err_code;

    err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            // Prepare wakeup buttons.
            err_code = bsp_btn_ble_sleep_mode_prepare();
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }

    return true;
}

NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler, APP_SHUTDOWN_HANDLER_PRIORITY);


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t            err_code;
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            err_code = ble_cus_c_handles_assign(&m_ble_cus_c, p_ble_evt->evt.gap_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);

            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);

            // start discovery of services. The CUS Client waits for a discovery result
            err_code = ble_db_discovery_start(&m_db_disc, p_ble_evt->evt.gap_evt.conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:

            NRF_LOG_INFO("Disconnected. conn_handle: 0x%x, reason: 0x%x",
                         p_gap_evt->conn_handle,
                         p_gap_evt->params.disconnected.reason);
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_INFO("Connection Request timed out.");
            }
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported.
            err_code = sd_ble_gap_sec_params_reply(p_ble_evt->evt.gap_evt.conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            // Accepting parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            break;
    }
}

void SWI0_EGU0_IRQHandler(void) {
  if (NRF_EGU0->EVENTS_TRIGGERED[0]) {
    /* Clear event register */
    NRF_EGU0->EVENTS_TRIGGERED[0] = 0;
    nrfx_gpiote_out_set(31);
  } else { 
    /* Clear event register */
    NRF_EGU0->EVENTS_TRIGGERED[1] = 0;
    NRF_EGU0->EVENTS_TRIGGERED[2] = 0;
    nrfx_gpiote_out_clear(31);
  }
}

void TraceTimingConfig() {
    ret_code_t err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_out_config_t config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(false);
    nrfx_gpiote_out_init(31, &config);

    NVIC_SetPriority(SWI0_EGU0_IRQn, 2);
    NVIC_EnableIRQ(SWI0_EGU0_IRQn);

    NRF_EGU0->INTENSET = 1 << 0;
    NRF_PPI->CH[1].EEP = (uint32_t) &NRF_RADIO->EVENTS_READY;  /* configure event */
    NRF_PPI->CH[1].TEP = (uint32_t) &NRF_EGU0->TASKS_TRIGGER[0];    /* configure task */
    NRF_PPI->CHENSET = 1 << 1;                            /* enable channel */

    NRF_EGU0->INTENSET = 1 << 1;
    NRF_PPI->CH[2].EEP = (uint32_t) &NRF_RADIO->EVENTS_END;  /* configure event */
    NRF_PPI->CH[2].TEP = (uint32_t) &NRF_EGU0->TASKS_TRIGGER[1];    /* configure task */
    NRF_PPI->CHENSET = 1 << 2;                            /* enable channel */

    NRF_EGU0->INTENSET = 1 << 2;
    NRF_PPI->CH[3].EEP = (uint32_t) &NRF_RADIO->EVENTS_DISABLED;  /* configure event */
    NRF_PPI->CH[3].TEP = (uint32_t) &NRF_EGU0->TASKS_TRIGGER[2];    /* configure task */
    NRF_PPI->CHENSET = 1 << 3;                            /* enable channel */
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

    TraceTimingConfig();
}


/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)
    {
        NRF_LOG_INFO("ATT MTU exchange completed.");

        m_ble_cus_max_data_len = p_evt->params.att_mtu_effective;
        NRF_LOG_INFO("Ble CUS max data length set to 0x%X(%d)", m_ble_cus_max_data_len, m_ble_cus_max_data_len);
    }
}


/**@brief Function for initializing the GATT library. */
void gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_central_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in] event  Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    ret_code_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_ble_cus_c.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        default:
            break;
    }
}


/**@brief Function for initializing the Nordic UART Service (CUS) client. */
static void cus_c_init(void)
{
    ret_code_t       err_code;
    ble_cus_c_init_t init;

    init.evt_handler   = ble_cus_c_evt_handler;
    init.error_handler = cus_error_handler;
    init.p_gatt_queue  = &m_ble_gatt_queue;

    err_code = ble_cus_c_init(&m_ble_cus_c, &init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing buttons and leds. */
static void buttons_leds_init(void)
{
    ret_code_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LEDS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the timer. */
static void timer_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the nrf log module. */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


/** @brief Function for initializing the database discovery module. */
static void db_discovery_init(void)
{
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details Handles any pending log operations, then sleeps until the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


int main(void)
{
    // Initialize.
    log_init();
    timer_init();
    buttons_leds_init();
    db_discovery_init();
    power_management_init();
    ble_stack_init();
    gatt_init();
    cus_c_init();
    scan_init();

    // Start execution.
    NRF_LOG_INFO("BLE Custom Service central started.");
    scan_start();

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}
