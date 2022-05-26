package com.example.smartwatch;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.Manifest;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.StrictMode;
import android.text.format.DateFormat;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Locale;
import java.util.Properties;

import timber.log.Timber;
import com.welie.blessed.BluetoothCentralManager;
import com.welie.blessed.BluetoothPeripheral;

import javax.mail.Flags;
import javax.mail.Folder;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Store;
import javax.mail.search.FlagTerm;


// some of the functions from https://github.com/weliem/blessed-android examples/tutorials.
public class MainActivity extends AppCompatActivity {
    private static final int REQUEST_ENABLE_BT = 1;
    private static final int ACCESS_LOCATION_REQUEST = 2;

    private TextView timeTextBox;
    private EditText numberOfEmailsBox;
    private Button setNumberOfEmailsBtn;
    private Button getNumberofEmailsBtn;
    private Button setTimeManualBtn;

    private static final String CSV_LOGDIR = "/Documents/ble-logger/";
    private static final String CSV_PREFIX = "logger-";
    private static final String CSV_EXTENSION = ".csv";

    private File csvFile;

    private FileWriter fw;
    private BufferedWriter bw;
    private PrintWriter outCsv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        timeTextBox = (TextView) findViewById(R.id.time_text);
        numberOfEmailsBox = (EditText) findViewById(R.id.textboxemails);
        numberOfEmailsBox.setText("0");
        setNumberOfEmailsBtn = (Button) findViewById(R.id.setUnreadEmails);
        setTimeManualBtn = (Button) findViewById(R.id.pushTimeBtn);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        createFile();
        try {
            fw = new FileWriter(csvFile.getAbsoluteFile(), true);
            bw = new BufferedWriter(fw);
            outCsv = new PrintWriter(bw);
        } catch (IOException e){
            e.printStackTrace();
        }

