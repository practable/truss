## Mini Truss PCB design

# Current state

An Arduino Nano 33 IoT is used as the microcontroller for the lab. Firmware, in an unfinished state, is available [here](https://github.com/dpreid/truss-firmware/tree/main/MiniTruss). The latest mini-truss firmware is available on the develop branch.

The PCB design currently consists of the analogue-to-digital converters (ADC) for strain gauge measurements and the Arduino Nano 33 IoT. The ADC uses the [HX711](https://github.com/dpreid/mini-truss-pcb/blob/main/docs/hx711_english.pdf)

Firmware and PCB updates for the new [linear actuator](https://github.com/dpreid/mini-truss-pcb/blob/main/docs/Actuonix%2BL16%2BDatasheet.pdf) have been incorporated into the latest version.

# TODO

- Check correct resistance values are being used for voltage divider within ADC unit (R1 and R2 and then equivalents).
- TBC, whether we will stick with the Arduino Nano 33 IoT or convert to using Tim Drysdale's custom [microcontroller board](https://github.com/timdrysdale/atmega-demo). Perhaps better to use Arduino Nano (ie not 33 IoT) or Uno in order to have 5V pins and not require a logic level shifter.
- testing with prototype mini truss once strain gauges are ordered.
