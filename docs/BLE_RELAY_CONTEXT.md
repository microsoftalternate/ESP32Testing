# BLE Relay Context

## Purpose
- ESP32 BLE UART relay test target for the JadePoint Android app.
- Current job: advertise, bond securely, accept UART-style BLE writes, echo notifications, and survive disconnect/reconnect cycles without requiring power reset.

## BLE Contract
- Device name: `ESP32-BLE-ECHO`
- Service UUID: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- RX characteristic: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E`
- TX characteristic: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E`

## Security Contract
- Bonding required for encrypted writes.
- On auth failure, stale bonds may be cleared and advertising restarted.

## Important Debugging Finding
- A prior reconnect bug was most likely caused by restarting BLE advertising directly inside disconnect/auth callbacks.
- Power cycling fixed the issue because it cleared ESP32 BLE runtime/controller state, not because it cleared the Android bond.
- Current firmware defers sensitive BLE restart work out of callbacks and into `loop()`.

## Current Test Objective
- Android app can connect, disconnect, reconnect, and send payloads in the same ESP32 power cycle.
