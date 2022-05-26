#include "sdk_common.h"
#include <stdlib.h>

#include "ble.h"
#include "ble_cus_c.h"
#include "ble_gattc.h"
#include "ble_srv_common.h"
#include "app_error.h"

#define NRF_LOG_MODULE_NAME ble_cus_c
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();


/**@brief Function for intercepting the errors of GATTC and the BLE GATT Queue.
 *
 * @param[in] nrf_error   Error code.
 * @param[in] p_ctx       Parameter from the event handler.
 * @param[in] conn_handle Connection handle.
 */
static void gatt_error_handler(uint32_t   nrf_error,
                               void     * p_ctx,
                               uint16_t   conn_handle)
{
    ble_cus_c_t * p_ble_cus_c = (ble_cus_c_t *)p_ctx;

    NRF_LOG_DEBUG("A GATT Client error has occurred on conn_handle: 0X%X", conn_handle);

    if (p_ble_cus_c->error_handler != NULL)
    {
        p_ble_cus_c->error_handler(nrf_error);
    }
}


void ble_cus_c_on_db_disc_evt(ble_cus_c_t * p_ble_cus_c, ble_db_discovery_evt_t * p_evt)
{
    ble_cus_c_evt_t cus_c_evt;
    memset(&cus_c_evt,0,sizeof(ble_cus_c_evt_t));

    ble_gatt_db_char_t * p_chars = p_evt->params.discovered_db.charateristics;

    // Check if the CUS was discovered.
    if (    (p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE)
        &&  (p_evt->params.discovered_db.srv_uuid.uuid == BLE_UUID_CUS_SERVICE)
        &&  (p_evt->params.discovered_db.srv_uuid.type == p_ble_cus_c->uuid_type))
    {
        for (uint32_t i = 0; i < p_evt->params.discovered_db.char_count; i++)
        {
            switch (p_chars[i].characteristic.uuid.uuid)
            {
                case BLE_UUID_CUS_CP_CHARACTERISTIC:
                    cus_c_evt.handles.cus_cp_handle = p_chars[i].characteristic.handle_value;
                    cus_c_evt.handles.cus_cp_cccd_handle = p_chars[i].cccd_handle;
                    break;

                case BLE_UUID_CUS_FL_CHARACTERISTIC:
                    cus_c_evt.handles.cus_fl_handle = p_chars[i].characteristic.handle_value;
                    break;

                case BLE_UUID_CUS_RI_CHARACTERISTIC:
                    cus_c_evt.handles.cus_ri_handle = p_chars[i].characteristic.handle_value;
                    cus_c_evt.handles.cus_ri_cccd_handle = p_chars[i].cccd_handle;
                    break;

                case BLE_UUID_CUS_FS_CHARACTERISTIC:
                    cus_c_evt.handles.cus_fs_handle = p_chars[i].characteristic.handle_value;
                    break;

                default:
                    break;
            }
        }
        if (p_ble_cus_c->evt_handler != NULL)
        {
            cus_c_evt.conn_handle = p_evt->conn_handle;
            cus_c_evt.evt_type    = BLE_CUS_C_EVT_DISCOVERY_COMPLETE;
            p_ble_cus_c->evt_handler(p_ble_cus_c, &cus_c_evt);
        }
    }
}

/**@brief     Function for handling Handle Value Notification received from the SoftDevice.
 *
 * @details   This function uses the Handle Value Notification received from the SoftDevice
 *            and checks if it is a notification from the peer.
 *            If it is, this function decodes the data and sends it to the application.
 *            
 * @param[in] p_ble_cus_c Pointer to the CUS Client structure.
 * @param[in] p_ble_evt   Pointer to the BLE event received.
 */
