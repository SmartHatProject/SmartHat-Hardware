# SmartHat ble config

## Device Info
Device Name: SmartHat
Service UUID: 12345678-1234-5678-1234-56789abcdef0

## Characteristics
1. Sound Sensor:
   - UUID: abcd1234-5678-1234-5678-abcdef123456
   - Properties: READ, NOTIFY
   - Data Format: JSON string
   - {"messageType":"SOUND_SENSOR_DATA","data":45.5,"timeStamp":1234567890}

2. Dust Sensor:
   - UUID: dcba4321-8765-4321-8765-654321fedcba
   - Properties: READ, NOTIFY
   - Data Format: JSON string
   -  {"messageType":"DUST_SENSOR_DATA","data":25.5,"timeStamp":1234567890}

## Data Format
- JSON with 3 fields:
  - messageType: String identifying the sensor ("SOUND_SENSOR_DATA" or "DUST_SENSOR_DATA")
  - data: float value of the sensor reading
  - timeStamp: Long integer Unix timestamp in ms

## Alert Thresholds
- Dust: > 50.0  alert
- Sound: > 85.0  alert

## Implementation Notes
1. ESP32:
   - Provides BLE with service and characteristics sound and dust
   - Updates characteristics 
   - Sends notifications 
   - Handles connect/disconnect 
   - JSON formats same

2. Android:
   - Scans for device named "SmartHat"
   - Connects to service UUID
   - Subscribes to notifications for both characteristics
   - Parses JSON data for display and alerts

## Compatibility HW/APP
- Device name matches
- Service and characteristic UUIDs match
- JSON data format 
- NOTIFY property implemented
- Connection handling 
-Treasholds consiAtent 

