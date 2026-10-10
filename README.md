# Arduino UNO-Based Smart Voltage Protector

## Project Overview
This project demonstrates a microcontroller-based voltage and overcurrent protection system using an Arduino UNO and Wokwi simulation.

The system continuously monitors simulated voltage and current inputs. When either value exceeds its configured threshold, the controller disconnects the protected load through a relay and activates a fault indicator.

## Features
- Overvoltage protection
- Overcurrent protection
- Automatic load disconnection
- Fault latching and manual reset
- LED-based operating status indication
- Computer communication through Serial Monitor
- Adjustable voltage and current protection limits
- Automatic status reporting with pause/resume functionality

## Components
- Arduino UNO (ATmega328P)
- Two potentiometers for simulated voltage and current inputs
- Relay module
- Green LED for normal operation
- Red LED for fault indication
- Yellow LED representing the protected appliance
- Push button for manual reset
- Current-limiting resistors

## Protection Settings

| Parameter | Default Value |
|---|---|
| Maximum Voltage | 250 V |
| Maximum Current | 10 A |
| Simulated Voltage Range | 0–300 V |
| Simulated Current Range | 0–15 A |
| Monitoring Interval | 50 ms |

## Serial Commands

| Command | Description |
|---|---|
| `HELP` | Display available commands and pause automatic status reporting |
| `STATUS` | Display current measurements and relay status |
| `SETV 240` | Set voltage threshold to 240 V |
| `SETI 8` | Set current threshold to 8 A |
| `RESET` | Restore operation when fault conditions are cleared |

## How to Run the Simulation
1. Open the saved Wokwi project using the link below.
2. Start the simulation.
3. Adjust the potentiometers to simulate changing voltage and current.
4. Observe the load LED, status indicators, relay, and Serial Monitor.
5. Increase voltage or current beyond the configured limit to trigger protection.
6. Return both inputs to safe values.
7. Press the manual reset button or send `RESET` through Serial Monitor to reconnect the load.

## Project Structure
- `src/sketch.ino` – Arduino firmware
- `project_files/diagram.json` – Wokwi circuit configuration
- `schematics/` – Circuit screenshot and documentation
- `test_evidence/` – Functional testing screenshots
- `design_document.pdf` – One-page project report

## Wokwi Simulation Link
https://wokwi.com/projects/477483968192091137

## Limitations
This is a simulation-based prototype. The voltage and current values are emulated using potentiometers, not measured from a real power grid. The design has not been validated for physical mains-voltage protection.
