/**@file
 *
 * @defgroup ble_cus_c Custom service client
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Custom service client module.
 *
 * @details  This module contains the APIs and types exposed by the Custom service client
 *           module. 
 *
 * @note    The application must register this module as the BLE event observer by using the
 *          NRF_SDH_BLE_OBSERVER macro. Example:
 *          @code
 *              ble_cus_c_t instance;
 *              NRF_SDH_BLE_OBSERVER(anything, BLE_CUS_C_BLE_OBSERVER_PRIO,
 *                                   ble_cus_c_on_ble_evt, &instance);
 *          @endcode
 *
 */


#ifndef BLE_CUS_C_H__
#define BLE_CUS_C_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_gatt.h"
#include "ble_db_discovery.h"
#include "ble_srv_common.h"
#include "nrf_ble_gq.h"
#include "nrf_sdh_ble.h"

#include "sdk_config.h"

// <o> BLE_CUS_C_BLE_OBSERVER_PRIO  
// <i> Priority with which BLE events are dispatched to the Custom Service client.
#ifndef BLE_CUS_C_BLE_OBSERVER_PRIO
#define BLE_CUS_C_BLE_OBSERVER_PRIO 2
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_cus_c instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_CUS_C_DEF(_name)                                                                        \
static ble_cus_c_t _name;                                                                           \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_CUS_C_BLE_OBSERVER_PRIO,                                                   \
                     ble_cus_c_on_ble_evt, &_name)

/** @brief Macro for defining multiple ble_cus_c instances.
 *
 * @param   _name   Name of the array of instances.
 * @param   _cnt    Number of instances to define.
 * @hideinitializer
 */
//#define BLE_CUS_C_ARRAY_DEF(_name, _cnt)                 \
//static ble_cus_c_t _name[_cnt];                          \
//NRF_SDH_BLE_OBSERVERS(_name ## _obs,                     \
//                      BLE_CUS_C_BLE_OBSERVER_PRIO,       \
//                      ble_cus_c_on_ble_evt, &_name, _cnt)

#define CUS_BASE_UUID                   {{0x51, 0xe2, 0x47, 0xf1, 0x7e, 0x1a, 0x67, 0x88, 0x47, 0x44, 0x90, 0x3c, 0x00, 0x00, 0xcf, 0x3c}} /**< Used vendor-specific UUID. */

#define BLE_UUID_CUS_SERVICE            0x0001                      /**< The UUID of the custom service. */
#define BLE_UUID_CUS_CP_CHARACTERISTIC  0x0002                      /**< The UUID of the Control Point Characteristic. */
#define BLE_UUID_CUS_FL_CHARACTERISTIC  0x0003                      /**< The UUID of the FW Length Characteristic. */
#define BLE_UUID_CUS_RI_CHARACTERISTIC  0x0004                      /**< The UUID of the Request Index Characteristic. */
#define BLE_UUID_CUS_FS_CHARACTERISTIC  0x0005                      /**< The UUID of the FW Section Characteristic. */

/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
    #define BLE_CUS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE)
#else
    #define BLE_CUS_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT)
    #warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif


/**@brief CUS Client event type. */
typedef enum
{
    BLE_CUS_C_EVT_DISCOVERY_COMPLETE,   /**< Event indicating that the CUS service and its characteristics were found. */
    BLE_CUS_C_EVT_CP_UPDATED,           /**< Event indicating that the control point is updated. */
    BLE_CUS_C_EVT_RECEIVED_REQ_IDX,     /**< Event indicating that a request index is received. */
    BLE_CUS_C_EVT_DISCONNECTED          /**< Event indicating that the CUS server disconnected. */
} ble_cus_c_evt_type_t;

/**@brief Handles on the connected peer device needed to interact with it. */
typedef struct
{
    uint16_t cus_cp_handle;      /**< Handle of the CUS CP characteristic, as provided by a discovery. */
    uint16_t cus_cp_cccd_handle; /**< Handle of the CCCD of the CUS CP characteristic, as provided by a discovery. */
    uint16_t cus_fl_handle;      /**< Handle of the CUS FL characteristic, as provided by a discovery. */
    uint16_t cus_ri_handle;      /**< Handle of the CUS RI characteristic, as provided by a discovery. */
    uint16_t cus_ri_cccd_handle; /**< Handle of the CCCD of the CUS RI characteristic, as provided by a discovery. */
    uint16_t cus_fs_handle;      /**< Handle of the CUS FS characteristic, as provided by a discovery. */
} ble_cus_c_handles_t;

/**@brief Structure containing the CUS event data received from the peer. */
typedef struct
{
    ble_cus_c_evt_type_t evt_type;
    uint16_t             conn_handle;
    uint16_t             max_data_len;
    uint8_t            * p_data;
    uint16_t             data_len;
    ble_cus_c_handles_t  handles;     /**< Handles on which the custom service characteristics were discovered on the peer device. This is filled if the evt_type is @ref BLE_CUS_C_EVT_DISCOVERY_COMPLETE.*/
} ble_cus_c_evt_t;

// Forward declaration of the ble_cus_t type.
typedef struct ble_cus_c_s ble_cus_c_t;

/**@brief   Event handler type.
 *
 * @details This is the type of the event handler that is to be provided by the application
 *          of this module to receive events.
 */
typedef void (* ble_cus_c_evt_handler_t)(ble_cus_c_t * p_ble_cus_c, ble_cus_c_evt_t const * p_evt);

