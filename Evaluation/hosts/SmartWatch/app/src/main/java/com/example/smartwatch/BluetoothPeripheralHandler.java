package com.example.smartwatch;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.os.Handler;
import android.util.Log;

import com.welie.blessed.BluetoothCentral;
import com.welie.blessed.BluetoothCentralManagerCallback;
import com.welie.blessed.BluetoothPeripheralManager;
import com.welie.blessed.BluetoothPeripheralManagerCallback;
import com.welie.blessed.GattStatus;

import org.jetbrains.annotations.NotNull;

import javax.mail.*;
import javax.mail.search.FlagTerm;
import java.util.*;

import timber.log.Timber;

import static android.bluetooth.BluetoothGattCharacteristic.FORMAT_UINT8;
import static android.bluetooth.BluetoothGattCharacteristic.PERMISSION_READ;
import static android.bluetooth.BluetoothGattCharacteristic.PERMISSION_WRITE;
import static android.bluetooth.BluetoothGattCharacteristic.PROPERTY_INDICATE;
import static android.bluetooth.BluetoothGattCharacteristic.PROPERTY_READ;
import static android.bluetooth.BluetoothGattService.SERVICE_TYPE_PRIMARY;
import static com.example.smartwatch.AlertProfile.COMMAND_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION;
import static com.example.smartwatch.AlertProfile.COMMAND_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION;
import static com.example.smartwatch.AlertProfile.COMMAND_ENABLE_INCOMING_ALERT_NOTIFICATION;
import static com.example.smartwatch.AlertProfile.COMMAND_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION;
import static com.example.smartwatch.AlertProfile.COMMAND_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY;
import static com.example.smartwatch.AlertProfile.COMMAND_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY;
import static com.example.smartwatch.AlertProfile.SUP_NEW_ALERT_CATEGORY;
import static com.example.smartwatch.AlertProfile.SUP_UNREAD_ALERT_CATEGORY;
import static com.example.smartwatch.AlertProfile.UNREAD_ALERT_STATUS;

public class BluetoothPeripheralHandler {
    private static BluetoothPeripheralHandler instance = null;
    private final Context context;
    private final Handler handler = new Handler();
    public BluetoothPeripheralManager peripheralManager;

    /* Current Time Service UUID */
    public static UUID TIME_SERVICE = UUID.fromString("00001805-0000-1000-8000-00805f9b34fb");
    /* Mandatory Current Time Information Characteristic */
    public static UUID CURRENT_TIME    = UUID.fromString("00002a2b-0000-1000-8000-00805f9b34fb");
    /* Optional Local Time Information Characteristic */
    public static UUID LOCAL_TIME_INFO = UUID.fromString("00002a0f-0000-1000-8000-00805f9b34fb");
    /* Mandatory Client Characteristic Config Descriptor */
    public static UUID CLIENT_CONFIG = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    private BluetoothGattService timeService;
    private BluetoothGattService alertService;
    public boolean manualUpdate = false;

    public BluetoothPeripheralHandler(Context context){
        this.context = context;

        // Plant a tree
        Timber.plant(new Timber.DebugTree());

        BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);

        peripheralManager = new BluetoothPeripheralManager(context, bluetoothManager, peripheralManagerCallback);

