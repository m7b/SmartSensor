# Controller pinout of "witty cloud" board

## Schematic
![Schematic](witty_cloud_schematic.png)

## Pinout

| GPIO | Pin | build-in function  | using for          |
| ----:|-----|--------------------|--------------------|
|   16 | D0  | -                  | I<sup>2</sup>C SDA |
|    5 | D1  | -                  | I<sup>2</sup>C SCL |
|    0 | D3  | Flash-Button       | Button Pump        |
|   14 | D5  | -                  | Button Valve       |
|(-)   |     |                    |                    |
|(-)   |     |                    | Output Pump        |
|(-)   |     |                    | Output Valve       |
|(-)   |     |                    |                    |
|    2 | D4  | Blue LED on module |                    |
|    4 | D2  | Button             |                    |
|(-)   |     |                    |                    |
|   15 | D8  | RGB-LED red        |                    |
|   12 | D6  | RGB-LED green      |                    |
|   13 | D7  | RGB-LED blue       |                    |