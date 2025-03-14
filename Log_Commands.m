# Hardware testing doc

If logcat viewer isn't helping, run these commands in Android Studio terminal

## Make sure

1. Uninstall / install app (use latest code version)
2. Enable developer options and USB debugging on the phone
3. Run the app and connect to ESP32

## To see data being read/written:

```bash
adb logcat | grep -i "onCharacteristic"
```

## BLE connection check commands:

```bash
# See all Bluetooth-related logs
adb logcat | grep -i bluetooth

# Only show logs from our app
adb logcat | grep -i "com.team12.smarthat"

# BLE operations only
adb logcat | grep -E "BluetoothGatt|BLE|GATT"
```

## See data being processed by app:

```bash
adb logcat | grep -i "parse"
adb logcat | grep -i "data"
```

## Diagnosing connection problems:

```bash
# Check current status
adb shell dumpsys bluetooth_manager

# Check signal strength
adb logcat | grep -i "rssi"

# Reset the Bluetooth system
adb shell service call bluetooth_manager 8
# Wait a few seconds, then
adb shell service call bluetooth_manager 6
```

## Working with the app:

```bash
# Close the app completely
adb shell am force-stop com.team12.smarthat

# Start the app
adb shell monkey -p com.team12.smarthat 1
```

## Saving test results:

If you find an interesting issue and want to share it later, use this:

```bash
# Save a full test result to a file
adb logcat -v time > smarthat_test_session.log
```
```

