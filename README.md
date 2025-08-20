# Capstone Project Docs
A repository for my capstone project code and miscellaneous files

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

