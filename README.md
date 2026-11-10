# BLE Soil Moisture Monitor
A simple BLE project to transmit soil moisture data to a companion app.

## Problem Statement

Every time I try to take care of a plant, I find myself unsure of when to water it. Not watering it enough leads to the plant drying up and withering, while watering it too much prevents oxygen from reaching its roots, suffocating it. To fix this problem, I have designed a BLE soil moisture monitor that allows me to check the current moisture as a percentage from my phone.

## Demo

![Assembled PCB](./images/pcbaphysical.png)

This is the final assembled PCB.

![Device in plant](./images/devicefinal.png)

This shows the device embedded in a plant's soil.

![Scanner](./images/scanner.png)

The scanner is part of the nRF Connect app, which is used to connect to the device via Bluetooth. From this app, the GATT services and characteristics can be observed.

![GATT service](./images/gattservice.png)

Here, the GATT service is demonstrated. From the app, the user can check whether they have notifications enabled, and see the current soil moisture reading as a percentage (48% in this example). A moisture around 50% is optimal, since 0% indicates that the plant is underwatered and 100% indicates that the soil is saturated.

## Components

The design consists of 3 components: a XIAO nRF52840 microcontroller, a Soldered capacitive soil moisture sensor, and an Adafruit coin cell battery holder. 

[Image of product]
[Product link]
The XIAO microcontroller was selected because of its built-in BLE capabilities, ultra low power consumption, and cheap price. 

[Image of product]
[Product link]
The capacitive soil moisture sensor was chosen because the resistive sensors are prone to corrosion and become inaccurate over time whereas the capacitive sensors do not. 

[Image of product]
[Product link]
The coin cell battery holder was necessary to supply the device with power, enabling remote operation. A coin cell battery is the ideal choice because of their small form factor and impressive battery capacity given their size.

## Implementation

[Hardware Schematic image]
[PCB Layout image]
[Firmware Architecture image (state machine)]

## Challenges Faced

This project required me to learn many new technologies in order to assemble everything. One of the biggest roadblocks I faced was settling on a design when I had little experience working with BLE or ULP devices. Ultimately, I opted to keep the hardware simple and focus more on gaining depth in my software knowledge. This made it easier to test each component individually, allowing me to minimize potential points of failure while designing the firmware for the device. 

[Flow chart of testing stages]

In fact, the firmware could be designed in sections to work with each component before combining everything together. Specifically, I loaded a sample blink program onto the microcontroller, modified a sample BLE button notification program to work with my hardware, modified a USB serial monitor example to verify the soil moisture sensor readings, and tested powering the device using the battery rather than the USB cable. Isolating components and gradually building up towards the completed final product made the task of learning many new technologies more manageable.

## Battery Estimate

[Full battery estimate equation]
[Sources for each value]

## Future Work
In the future, I would further improve the power management with the addition of two main features. The first would be an on/off switch for the coin cell battery power supply. Some components come with this pre-installed, and this would have been an ideal choice for my use case.

The second improvement I would make would be adding a MOSFET transistor between the power supply and the moisture sensor so that the sensor can only pull current when digitally enabled. This would significantly improve the battery life because the current drawn by the sensor is orders of magnitudes larger than that of the microcontroller while the microcontroller is in low-power mode.
