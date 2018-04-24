#ifndef HARDWARE_SELECTION_H
#define HARDWARE_SELECTION_H

// Modules:
// 0 - Voltage
// 1 - Current
// 2 - Resistance
// 3 - Frequency
// 4 - Capacitance

// Ranges:
// Voltage:
// 0 - 10m  - 000
// 1 - 100m - 001
// 2 - 1    - 010
// 3 - 10   - 011
// Current:
// Idk yet
// Resistance:
// 0 - 1k   - 000
// 1 - 5k   - 001
// 2 - 10k  - 001
// 3 - 50k  - 010
// 4 - 100k - 010
// 5 - 500k - 011
// 6 - 1M   - 011
// Frequency:
// idk if this even needs to be here
// Capacitace:
// idk yet



// Set a range manually
// Sets digital pins 2, 3 and 4 on J7 to output the desired range, in 3 bit encoding.
// Pin 2 is the MSB and pin 4 is the LSB
void setRange(int module, int range);
// Set the hardware module you wnat to select.
// Using J5 pins 3-5, 3 MSB 5 LSB
void setModule(int module);

// Automatically chose a suitable range for the user
int autoRange(int range);

#endif
