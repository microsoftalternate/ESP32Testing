# Hardware Specification

## Purpose
This document is the source of truth for hardware used in the BLE relay test flow.

## Test Scope (Current)
- Android app sends BLE payload to ESP32.
- ESP32 prints payload to USB serial monitor at `115200` baud.
- ESP32 acknowledges each newline-terminated BLE command back to the phone on TX notify.
- Android app stores inbound BLE messages in a `Messages` tab and can raise phone notifications.

## Primary Board
- MCU: ESP32-WROOM development board (first target)
- USB-UART: board-integrated bridge (exact chip model to confirm during bring-up)

## BLE Contract
- Device Name: `ESP32-BLE-ECHO`
- Service UUID: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- RX (App -> ESP32, write): `6E400002-B5A3-F393-E0A9-E50E24DCCA9E`
- TX (ESP32 -> App, notify/read): `6E400003-B5A3-F393-E0A9-E50E24DCCA9E`

## Serial Contract
- Interface: USB serial monitor
- Baud: `115200`
- Expected line format:
  - `BLE RX -> <payload>`
  - `BLE TX -> ACK:<payload>`

## Wiring / Power
- For BLE echo test: no external module wiring required.
- Power from USB only.

## Pass / Fail Criteria
- Pass: Text entered in app appears exactly in ESP32 serial output.
- Pass: Phone receives `ACK:<payload>` in the app message inbox after each command.
- Fail: No BLE connection, no write success, or serial output mismatch.

## Next Phase (LoRa Relay)
Planned hardware to add after BLE test validation:
- Wio-E5 module connected to ESP32 UART.
- ESP32 firmware converts app command payloads to Wio-E5 AT commands and returns responses.
