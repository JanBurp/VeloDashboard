# Velo Dashboard

A bicycle computer made for velomobiles.

## Reasons

In the summer of 2022 i bought a second hand [Quest](https://www.velomobiel.nl/quest/) velomobile.
It was in very good shape, had only driven 8000km while 10 years old.

But it was missing blinkers, had a 6V system on board, and two headlights which could be turned on independent from eachother. Which results in my Quest looking cross-eyed :-(
That could be done better.

And i struggled finding the 'perfect' bicycle computer to keep track of at least my current speed, average speed, maximum speed, time, time of the trip, distance, and at least two seperate distance counters.
The most anoying were the small screen, the not so ideal combination of functions, button combo's etc. And a GPS one took too long to recognise my location so i allways missed a few km's.

I took some inspiration found online from other velonauts. Such as:
- [VeloPuter](https://veloputer.wordpress.com/about/)
- [Arduino in VM](https://quest.robbroek.nl/search/label/arduino)
- [Velomobiel Led strip verlichting](https://bickyenzijnfietsen.blogspot.com/2018/10/velomobiel-led-strip-verlichting.html)

## Choices

My take on a bicycle computer for my velomibile must have at least this features:

Lighting:
- Drive LED strips on each side of the velomobile. For blinkers (with nice animation!), and normal light for visibility (front, rear and brake light).
- Drive the double front lights in several steps (off, dim, normal, beam, fog)
- Drive the normal LED lights that are standard in most velombiles (rear light, brake light, fog light)
  
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
- Big display!
- Battery status
- Battery saving modes
- Automatic turn off after a set time (to spare battery)
- Option to set the total distance
- Option to set the clock time
- Option to set the wheel circumference
- Optionts to reset all extra counters
  
Knobs:
- All controls should be on the steering wheel.
- At least knobs for the blinkers, lights, horn and switching through several pages on the display.
- Nice 3D printed console with the kbobs, on the steer.

# Result

I have a working prototype since december 2022, with several improvements and new featuers since then. Still in beta/prototype fase. Results and images will follow in the future...

