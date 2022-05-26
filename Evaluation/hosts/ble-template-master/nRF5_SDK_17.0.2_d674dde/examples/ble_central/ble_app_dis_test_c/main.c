/**
 * Copyright (c) 2014 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * @brief BLE LED Button Service central and client application main file.
 *
 * This file contains the source code for a sample client application using the LED Button service.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "nrf_pwr_mgmt.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_lbs_c.h"
#include "ble_dis_c.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_scan.h"
//#include "ble_radio_notification.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


#define CENTRAL_SCANNING_LED            BSP_BOARD_LED_0                     /**< Scanning LED will be on when the device is scanning. */
#define CENTRAL_CONNECTED_LED           BSP_BOARD_LED_1                     /**< Connected LED will be on when the device is connected. */
#define LEDBUTTON_LED                   BSP_BOARD_LED_2                     /**< LED to indicate a change of state of the the Button characteristic on the peer. */

#define SCAN_INTERVAL                   0x00A0                              /**< Determines scan interval in units of 0.625 millisecond. */
#define SCAN_WINDOW                     0x0098                              /**< Determines scan window in units of 0.625 millisecond. */
#define SCAN_DURATION                   0x0000                              /**< Timout when scanning. 0x0000 disables timeout. */

//#define MIN_CONNECTION_INTERVAL         MSEC_TO_UNITS(1000, UNIT_1_25_MS)    /**< Determines minimum connection interval in milliseconds. */
//#define MAX_CONNECTION_INTERVAL         MSEC_TO_UNITS(1000, UNIT_1_25_MS)     /**< Determines maximum connection interval in milliseconds. */
//#define SLAVE_LATENCY                   0                                   /**< Determines slave latency in terms of connection events. */
//#define SUPERVISION_TIMEOUT             MSEC_TO_UNITS(4000, UNIT_10_MS)     /**< Determines supervision time-out in units of 10 milliseconds. */
#define MIN_CONNECTION_INTERVAL         3199    /**< Determines minimum connection interval in milliseconds. */
#define MAX_CONNECTION_INTERVAL         3199     /**< Determines maximum connection interval in milliseconds. */
#define SLAVE_LATENCY                   3                                   /**< Determines slave latency in terms of connection events. */
#define SUPERVISION_TIMEOUT             3200     /**< Determines supervision time-out in units of 10 milliseconds. */

#define APP_BLE_CONN_CFG_TAG            1                                   /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                   /**< Application's BLE observer priority. You shouldn't need to modify this value. */

NRF_BLE_SCAN_DEF(m_scan);                                       /**< Scanning module instance. */
BLE_DIS_C_DEF(m_ble_dis_c);
NRF_BLE_GATT_DEF(m_gatt);                                       /**< GATT module instance. */
BLE_DB_DISCOVERY_DEF(m_db_disc);                                /**< DB discovery module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);

static char const m_target_periph_name[] = "Template";     /**< Name of the device we try to connect to. This name is searched in the scan report data*/


/**@brief Function to handle asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing ASSERT call.
 * @param[in] p_file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name) {
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for handling the client errors.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void dis_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void) {
    bsp_board_init(BSP_INIT_LEDS);
    nrf_gpio_cfg_output(31);
    nrf_gpio_pin_clear(31);
}


/**@brief Function to start scanning.
 */
static void scan_start(void) {
    ret_code_t err_code;

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);

    bsp_board_led_off(CENTRAL_CONNECTED_LED);
    bsp_board_led_on(CENTRAL_SCANNING_LED);
}

/**@brief Function for reading all characteristics that can be present in DIS.
 *
 * @return    The number of discovered characteristics.
 */
