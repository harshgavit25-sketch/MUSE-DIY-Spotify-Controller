# 🎧 MUSE — DIY Spotify Controller

**A custom-built desktop music controller powered by ESP32, Bluetooth, and Python.**

MUSE is a DIY hardware project that brings physical music controls to your desktop. Built around an ESP32 microcontroller, it combines an OLED display, dedicated push buttons, LED indicators, and a Python desktop application to create a compact music-control interface.

The controller displays track information and playback progress while allowing users to interact with music through physical buttons.
<img width="3688" height="2074" alt="muse-final build" src="https://github.com/user-attachments/assets/fc114e04-4cc7-4258-aef6-53cdec6808a2" />



> **Project Status:** Version 1.0 — Completed

## ✨ Features

- 🎵 **Track Information:** Displays the current song title and artist.
- 📟 **OLED Interface:** 128 × 64 monochrome SSD1306 display.
- ⏯️ **Physical Controls:** Previous, play/pause, next, and mute buttons.
- 💡 **LED Indicators:** Visual feedback for selected button interactions.
- 📶 **Bluetooth Communication:** Wireless serial communication between the ESP32 and Windows.
- 🐍 **Python Integration:** Connects the hardware to desktop media-control functionality.
- ⏱️ **Playback Progress:** Displays elapsed time, total duration, and a progress bar.
- 🖥️ **Desktop Launch:** Supports a configured desktop shortcut for starting the application.

## 🛠️ Hardware Components

| Component | Purpose |
|---|---|
| ESP32 DevKit V1 (ESP-WROOM-32) | Main controller |
| SSD1306 128 × 64 OLED | Track information and playback progress |
| 4 push buttons | Music control inputs |
| 3 LEDs | Visual feedback |
| Current-limiting resistors | LED protection |
| Breadboard and jumper wires | Circuit assembly |
| USB cable | Power and firmware upload |

The potentiometer used during development has been removed from the final firmware.

## 💻 Tech Stack

- **Embedded programming:** Arduino C++
- **Microcontroller:** ESP32
- **Desktop integration:** Python
- **Wireless communication:** Bluetooth Serial
- **Display interface:** I²C
- **Display libraries:** Adafruit GFX and Adafruit SSD1306
- **Development platform:** Windows 11

## 🔌 Pin Configuration

### OLED Display

| OLED Pin | ESP32 GPIO |
|---|---|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| VCC | 3.3 V |
| GND | GND |

I²C address: `0x3C`

### Push Buttons

| Button | ESP32 GPIO |
|---|---:|
| Previous | GPIO 26 |
| Play/Pause | GPIO 27 |
| Next | GPIO 14 |
| Mute | GPIO 13 |

The buttons use the ESP32's internal pull-up resistors and are configured to connect their GPIO pins to GND when pressed.

### LEDs

| LED | ESP32 GPIO |
|---|---:|
| Previous | GPIO 16 |
| Play/Pause | GPIO 17 |
| Next | GPIO 18 |

Use suitable series resistors for the LEDs.

## 🧠 How It Works

MUSE consists of two main components.

### 1. ESP32 Controller

The ESP32 firmware:

- Reads button inputs and applies software debouncing.
- Sends button commands through Bluetooth Serial.
- Receives track metadata and playback progress.
- Updates the OLED interface.
- Controls the three indicator LEDs.

### 2. Python Desktop Application

The Python application runs on Windows and communicates with the ESP32 over a Bluetooth serial connection.

Its intended responsibilities are to:

- Send song title, artist, and playback progress to the ESP32.
- Receive button commands from the controller.
- Translate commands into desktop media actions.
- Coordinate with the music application.

### System Architecture

```text
       Spotify / Desktop Media
                 ↕
        Python Desktop Script
                 ↕
        Bluetooth Serial (SPP)
                 ↕
             ESP32 Board
           ↙      ↓      ↘
         OLED   Buttons   LEDs
```

## 📡 Bluetooth Communication Protocol

The ESP32 advertises the Bluetooth device name:

`MUSE-Sharvith`

### Computer → ESP32

| Message | Description |
|---|---|
| `TITLE:<title>` | Updates the song title |
| `ARTIST:<artist>` | Updates the artist name |
| `PROG:<position>,<duration>` | Updates playback progress in milliseconds |

Example:

```text
TITLE:Blinding Lights
ARTIST:The Weeknd
PROG:65000,200000
```

### ESP32 → Computer

| Message | Description |
|---|---|
| `BTN:PREV` | Previous-track command |
| `BTN:PLAY` | Play/pause command |
| `BTN:NEXT` | Next-track command |
| `BTN:MUTE` | Mute command |

The Python application must implement the corresponding command handlers for these actions to work.

## 🚀 Getting Started

### Prerequisites

- Arduino IDE
- ESP32 board support package
- Python 3
- Windows 10/11
