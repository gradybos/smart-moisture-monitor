# BLE Soil Moisture Monitor
A simple BLE project to transmit soil moisture data to a companion app.

## Problem Statement

Every time I try to take care of a plant, I find myself unsure of when to water it. Not watering it enough leads to the plant drying up and withering, while watering it too much prevents oxygen from reaching its roots, suffocating it. To fix this problem, I have designed a BLE soil moisture monitor that allows me to check the current moisture as a percentage from my phone.

## Demo

## Components

## Implementation

## Challenges Faced

## Battery Estimate

## Future Work
In the future, I would further improve the power management with the addition of two main features. The first would be an on/off switch for the coin cell battery power supply. Some components come with this pre-installed, and this would have been an ideal choice for my usecase.

The second improvement I would make would be adding a MOSFET transistor between the power supply and the moisture sensor so that the sensor can only pull current when digitally enabled. This would significantly improve the battery life because the current drawn by the sensor is orders of magnitudes larger than that of the microcontroller while the microcontroller is in low-power mode.