static void on_hvx(ble_cus_c_t * p_ble_cus_c, ble_evt_t const * p_ble_evt)
{
    // HVX can only occur from remote sending.
    if (   (p_ble_cus_c->handles.cus_cp_handle != BLE_GATT_HANDLE_INVALID)
        && (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ble_cus_c->handles.cus_cp_handle)
        && (p_ble_cus_c->evt_handler != NULL))
    {
        ble_cus_c_evt_t ble_cus_c_evt;

        ble_cus_c_evt.evt_type = BLE_CUS_C_EVT_CP_UPDATED;
        ble_cus_c_evt.p_data   = (uint8_t *)p_ble_evt->evt.gattc_evt.params.hvx.data;
        ble_cus_c_evt.data_len = p_ble_evt->evt.gattc_evt.params.hvx.len;

        p_ble_cus_c->evt_handler(p_ble_cus_c, &ble_cus_c_evt);
        NRF_LOG_DEBUG("Control point updated.");
    }

    if (   (p_ble_cus_c->handles.cus_ri_handle != BLE_GATT_HANDLE_INVALID)
        && (p_ble_evt->evt.gattc_evt.params.hvx.handle == p_ble_cus_c->handles.cus_ri_handle)
        && (p_ble_cus_c->evt_handler != NULL))
    {
        ble_cus_c_evt_t ble_cus_c_evt;

        ble_cus_c_evt.evt_type = BLE_CUS_C_EVT_RECEIVED_REQ_IDX;
        ble_cus_c_evt.p_data   = (uint8_t *)p_ble_evt->evt.gattc_evt.params.hvx.data;
        ble_cus_c_evt.data_len = p_ble_evt->evt.gattc_evt.params.hvx.len;

        p_ble_cus_c->evt_handler(p_ble_cus_c, &ble_cus_c_evt);
        NRF_LOG_DEBUG("Received a request index.");
    }
}

uint32_t ble_cus_c_init(ble_cus_c_t * p_ble_cus_c, ble_cus_c_init_t * p_ble_cus_c_init)
{
    uint32_t      err_code;
    ble_uuid_t    fwu_uuid;
    ble_uuid128_t cus_base_uuid = CUS_BASE_UUID;

    VERIFY_PARAM_NOT_NULL(p_ble_cus_c);
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c_init);
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c_init->p_gatt_queue);

    err_code = sd_ble_uuid_vs_add(&cus_base_uuid, &p_ble_cus_c->uuid_type);
    VERIFY_SUCCESS(err_code);

    fwu_uuid.type = p_ble_cus_c->uuid_type;
    fwu_uuid.uuid = BLE_UUID_CUS_SERVICE;

    p_ble_cus_c->conn_handle           = BLE_CONN_HANDLE_INVALID;
    p_ble_cus_c->evt_handler           = p_ble_cus_c_init->evt_handler;
    p_ble_cus_c->error_handler         = p_ble_cus_c_init->error_handler;
    p_ble_cus_c->handles.cus_cp_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_cus_c->handles.cus_cp_cccd_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_cus_c->handles.cus_fl_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_cus_c->handles.cus_ri_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_cus_c->handles.cus_ri_cccd_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_cus_c->handles.cus_fs_handle = BLE_GATT_HANDLE_INVALID;
    p_ble_cus_c->p_gatt_queue          = p_ble_cus_c_init->p_gatt_queue;

    return ble_db_discovery_evt_register(&fwu_uuid);
}