        addTimeService();
        addAlertService();
        setupAlertService();
    }

    public static synchronized BluetoothPeripheralHandler getInstance(Context context) {
        if (instance == null) {
            instance = new BluetoothPeripheralHandler(context.getApplicationContext());
        }
        return instance;
    }

    public void addTimeService(){
        timeService = TimeProfile.createTimeService();
        peripheralManager.add(timeService);
        Timber.i("Time service added");
    }

    public void addAlertService(){
        alertService = AlertProfile.createAlertService();
        peripheralManager.add(alertService);
        Timber.i("Alert service added");
    }

    public void setupAlertService(){
        BluetoothGattCharacteristic alert = alertService.getCharacteristic(SUP_NEW_ALERT_CATEGORY);
        alert.setValue(0x01, FORMAT_UINT8, 0);

        BluetoothGattCharacteristic unread = alertService.getCharacteristic(SUP_UNREAD_ALERT_CATEGORY);
        unread.setValue(0x01, FORMAT_UINT8, 0);
    }
    public void alertNewEmails(byte numberofEmails){
        if(emailAlertsEnabled){
            BluetoothGattCharacteristic characteristic = alertService.getCharacteristic(UNREAD_ALERT_STATUS);
            byte byteArray[] = {0x01, numberofEmails}; // 0x01 lists a emails alert
            characteristic.setValue(byteArray);
            peripheralManager.notifyCharacteristicChanged(byteArray, characteristic);
        }
    }

    boolean emailAlertsEnabled = false;
    byte numberOfUnreadEmails = 0;

    public void processAlertControlWrite(byte id){
        switch (id){
            case COMMAND_ENABLE_INCOMING_ALERT_NOTIFICATION:
                break;
            case COMMAND_ENABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION:
                emailAlertsEnabled = true;
                break;
            case COMMAND_DISABLE_NEW_INCOMING_ALERT_NOTIFICATION:
                break;
            case COMMAND_DISABLE_UNREAD_CATEGORY_STATUS_NOTIFICATION:
                emailAlertsEnabled = false;
                break;
            case COMMAND_NOTIFY_NEW_INCOMING_ALERT_IMMEDIATELY:
                break;
            case COMMAND_NOTIFY_UNREAD_CATEGORY_STATUS_IMMEDIATELY:
                alertNewEmails(numberOfUnreadEmails);
                break;
            default:
                break;
        }

    }

    public void updateTime(long timestamp, byte adjustReason){
            BluetoothGattCharacteristic characteristic = timeService.getCharacteristic(CURRENT_TIME);
            characteristic.setValue(TimeProfile.getExactTime(timestamp, adjustReason));
            peripheralManager.notifyCharacteristicChanged(characteristic.getValue(), characteristic);
    }

    private final BluetoothPeripheralManagerCallback peripheralManagerCallback = new BluetoothPeripheralManagerCallback() {
        @Override
        public void onNotifyingEnabled(@NotNull BluetoothCentral central, @NotNull BluetoothGattCharacteristic characteristic) {
            if (characteristic.getUuid().equals(CURRENT_TIME)) {
//                peripheralManager.notifyCharacteristicChanged(characteristic.getValue(), characteristic);
                manualUpdate = true;
            }
        }

        @Override
        public void onNotifyingDisabled(@NotNull BluetoothCentral central, @NotNull BluetoothGattCharacteristic characteristic) {
            if (characteristic.getUuid().equals(CURRENT_TIME)) {
//                stopNotifying();
            }
        }

        @Override
        public void onCharacteristicRead(@NotNull BluetoothCentral bluetoothCentral, @NotNull BluetoothGattCharacteristic characteristic) {
            long now = System.currentTimeMillis();
            if (TimeProfile.CURRENT_TIME.equals(characteristic.getUuid())) {
                Timber.i("Read CurrentTime");
                characteristic.setValue(TimeProfile.getExactTime(now, TimeProfile.ADJUST_NONE));
            } else if (TimeProfile.LOCAL_TIME_INFO.equals(characteristic.getUuid())) {
                Timber.i("Read LocalTimeInfo");
                characteristic.setValue(TimeProfile.getLocalTimeInfo(now));
            }
        }

        @Override
        public GattStatus onCharacteristicWrite(@NotNull BluetoothCentral bluetoothCentral, @NotNull BluetoothGattCharacteristic characteristic, @NotNull byte[] value) {
            if(AlertProfile.ALERT_NOTIFICATION_CONTROL_POINT.equals(characteristic.getUuid())){
                processAlertControlWrite(value[0]); // here we assume the category in the second byte is always correct
            }
            return super.onCharacteristicWrite(bluetoothCentral, characteristic, value);
        }
    };
}
