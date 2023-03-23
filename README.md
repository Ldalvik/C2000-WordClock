# TI-F2806x-WordClock
A replication of my "RGB-WordClock" on a TI-C2000 F28069M, written in bare metal C.

## Software
Since there is no official HAL for this device, this project required deep digging into data sheets. I threw together a small library that does basic things like starting/stopping interrupts, interfacing the main loop to be more arduino-like, and all the other IO related things I would need like digital in/out and muxing. The most important part was setting the SPI peripheral to work with a real-time clock, specifically the DS1302. After reading the datasheet, I was able to get the correct settings (baud-rate, polarity/phase, etc) and communicate back and forth with the RTC. I created a full library for it to work with this F2806x as well.

# Hardware

For the PCB, I used KiCAD and made a shield for the F28069. The LEDs I decided on were extremely small (about a millimeter), and a microscope was needed to place them correctly. Other components consist of the DS1302, resistors for each LED, capacitor for filtering, 6pF 32.768khz crystal, and a small coin cell battery for a few days of time keeping while unplugged.