static uint32_t ble_dis_c_all_chars_read(void) {
    ret_code_t err_code;
    //uint32_t   disc_char_num = 0;

    //for (ble_dis_c_char_type_t char_type = (ble_dis_c_char_type_t) 0;
    //     char_type < BLE_DIS_C_CHAR_TYPES_NUM;
    //     char_type++)
    //{
    //    err_code = ble_dis_c_read(&m_ble_dis_c, char_type);

    //    // The NRF_ERROR_INVALID_STATE error code means that the characteristic is not present in DIS.
    //    if (err_code != NRF_ERROR_INVALID_STATE)
    //    {
    //        APP_ERROR_CHECK(err_code);
    //        disc_char_num++;
    //    }
    //}

    //return disc_char_num;

    static ble_dis_c_char_type_t char_type = (ble_dis_c_char_type_t) 0;
    if (char_type < BLE_DIS_C_CHAR_TYPES_NUM) {
        err_code = ble_dis_c_read(&m_ble_dis_c, char_type);
        // The NRF_ERROR_INVALID_STATE error code means that the characteristic is not present in DIS.
        if (err_code != NRF_ERROR_INVALID_STATE) {
            APP_ERROR_CHECK(err_code);
            char_type++;
        }
    }
    return (uint32_t) char_type;
}

/**@brief Handles events coming from the module.
 */
static void dis_c_evt_handler(ble_dis_c_t *p_ble_dis_c, ble_dis_c_evt_t const *p_evt) {
    switch (p_evt->evt_type) {
        case BLE_DIS_C_EVT_DISCOVERY_COMPLETE: {
            ret_code_t err_code;

            err_code = ble_dis_c_handles_assign(&m_ble_dis_c,
                                                p_evt->conn_handle,
                                                p_evt->params.disc_complete.handles);
            APP_ERROR_CHECK(err_code);
            NRF_LOG_INFO("DIS discovered on conn_handle 0x%x.", p_evt->conn_handle);

            //err_code = ble_dis_c_read(p_ble_dis_c, BLE_DIS_C_MANUF_NAME);
            ble_dis_c_all_chars_read();
            APP_ERROR_CHECK(err_code);
        }
            break; // BLE_DIS_C_EVT_DISCOVERY_COMPLETE

        case BLE_DIS_C_EVT_DIS_C_READ_RSP: {
            ble_dis_c_all_chars_read();
            NRF_LOG_INFO("DIS read response.");
        }
            break; // BLE_LBS_C_EVT_BUTTON_NOTIFICATION

        case BLE_DIS_C_EVT_DIS_C_READ_RSP_ERROR: {
            __NOP();
            NRF_LOG_INFO("DIS read response error.");
        }
            break;

        case BLE_DIS_C_EVT_DISCONNECTED: {
            __NOP();
            NRF_LOG_INFO("DIS disconnected.");
        }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
    ret_code_t err_code;

    // For readability.
    ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id) {
        // Upon connection, check which peripheral has connected (HR or RSC), initiate DB
        // discovery, update LEDs status and resume scanning if necessary. */
        case BLE_GAP_EVT_CONNECTED: {
            NRF_LOG_INFO("Connected.");

            err_code = ble_db_discovery_start(&m_db_disc, p_gap_evt->conn_handle);
            APP_ERROR_CHECK(err_code);

            // Update LEDs status, and check if we should be looking for more
            // peripherals to connect to.
            bsp_board_led_on(CENTRAL_CONNECTED_LED);
            bsp_board_led_off(CENTRAL_SCANNING_LED);
        }
            break;

            // Upon disconnection, reset the connection handle of the peer which disconnected, update
            // the LEDs status and start scanning again.
        case BLE_GAP_EVT_DISCONNECTED: {
            NRF_LOG_INFO("Disconnected.");
            scan_start();
        }
            break;

        case BLE_GAP_EVT_TIMEOUT: {
            // We have not specified a timeout for scanning, so only connection attemps can timeout.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN) {
                NRF_LOG_DEBUG("Connection request timed out.");
            }
        }
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            // Accept parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                                    &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        }
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
                    {
                            .rx_phys = BLE_GAP_PHY_AUTO,
                            .tx_phys = BLE_GAP_PHY_AUTO,
                    };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        }
            break;

        case BLE_GATTC_EVT_TIMEOUT: {
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        }
            break;

        case BLE_GATTS_EVT_TIMEOUT: {
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        }
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief client initialization.
 */
static void dis_c_init(void) {
    ret_code_t err_code;
    ble_dis_c_init_t dis_c_init_obj;

    memset(&dis_c_init_obj, 0, sizeof(ble_dis_c_init_t));
    //dis_c_init_obj.char_group = dis_c_char_group;
    dis_c_init_obj.evt_handler = dis_c_evt_handler;
    dis_c_init_obj.p_gatt_queue = &m_ble_gatt_queue;
    dis_c_init_obj.error_handler = dis_error_handler;

    err_code = ble_dis_c_init(&m_ble_dis_c, &dis_c_init_obj);
    APP_ERROR_CHECK(err_code);
}