/**@brief CUS Client structure. */
struct ble_cus_c_s
{
    uint8_t                   uuid_type;      /**< UUID type. */
    uint16_t                  conn_handle;    /**< Handle of the current connection. Set with @ref ble_cus_c_handles_assign when connected. */
    ble_cus_c_handles_t       handles;        /**< Handles on the connected peer device needed to interact with it. */
    ble_cus_c_evt_handler_t   evt_handler;    /**< Application event handler to be called when there is an event related to the CUS. */
    ble_srv_error_handler_t   error_handler;  /**< Function to be called in case of an error. */
    nrf_ble_gq_t            * p_gatt_queue;   /**< Pointer to BLE GATT Queue instance. */
};

/**@brief CUS Client initialization structure. */
typedef struct
{
    ble_cus_c_evt_handler_t   evt_handler;    /**< Application event handler to be called when there is an event related to the CUS. */
    ble_srv_error_handler_t   error_handler;  /**< Function to be called in case of an error. */
    nrf_ble_gq_t            * p_gatt_queue;   /**< Pointer to BLE GATT Queue instance. */
} ble_cus_c_init_t;


/**@brief     Function for initializing the Nordic UART client module.
 *
 * @details   This function registers with the Database Discovery module
 *            for the CUS. The Database Discovery module looks for the presence
 *            of a CUS instance at the peer when a discovery is started.
 *            
 * @param[in] p_ble_cus_c      Pointer to the CUS client structure.
 * @param[in] p_ble_cus_c_init Pointer to the CUS initialization structure that contains the
 *                             initialization information.
 *
 * @retval    NRF_SUCCESS If the module was initialized successfully.
 * @retval    err_code    Otherwise, this function propagates the error code
 *                        returned by the Database Discovery module API
 *                        @ref ble_db_discovery_evt_register.
 */
uint32_t ble_cus_c_init(ble_cus_c_t * p_ble_cus_c, ble_cus_c_init_t * p_ble_cus_c_init);


/**@brief Function for handling events from the Database Discovery module.
 *
 * @details This function handles an event from the Database Discovery module, and determines
 *          whether it relates to the discovery of CUS at the peer. If it does, the function
 *          calls the application's event handler to indicate that CUS was
 *          discovered at the peer. The function also populates the event with service-related
 *          information before providing it to the application.
 *
 * @param[in] p_ble_cus_c Pointer to the CUS client structure.
 * @param[in] p_evt       Pointer to the event received from the Database Discovery module.
 */
 void ble_cus_c_on_db_disc_evt(ble_cus_c_t * p_ble_cus_c, ble_db_discovery_evt_t * p_evt);


/**@brief     Function for handling BLE events from the SoftDevice.
 *
 * @details   This function handles the BLE events received from the SoftDevice. If a BLE
 *            event is relevant to the CUS module, the function uses the event's data to update
 *            internal variables and, if necessary, send events to the application.
 *
 * @param[in] p_ble_evt     Pointer to the BLE event.
 * @param[in] p_context     Pointer to the CUS client structure.
 */
void ble_cus_c_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief   Function for requesting the peer to start sending notification.
 *
 * @details This function enables notifications at the peer
 *          by writing to the CCCD.
 *
 * @param   p_ble_cus_c Pointer to the CUS client structure.
 *
 * @retval  NRF_SUCCESS If the operation was successful. 
 * @retval  err_code 	Otherwise, this API propagates the error code returned by function @ref nrf_ble_gq_item_add.
 */
uint32_t ble_cus_c_cp_notif_enable(ble_cus_c_t * p_ble_cus_c);
uint32_t ble_cus_c_ri_notif_enable(ble_cus_c_t * p_ble_cus_c);


/**@brief Function for sending to the server.
 *
 * @details This function writes the characteristic of the server.
 *
 * @param[in] p_ble_cus_c Pointer to the CUS client structure.
 * @param[in] p_data      Data to be sent.
 * @param[in] length      Length of the data.
 *
 * @retval NRF_SUCCESS If the string was sent successfully. 
 * @retval err_code    Otherwise, this API propagates the error code returned by function @ref nrf_ble_gq_item_add.
 */
uint32_t ble_cus_c_cp_send(ble_cus_c_t * p_ble_cus_c, uint8_t * p_cp, uint16_t length);
uint32_t ble_cus_c_fl_send(ble_cus_c_t * p_ble_cus_c, uint8_t * p_fl, uint16_t length);
uint32_t ble_cus_c_fs_send(ble_cus_c_t * p_ble_cus_c, uint8_t * p_fs, uint16_t length);


/**@brief Function for assigning handles to this instance of cus_c.
 *
 * @details Call this function when a link has been established with a peer to
 *          associate the link to this instance of the module. This makes it
 *          possible to handle several links and associate each link to a particular
 *          instance of this module. The connection handle and attribute handles are
 *          provided from the discovery event @ref BLE_CUS_C_EVT_DISCOVERY_COMPLETE.
 *
 * @param[in] p_ble_cus_c    Pointer to the CUS client structure instance to associate with these
 *                           handles.
 * @param[in] conn_handle    Connection handle to associated with the given CUS Instance.
 * @param[in] p_peer_handles Attribute handles on the CUS server that you want this CUS client to
 *                           interact with.
 *
 * @retval    NRF_SUCCESS    If the operation was successful.
 * @retval    NRF_ERROR_NULL If a p_cus was a NULL pointer.
 * @retval    err_code       Otherwise, this API propagates the error code returned 
 *                           by function @ref nrf_ble_gq_item_add.
 */
uint32_t ble_cus_c_handles_assign(ble_cus_c_t *               p_ble_cus_c,
                                  uint16_t                    conn_handle,
                                  ble_cus_c_handles_t const * p_peer_handles);


#ifdef __cplusplus
}
#endif

#endif // BLE_CUS_C_H__

/** @} */
