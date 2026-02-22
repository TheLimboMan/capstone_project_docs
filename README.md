# Docs For IoT-Based Soil and Air Quality Detection System for Maize Plantation Site Survey

This is a repository for our capstone project code and miscellaneous files

### About the project:
**Title:** 
IoT-Based Soil and Air Quality Detection System for Maize Plantation Site Survey

**Description:** 
This project aims to develop a portable solution that can enable farmers to obtain soil and air quality data to get a first opinion on land usability for maize cultivation. This project attemps to streamline the process of obtaining data and provide a real-time, approachable user interface to gauge the land's usability, allowing the reduction of immediate reliance on time-consuming and labor-intensive laboratory testing. This fast, accessible approach empowers farmers to make data-driven decisions directly on-site, significantly improving operational efficiency and resource allocation before committing to formal lab analysis.

**System Architecture:**
<img width="4345" height="1843" alt="TheArchitecture" src="https://github.com/user-attachments/assets/a5adab81-58e4-43e3-b6e4-9e80407016a2" />

**Features:**
- Uses the ESP32 for it's powerful processing power and wireless connectivity
- On Board LED Indicator for Status Indication
- Sensors:

    - NPK Sensor
    - Air Quality Sensor
    - Soil Moisture Sensor
  
- SD Card For Offline Data Storage
- LCD Display To Display Captured Data
- Ability To Transfer Data To A Server Through HTTP

# Wiring:
<img width="2218" height="2020" alt="Wiring Diagram" src="https://github.com/user-attachments/assets/304eac7f-bed8-4687-9804-462f5b552a20" />

### Modbus RTU requests for reading NPK values:
```cpp
//For our case, our NPK sensor's address is 0x05, so our code has 0x05 as our first bit sent,
//in accordance with our NPK sensor's manufacturer's instructions
const byte nitro[] = {0x05, 0x03, 0x00, 0x03, 0x00, 0x06, 0x34, 0x4C};
const byte phos[] = {0x05, 0x03, 0x00, 0x04, 0x00, 0x06, 0x85, 0x8D}; 
const byte pota[] = {0x05, 0x03, 0x00, 0x05, 0x00, 0x06, 0xD4, 0x4D}; 
```

# Capstone Project Members
<table>
  <tr>
    <td align="center">
      <a href="https://github.com/TheLimboMan">
        <img src="https://github.com/TheLimboMan.png?size=60" width="60" style="border-radius:50%"/><br/>
        @TheLimboMan
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/JJLee0219">
        <img src="https://github.com/JJLee0219.png?size=60" width="60" style="border-radius:50%"/><br/>
        @JJLee0219
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/NgSZ401">
        <img src="https://github.com/NgSZ401.png?size=60" width="60" style="border-radius:50%"/><br/>
        @NgSZ401
      </a>
    </td>
  </tr>
</table>