//void ble_radio_notification_evt_handler(bool radio_active) {
//    if (radio_active) {
//        nrf_gpio_pin_set(31);
//    } else {
//        nrf_gpio_pin_clear(31);
//    }
//}

void SWI0_EGU0_IRQHandler(void) {
  if (NRF_EGU0->EVENTS_TRIGGERED[0]) {
    /* Clear event register */
    NRF_EGU0->EVENTS_TRIGGERED[0] = 0;
    nrfx_gpiote_out_clear(31);
  } else { 
    /* Clear event register */
    NRF_EGU0->EVENTS_TRIGGERED[1] = 0;
    NRF_EGU0->EVENTS_TRIGGERED[2] = 0;
    nrfx_gpiote_out_set(31);
  }
}

void TraceTimingConfig() {
    //ret_code_t err_code = nrfx_gpiote_init();
    //APP_ERROR_CHECK(err_code);

    //nrfx_gpiote_out_config_t config = NRFX_GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);
    //nrfx_gpiote_out_init(31, &config);

    //NRF_PPI->CH[1].EEP = (uint32_t) &NRF_RADIO->EVENTS_READY;  /* configure event */
    //NRF_PPI->CH[1].TEP = nrfx_gpiote_out_task_addr_get(31);    /* configure task */
    //NRF_PPI->CHENSET = 1 << 1;                            /* enable channel */

    //NRF_PPI->CH[2].EEP = (uint32_t) &NRF_RADIO->EVENTS_DISABLED;  /* configure event */
    //NRF_PPI->CH[2].TEP = nrfx_gpiote_out_task_addr_get(31);    /* configure task */
    //NRF_PPI->CHENSET = 1 << 2;                            /* enable channel */

    //nrfx_gpiote_out_task_enable(31);

    ret_code_t err_code = nrfx_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrfx_gpiote_out_config_t config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
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
 * @details Initializes the SoftDevice and the BLE event interrupts.
 */
static void ble_stack_init(void) {
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

//    err_code = ble_radio_notification_init(3, NRF_RADIO_NOTIFICATION_DISTANCE_800US,
//                                           ble_radio_notification_evt_handler);
//    APP_ERROR_CHECK(err_code);
    TraceTimingConfig();
}

/**@brief Function for handling Scaning events.
 *
 * @param[in]   p_scan_evt   Scanning event.
 */
static void scan_evt_handler(scan_evt_t const *p_scan_evt) {
    ret_code_t err_code;

    switch (p_scan_evt->scan_evt_id) {
        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
            break;
        default:
            break;
    }
}

/**@brief Function for handling database discovery events.
 *
 * @details This function is callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t *p_evt) {
    ble_dis_c_on_db_disc_evt(&m_ble_dis_c, p_evt);
}


/**@brief Database discovery initialization.
 */
static void db_discovery_init(void) {
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(db_init));

    db_init.evt_handler = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the log.
 */
static void log_init(void) {
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing the timer.
 */
static void timer_init(void) {
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Power manager. */
static void power_management_init(void) {
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

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

static void scan_init(void) {
    ret_code_t err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag = APP_BLE_CONN_CFG_TAG;
    init_scan.p_scan_param = &m_scan_param;
    init_scan.p_conn_param = &conn_params;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    // Setting filters for scanning.
    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_NAME_FILTER, false);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_NAME_FILTER, m_target_periph_name);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void) {
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details Handle any pending log operation(s), then sleep until the next event occurs.
 */
static void idle_state_handle(void) {
    NRF_LOG_FLUSH();
    nrf_pwr_mgmt_run();
}


int main(void) {
    // Initialize.
    log_init();
    timer_init();
    leds_init();
    power_management_init();
    ble_stack_init();
    scan_init();
    gatt_init();
    db_discovery_init();
    dis_c_init();

    // Start execution.
    NRF_LOG_INFO("CENTRAL example started.");
    scan_start();

    // Turn on the LED to signal scanning.
    bsp_board_led_on(CENTRAL_SCANNING_LED);

    // Enter main loop.
    for (;;) {
        idle_state_handle();
    }
}