void ble_cus_c_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_cus_c_t * p_ble_cus_c = (ble_cus_c_t *)p_context;

    if ((p_ble_cus_c == NULL) || (p_ble_evt == NULL))
    {
        return;
    }

    if ( (p_ble_cus_c->conn_handle == BLE_CONN_HANDLE_INVALID)
       ||(p_ble_cus_c->conn_handle != p_ble_evt->evt.gap_evt.conn_handle)
       )
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTC_EVT_HVX:
            on_hvx(p_ble_cus_c, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            if (p_ble_evt->evt.gap_evt.conn_handle == p_ble_cus_c->conn_handle
                    && p_ble_cus_c->evt_handler != NULL)
            {
                ble_cus_c_evt_t cus_c_evt;

                cus_c_evt.evt_type = BLE_CUS_C_EVT_DISCONNECTED;

                p_ble_cus_c->conn_handle = BLE_CONN_HANDLE_INVALID;
                p_ble_cus_c->evt_handler(p_ble_cus_c, &cus_c_evt);
            }
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for creating a message for writing to the CCCD. */
static uint32_t cp_cccd_configure(ble_cus_c_t * p_ble_cus_c, bool notification_enable)
{
    nrf_ble_gq_req_t cccd_req;
    uint8_t          cccd[BLE_CCCD_VALUE_LEN];
    uint16_t         cccd_val = notification_enable ? BLE_GATT_HVX_NOTIFICATION : 0;

    memset(&cccd_req, 0, sizeof(nrf_ble_gq_req_t));

    cccd[0] = LSB_16(cccd_val);
    cccd[1] = MSB_16(cccd_val);

    cccd_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    cccd_req.error_handler.cb            = gatt_error_handler;
    cccd_req.error_handler.p_ctx         = p_ble_cus_c;
    cccd_req.params.gattc_write.handle   = p_ble_cus_c->handles.cus_cp_cccd_handle;
    cccd_req.params.gattc_write.len      = BLE_CCCD_VALUE_LEN;
    cccd_req.params.gattc_write.offset   = 0;
    cccd_req.params.gattc_write.p_value  = cccd;
    cccd_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_REQ;
    cccd_req.params.gattc_write.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    return nrf_ble_gq_item_add(p_ble_cus_c->p_gatt_queue, &cccd_req, p_ble_cus_c->conn_handle);
}

uint32_t ble_cus_c_cp_notif_enable(ble_cus_c_t * p_ble_cus_c)
{
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c);

    if ( (p_ble_cus_c->conn_handle == BLE_CONN_HANDLE_INVALID)
       ||(p_ble_cus_c->handles.cus_cp_cccd_handle == BLE_GATT_HANDLE_INVALID)
       )
    {
        return NRF_ERROR_INVALID_STATE;
    }
    return cp_cccd_configure(p_ble_cus_c, true);
}

/**@brief Function for creating a message for writing to the CCCD. */
static uint32_t ri_cccd_configure(ble_cus_c_t * p_ble_cus_c, bool notification_enable)
{
    nrf_ble_gq_req_t cccd_req;
    uint8_t          cccd[BLE_CCCD_VALUE_LEN];
    uint16_t         cccd_val = notification_enable ? BLE_GATT_HVX_NOTIFICATION : 0;

    memset(&cccd_req, 0, sizeof(nrf_ble_gq_req_t));

    cccd[0] = LSB_16(cccd_val);
    cccd[1] = MSB_16(cccd_val);

    cccd_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    cccd_req.error_handler.cb            = gatt_error_handler;
    cccd_req.error_handler.p_ctx         = p_ble_cus_c;
    cccd_req.params.gattc_write.handle   = p_ble_cus_c->handles.cus_ri_cccd_handle;
    cccd_req.params.gattc_write.len      = BLE_CCCD_VALUE_LEN;
    cccd_req.params.gattc_write.offset   = 0;
    cccd_req.params.gattc_write.p_value  = cccd;
    cccd_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_REQ;
    cccd_req.params.gattc_write.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    return nrf_ble_gq_item_add(p_ble_cus_c->p_gatt_queue, &cccd_req, p_ble_cus_c->conn_handle);
}

uint32_t ble_cus_c_ri_notif_enable(ble_cus_c_t * p_ble_cus_c)
{
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c);

    if ( (p_ble_cus_c->conn_handle == BLE_CONN_HANDLE_INVALID)
       ||(p_ble_cus_c->handles.cus_ri_cccd_handle == BLE_GATT_HANDLE_INVALID)
       )
    {
        return NRF_ERROR_INVALID_STATE;
    }
    return ri_cccd_configure(p_ble_cus_c, true);
}

uint32_t ble_cus_c_cp_send(ble_cus_c_t * p_ble_cus_c, uint8_t * p_cp, uint16_t length)
{
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c);

    nrf_ble_gq_req_t write_req;

    memset(&write_req, 0, sizeof(nrf_ble_gq_req_t));

    if (length != 1)
    {
        NRF_LOG_WARNING("Invalid length.");
        return NRF_ERROR_INVALID_PARAM;
    }
    if (p_ble_cus_c->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        NRF_LOG_WARNING("Connection handle invalid.");
        return NRF_ERROR_INVALID_STATE;
    }

    write_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    write_req.error_handler.cb            = gatt_error_handler;
    write_req.error_handler.p_ctx         = p_ble_cus_c;
    write_req.params.gattc_write.handle   = p_ble_cus_c->handles.cus_cp_handle;
    write_req.params.gattc_write.len      = length;
    write_req.params.gattc_write.offset   = 0;
    write_req.params.gattc_write.p_value  = p_cp;
    write_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_CMD;
    write_req.params.gattc_write.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    return nrf_ble_gq_item_add(p_ble_cus_c->p_gatt_queue, &write_req, p_ble_cus_c->conn_handle);
}

