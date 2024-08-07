# Velo Dashboard

A bicycle computer made for velomobiles.

## Reasons

In the summer of 2022 i bought a second hand [Quest](https://www.velomobiel.nl/quest/) velomobile.
It was in very good shape, had only driven 8000km while only 10 years old, and it fits my budget.

But it was missing blinkers, had a 6V system on board, and two headlights which could be turned on independent from eachother. Which results in my Quest looking cross-eyed :-(
That could be done better.

I also struggled finding the 'perfect' bicycle computer to keep track of at least my current speed, average speed, maximum speed, time, time of the trip, distance, and at least two seperate distance counters.
The most anoying things of these computers are the small screens, the not so ideal combination of functions, button combo's etc. And a GPS one took too long to recognise my location (possibly due to the carbon body).

I took some inspiration found online from other velonauts. Such as:
- [VeloPuter](https://veloputer.wordpress.com/about/)
- [Arduino in VM](https://quest.robbroek.nl/search/label/arduino)
- [Velomobiel Led strip verlichting](https://bickyenzijnfietsen.blogspot.com/2018/10/velomobiel-led-strip-verlichting.html)

## Features

My take on a bicycle computer for my velomibile must have at least these features:

Lighting:
- Drive the normal LED lights that are standard in most velombiles (rear light, brake light, fog light and blinkers if present)
- Drive the (double) head light(s) in several steps (off, dim, normal, beam, fog)
- Drive LED strips on each side of the velomobile. For blinkers (with nice animation!), and normal light for visibility (front, rear and brake lights).

Knobs & Display:
- Big OLED display so i can see it easy.
- All controls should be on the steering wheel (with a 3D printed console).
- Knobs for the blinkers, lights and switching through several pages on the display.

Speed & Distance:
- Current speed
- Current average speed
- Current maximum speed
- Distance of current trip
- Total distance of current day
- Distance of previous day
- Average and maximum speed of previous day
- Total distance of the velomobile in total and in my ownership
- Total distance of the velomobile while in my ownership
- At least three different tocal counters wich can be reset independent

Time:
- Current clock time
- Driving time of current trip (excluding stops for traffic lights etc)
- Total time of current trip (including these stops)
- Driving and total time of previous day

Others:
- Battery status
- Battery saving modes
- Automatic turn off after a set time (to spare battery)
- Option to set the total distance
- Option to set the clock time
- Option to set the wheel circumference
- Optionts to reset all extra counters

# Result

I have a working prototype since december 2022, with several improvements and new featuers since then. Still in beta/prototype fase. Results and images will follow in the future...

## Hardware

### Teensy 4.0

The computer is based around a [Teensy 4.0](https://www.pjrc.com/store/teensy40.html). My first attempt was made with a Arduino nano, which is cheaper and uses less power. But i wanted a clock. And the combination of driving LED strips and having an OLED screen was impossible (as far as i know) with te Arduino. The Teensy 4.0 has multiple i2c (OLED) and interrupts. And it also has a build in RTC (Real Time Clock) with power options for a small coin battery to keep the state of the clock. The Teensy has also lots of room for improvements or future additions (SD card storage for example).

### IP67 LEDstrips WS2812B

IP67 means rainproof. And WS2812B means it just has three wires to connect (5V, GND & Data). I have two of them, one for each side of the velomobile. But the computer can also run four of them in case you want four small ones instead of two large ones. I choose the large ones because of easier assembling on my Quest. Downside is it user more power.
Here is a guide for using [WS2812B LEDstrips](https://randomnerdtutorials.com/guide-for-ws2812b-addressable-rgb-led-strip-with-arduino/)

### Power

- For power i choose Lipo batteries of 12.6 Volts. Other batteries are possible.
- [Polo Mini Pushbutton Power Switch](https://www.pololu.com/product/2809) is a on/off switch, whith reverse protection and more.
- Power regulator to 5V. The Teensy has it's own regulator for 5V to 3.3V.

### LED drivers

For driving the front, rear, brake (and maybe blinker) LED's i use [Meanwell LDD Led drivers](https://www.meanwell.com/webapp/product/search.aspx?prod=LDD-L) (LDD-700 for now).

## Schematics

The raw schematics can be found in the [kicad folder](./kicad)

## Software

For developing i used [VScode](https://code.visualstudio.com/) with [PlatformIO](https://platformio.org/). Once setup, i found this much easier for developling compared the Arduino IDE. But thats a software developer speaking ;-)

## 3D print

A STL file for 3D printing the console can be found in the [3D folder](./3d).

# Next steps:

- Hardware design with connectors for easy disconnecting the computer and the dashboard.
- New design for the dashboard console with more wires to the computer (more and better options for the buttons and the brake sensor) and Led's for indicators.
- For now i soldered everything on prototyping breadboards. But real PCB's would be nice.
- ...

