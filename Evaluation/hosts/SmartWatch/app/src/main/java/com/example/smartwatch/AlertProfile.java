package com.example.smartwatch;

import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;

import java.util.UUID;

public class AlertProfile {
    /* Alert Notification Service UUID */
    public static UUID ALERT_NOTIFICATION_SERVICE = UUID.fromString("00001811-0000-1000-8000-00805f9b34fb");

    public static UUID SUP_NEW_ALERT_CATEGORY    = UUID.fromString("00002A47-0000-1000-8000-00805f9b34fb");
    public static UUID NEW_ALERT = UUID.fromString("00002A46-0000-1000-8000-00805f9b34fb");
    public static UUID SUP_UNREAD_ALERT_CATEGORY = UUID.fromString("00002A48-0000-1000-8000-00805f9b34fb");
    public static UUID UNREAD_ALERT_STATUS = UUID.fromString("00002A45-0000-1000-8000-00805f9b34fb");
    public static UUID ALERT_NOTIFICATION_CONTROL_POINT = UUID.fromString("00002A44-0000-1000-8000-00805f9b34fb");

    public static final byte COMMAND_ENABLE_INCOMING_ALERT_NOTIFICATION     = 0x0;
    public static final byte COMMAND_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION     = 0x1;
    public static final byte COMMAND_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION     = 0x2;
    public static final byte COMMAND_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION     = 0x3;
    public static final byte COMMAND_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY     = 0x4;
    public static final byte COMMAND_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY     = 0x5;


    /* Mandatory Client Characteristic Config Descriptor */
    public static UUID CCCD = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    public static BluetoothGattService createAlertService() {
        BluetoothGattService service = new BluetoothGattService(ALERT_NOTIFICATION_SERVICE,
                BluetoothGattService.SERVICE_TYPE_PRIMARY);

        BluetoothGattCharacteristic supNewAlertCategory = new BluetoothGattCharacteristic(SUP_NEW_ALERT_CATEGORY,
                BluetoothGattCharacteristic.PROPERTY_READ,  BluetoothGattCharacteristic.PERMISSION_READ);

        BluetoothGattCharacteristic newAlert = new BluetoothGattCharacteristic(NEW_ALERT,
                BluetoothGattCharacteristic.PROPERTY_NOTIFY,  BluetoothGattCharacteristic.PERMISSION_READ);
        BluetoothGattDescriptor newAlertDescriptor = new BluetoothGattDescriptor(CCCD,
                //Read/write descriptor
                BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        newAlert.addDescriptor(newAlertDescriptor);

        BluetoothGattCharacteristic supUnreadAlertCategory = new BluetoothGattCharacteristic(SUP_UNREAD_ALERT_CATEGORY,
                BluetoothGattCharacteristic.PROPERTY_READ,  BluetoothGattCharacteristic.PERMISSION_READ);

        BluetoothGattCharacteristic unreadAlertStatus = new BluetoothGattCharacteristic(UNREAD_ALERT_STATUS,
                BluetoothGattCharacteristic.PROPERTY_NOTIFY,  BluetoothGattCharacteristic.PERMISSION_READ);
        BluetoothGattDescriptor unreadAlertStatusDescriptor = new BluetoothGattDescriptor(CCCD,
                //Read/write descriptor
                BluetoothGattDescriptor.PERMISSION_READ | BluetoothGattDescriptor.PERMISSION_WRITE);
        unreadAlertStatus.addDescriptor(unreadAlertStatusDescriptor);

        BluetoothGattCharacteristic alertNotificationControlPoint = new BluetoothGattCharacteristic(ALERT_NOTIFICATION_CONTROL_POINT,
                BluetoothGattCharacteristic.PROPERTY_WRITE,  BluetoothGattCharacteristic.PERMISSION_WRITE);


        service.addCharacteristic(supNewAlertCategory);
        service.addCharacteristic(newAlert);
        service.addCharacteristic(supUnreadAlertCategory);
        service.addCharacteristic(unreadAlertStatus);
        service.addCharacteristic(alertNotificationControlPoint);

        return service;
    }
}