uint32_t ble_cus_c_fl_send(ble_cus_c_t * p_ble_cus_c, uint8_t * p_fl, uint16_t length)
{
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c);

    nrf_ble_gq_req_t write_req;

    memset(&write_req, 0, sizeof(nrf_ble_gq_req_t));

    if (length != 1)
    {
        NRF_LOG_WARNING("Invalid length.");
        return NRF_ERROR_INVALID_PARAM;
    }
    if (p_ble_cus_c->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        NRF_LOG_WARNING("Connection handle invalid.");
        return NRF_ERROR_INVALID_STATE;
    }

    write_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    write_req.error_handler.cb            = gatt_error_handler;
    write_req.error_handler.p_ctx         = p_ble_cus_c;
    write_req.params.gattc_write.handle   = p_ble_cus_c->handles.cus_fl_handle;
    write_req.params.gattc_write.len      = length;
    write_req.params.gattc_write.offset   = 0;
    write_req.params.gattc_write.p_value  = p_fl;
    write_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_CMD;
    write_req.params.gattc_write.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    return nrf_ble_gq_item_add(p_ble_cus_c->p_gatt_queue, &write_req, p_ble_cus_c->conn_handle);
}

uint32_t ble_cus_c_fs_send(ble_cus_c_t * p_ble_cus_c, uint8_t * p_fs, uint16_t length)
{
    VERIFY_PARAM_NOT_NULL(p_ble_cus_c);

    nrf_ble_gq_req_t write_req;

    memset(&write_req, 0, sizeof(nrf_ble_gq_req_t));

    if (length != 8)
    {
        NRF_LOG_WARNING("Invalid length.");
        return NRF_ERROR_INVALID_PARAM;
    }
    if (p_ble_cus_c->conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        NRF_LOG_WARNING("Connection handle invalid.");
        return NRF_ERROR_INVALID_STATE;
    }

    write_req.type                        = NRF_BLE_GQ_REQ_GATTC_WRITE;
    write_req.error_handler.cb            = gatt_error_handler;
    write_req.error_handler.p_ctx         = p_ble_cus_c;
    write_req.params.gattc_write.handle   = p_ble_cus_c->handles.cus_fs_handle;
    write_req.params.gattc_write.len      = length;
    write_req.params.gattc_write.offset   = 0;
    write_req.params.gattc_write.p_value  = p_fs;
    write_req.params.gattc_write.write_op = BLE_GATT_OP_WRITE_CMD;
    write_req.params.gattc_write.flags    = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE;

    return nrf_ble_gq_item_add(p_ble_cus_c->p_gatt_queue, &write_req, p_ble_cus_c->conn_handle);
}

uint32_t ble_cus_c_handles_assign(ble_cus_c_t               * p_ble_cus,
                                  uint16_t                    conn_handle,
                                  ble_cus_c_handles_t const * p_peer_handles)
{
    VERIFY_PARAM_NOT_NULL(p_ble_cus);

    p_ble_cus->conn_handle = conn_handle;
    if (p_peer_handles != NULL)
    {
        p_ble_cus->handles.cus_cp_handle      = p_peer_handles->cus_cp_handle;
        p_ble_cus->handles.cus_cp_cccd_handle = p_peer_handles->cus_cp_cccd_handle;
        p_ble_cus->handles.cus_fl_handle      = p_peer_handles->cus_fl_handle;
        p_ble_cus->handles.cus_ri_handle      = p_peer_handles->cus_ri_handle;
        p_ble_cus->handles.cus_ri_cccd_handle = p_peer_handles->cus_ri_cccd_handle;
        p_ble_cus->handles.cus_fs_handle      = p_peer_handles->cus_fs_handle;
    }
    return nrf_ble_gq_conn_handle_register(p_ble_cus->p_gatt_queue, conn_handle);
}
