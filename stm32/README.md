# STM32 CAN Loopback Wire Monitor

## Hardware
- MCU: STM32F103C8T6 Blue Pill
- Debugger: ST-Link V2
- Status LED: PC13
- Wire-sense input: PB0
- CAN RX: PA11
- CAN TX: PA12

## Functionality
- Sends an extended CAN frame with ID `0x1800EEF0` and data `0`
  every second.
- Detects wire disconnection through PB0 EXTI.
- Immediately sends data `1` when the wire disconnects.
- Uses CAN loopback mode.
- Accepts only extended data frames with ID `0x1800EEF0`.
- Data `0` turns the LED on.
- Data `1` toggles the LED.

## Wire connection
Connect PB0 to 3.3 V using a removable jumper wire. PB0 is configured
with a pull-down resistor and a falling-edge EXTI interrupt.
