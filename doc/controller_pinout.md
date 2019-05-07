# Controller pinout of "witty cloud" board

## Schematic
![Schematic](witty_cloud_schematic.png)

## Pinout

| GPIO | Pin | build-in function  | using for          |
| ----:|-----|--------------------|--------------------|
|   16 | D0  | -                  | I<sup>2</sup>C SDA |
|   14 | D5  | -                  | I<sup>2</sup>C SCL |
|    4 | D2  | Push Button        | Button Pump        |
|    0 | D3  | Flash-Button       | Button Valve       |
|   15 | D8  | RGB-LED red        | -                  |
|   12 | D6  | RGB-LED green      | Output Valve       |
|   13 | D7  | RGB-LED blue       | Output Pump        |
|    2 | D4  | Blue LED on module | -                  |