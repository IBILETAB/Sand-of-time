# Project by Pinout Crew for Ala Praxis
# Sand-of-time
# Weight-Based Audio Player

This project uses a **load cell with HX711 amplifier** and a **DFPlayer Mini MP3 module** to create an audio system where the playback volume is controlled by the weight measured on the load cell. The audio loops continuously while the weight is applied.

---

## Components & Connections

### 1. Arduino Nano
- Main microcontroller.

### 2. HX711 (Load Cell Amplifier)
- **VCC → 5V (Arduino)**
- **GND → GND (Arduino)**
- **DT → A1 (Arduino analog pin)**
- **SCK → A0 (Arduino analog pin)**
- **Load Cell → HX711 E+ / E- / A+ / A- (follow module pinout)**

### 3. DFPlayer Mini
- **VCC → 5V (Arduino)**
- **GND → GND (Arduino)**
- **RX → D10 (Arduino)**
- **TX → D11 (Arduino)**
- **SPK_1 / SPK_2 → Speaker (8Ω recommended)**

### 4. Power
- A stable 5V source (USB or regulated supply).
- If using an external supply, make sure Arduino and modules share the **same ground**.

---

## Libraries Used

1. **HX711 Arduino Library**  
   [HX711 by Bogde](https://github.com/bogde/HX711)  
   For reading weight from the load cell via HX711.

2. **DFRobotDFPlayerMini**  
   [DFRobotDFPlayerMini Library](https://github.com/DFRobot/DFRobotDFPlayerMini)  
   For controlling the DFPlayer Mini (play, loop, volume, etc.).

3. **Software Serial Library**
      [https://github.com/PaulStoffregen/SoftwareSerial]
---

## How It Works
1. The load cell measures weight through the HX711.  
2. Arduino reads the weight value.  
3. DFPlayer Mini plays an audio file in a loop.  
4. The playback volume is adjusted dynamically based on the weight detected.  

---

## Troubleshooting

- **No sound?**
  - Check speaker connection (SPK1 & SPK2).  
  - Make sure your audio file is in the `mp3` or `01` folder on the SD card.  
  - File should be named like `0001.mp3`, `0002.mp3`, etc.  

- **Wrong weight reading?**
  - Verify wiring between load cell and HX711.  
  - Use correct HX711 pins (DT → A1, SCK → A0).  
  - Try calibrating by placing a known weight.  

- **Distorted sound?**
  - Use an 8Ω speaker, not a buzzer.  
  - Ensure stable 5V power supply.  

- **DFPlayer not detected?**
  - Check if the SD card is FAT32 formatted.  
  - Ensure RX/TX pins are correctly wired (crossed: Arduino D10 → DFPlayer TX, Arduino D11 → DFPlayer RX).  
  - Add a 1K resistor in series with the DFPlayer RX pin if it’s unstable.

---

## Future Improvements
- Add an LCD display to show weight.  
- Allow multiple audio tracks depending on weight ranges.  
- Implement calibration mode for more accurate measurements.