        setNumberOfEmailsBtn.setOnClickListener( new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                String value = numberOfEmailsBox.getText().toString();
                Integer valueInt = Integer.parseInt(value);
                BluetoothPeripheralHandler.getInstance(getApplicationContext()).alertNewEmails(valueInt.byteValue());
            }
        });

        setTimeManualBtn.setOnClickListener( new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                long now = System.currentTimeMillis();
                BluetoothPeripheralHandler.getInstance(getApplicationContext()).updateTime(now,TimeProfile.ADJUST_MANUAL);
            }
        });

        getNumberofEmailsBtn = (Button) findViewById(R.id.getemailsbutton);
        getNumberofEmailsBtn.setOnClickListener( new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                try {
                    numberOfEmailsBox.setText(getNumberOfUnreadEmails().toString());
                } catch (MessagingException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        outCsv.close();
    }

    private void createFile() {
        try {
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd_HH-mm-ss-SSS", Locale.getDefault());
            String currentDateandTime = sdf.format(new Date());

            File directory = new File(Environment.getExternalStorageDirectory() + CSV_LOGDIR);
            if (!directory.exists()) {
                directory.mkdirs();
            }

            csvFile = new File(Environment.getExternalStorageDirectory() + CSV_LOGDIR + CSV_PREFIX + currentDateandTime + CSV_EXTENSION);

            // if file doesnt exists, then create it
            if (!csvFile.exists()) {
                csvFile.createNewFile();
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }


// from https://github.com/androidthings/sample-bluetooth-le-gattserver
    @Override
    protected void onStart() {
        super.onStart();

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_TIME_TICK);
        filter.addAction(Intent.ACTION_TIME_CHANGED);
        filter.addAction(Intent.ACTION_TIMEZONE_CHANGED);
        registerReceiver(mTimeReceiver, filter);
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (BluetoothAdapter.getDefaultAdapter() != null) {
            if (!isBluetoothEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            } else {
                checkPermissions();
            }
        } else {
            Timber.e("This device has no Bluetooth hardware");
        }
    }

    /**
     * Update graphical UI on devices that support it with the current time.
     */
    private void updateLocalUi(long timestamp) {
        Date date = new Date(timestamp);
        String displayDate = DateFormat.getMediumDateFormat(this).format(date)
                + "\n"
                + DateFormat.getTimeFormat(this).format(date);
        timeTextBox.setText(displayDate);
    }

    public Integer getNumberOfUnreadEmails() throws MessagingException {
        if (android.os.Build.VERSION.SDK_INT > 9) {
            StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
            StrictMode.setThreadPolicy(policy);
        }

        // taken from https://stackoverflow.com/a/28689722
        Properties props = new Properties( );
        props.setProperty("mail.imap.starttls.enable", "true");
        props.setProperty("mail.store.protocol", "imaps");
        Session session = Session.getDefaultInstance(props);
        Store store = session.getStore();
        store.connect("INSERTMAILSERVERHERE", 993, "INSERTEMAILLOGINHERE", "EMAILLOGINPASSWORD");
        Folder inbox = store.getFolder( "INBOX" );
        inbox.open( Folder.READ_ONLY );

        // Fetch unseen messages from inbox folder
        Message[] messages = inbox.search(
                new FlagTerm(new Flags(Flags.Flag.SEEN), false));

        return messages.length;
        // Sort messages from recent to oldest
//        Arrays.sort( messages, ( m1, m2 ) -> {
//            try {
//                return m2.getSentDate().compareTo( m1.getSentDate() );
//            } catch ( MessagingException e ) {
//                throw new RuntimeException( e );
//            }
//        } );
//
//        for ( Message message : messages ) {
//            System.out.println(
//                    "sendDate: " + message.getSentDate()
//                            + " subject:" + message.getSubject() );
//        }
//    }
    }
// from https://github.com/androidthings/sample-bluetooth-le-gattserver
    /**
     * Listens for system time changes and triggers a notification to
     * Bluetooth subscribers.
     */
    private BroadcastReceiver mTimeReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            byte adjustReason;
            switch (intent.getAction()) {
                case Intent.ACTION_TIME_CHANGED:
                    adjustReason = TimeProfile.ADJUST_MANUAL;
                    break;
                case Intent.ACTION_TIMEZONE_CHANGED:
                    adjustReason = TimeProfile.ADJUST_TIMEZONE;
                    break;
                default:
                case Intent.ACTION_TIME_TICK:
                    adjustReason = TimeProfile.ADJUST_NONE;
                    break;
            }
            long now = System.currentTimeMillis();
            // Hard coded manual update for now. Triggers the notification once then disables after connection
            // Simulates user updating time.
            BluetoothPeripheralHandler peripheralHandler = BluetoothPeripheralHandler.getInstance(getApplicationContext());
            if(peripheralHandler.manualUpdate){
                BluetoothPeripheralHandler.getInstance(getApplicationContext()).updateTime(now, TimeProfile.ADJUST_MANUAL);
                peripheralHandler.manualUpdate = false;
            }
            updateLocalUi(now);
        }
    };

    private void initBluetoothHandler()
    {
        BluetoothHandler instance = BluetoothHandler.getInstance(getApplicationContext());
        instance.setCsv(outCsv);
        BluetoothPeripheralHandler.getInstance(getApplicationContext());
    }

    private BluetoothPeripheral getPeripheral(String peripheralAddress) {
        BluetoothCentralManager central = BluetoothHandler.getInstance(getApplicationContext()).central;
        return central.getPeripheral(peripheralAddress);
    }

    private boolean isBluetoothEnabled() {
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(bluetoothAdapter == null) return false;

        return bluetoothAdapter.isEnabled();
    }

    private void checkPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            String[] missingPermissions = getMissingPermissions(getRequiredPermissions());
            if (missingPermissions.length > 0) {
                requestPermissions(missingPermissions, ACCESS_LOCATION_REQUEST);
            } else {
                permissionsGranted();
            }
        }
    }

    private String[] getMissingPermissions(String[] requiredPermissions) {
        List<String> missingPermissions = new ArrayList<>();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            for (String requiredPermission : requiredPermissions) {
                if (getApplicationContext().checkSelfPermission(requiredPermission) != PackageManager.PERMISSION_GRANTED) {
                    missingPermissions.add(requiredPermission);
                }
            }
        }
        return missingPermissions.toArray(new String[0]);
    }

    private String[] getRequiredPermissions() {
        int targetSdkVersion = getApplicationInfo().targetSdkVersion;
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q && targetSdkVersion >= Build.VERSION_CODES.Q)
            return new String[] {Manifest.permission.ACCESS_FINE_LOCATION};
        else return new String[] {Manifest.permission.ACCESS_COARSE_LOCATION};
    }

    private void permissionsGranted() {
        // Check if Location services are on because they are required to make scanning work
        if (checkLocationServices()) {
            initBluetoothHandler();
        }
    }

    private boolean areLocationServicesEnabled() {
        LocationManager locationManager = (LocationManager) getApplicationContext().getSystemService(Context.LOCATION_SERVICE);
        if (locationManager == null) {
            Timber.e("could not get location manager");
            return false;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            return locationManager.isLocationEnabled();
        } else {
            boolean isGpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
            boolean isNetworkEnabled = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);

            return isGpsEnabled || isNetworkEnabled;
        }
    }

    private boolean checkLocationServices() {
        if (!areLocationServicesEnabled()) {
            new AlertDialog.Builder(MainActivity.this)
                    .setTitle("Location services are not enabled")
                    .setMessage("Scanning for Bluetooth peripherals requires locations services to be enabled.") // Want to enable?
                    .setPositiveButton("Enable", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.cancel();
                            startActivity(new Intent(android.provider.Settings.ACTION_LOCATION_SOURCE_SETTINGS));
                        }
                    })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            // if this button is clicked, just close
                            // the dialog box and do nothing
                            dialog.cancel();
                        }
                    })
                    .create()
                    .show();
            return false;
        } else {
            return true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        // Check if all permission were granted
        boolean allGranted = true;
        for (int result : grantResults) {
            if (result != PackageManager.PERMISSION_GRANTED) {
                allGranted = false;
                break;
            }
        }

        if (allGranted) {
            permissionsGranted();
        } else {
            new AlertDialog.Builder(MainActivity.this)
                    .setTitle("Location permission is required for scanning Bluetooth peripherals")
                    .setMessage("Please grant permissions")
                    .setPositiveButton("Retry", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogInterface, int i) {
                            dialogInterface.cancel();
                            checkPermissions();
                        }
                    })
                    .create()
                    .show();
        }
    }
}
