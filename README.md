# DIY-Laser-Tag
### The LDR Variant

This is a DIY Laser Tag project that I worked on for my College's IEEE event. 
Everything had to be finished in a very limited time frame of two to three days, right from coding the software and building the prototype circuit, to fitting it all inside six separate guns.
Originally, I had planned for it to be an IR emitter and receiver system.
With IR, we would be able to have a more concrete stat system, such as who killed whom, what player has the most kills, who has the least, etc.
But after an entire day of testing and protoyping with the components we had available, I found out that the components weren't accurate and stable enough.
The plan then was to either scrap the event or try to come up with another tagging method.

That is where this project was born. With LDR, we won't be able to tell who shot whom, only that a particular person had been shot. This meant that we had to change the rules around a bit.
In this gamemode, points would be tallied based on how much a particular person got shot. At the end of the match, the person/team with the least deaths would win.

This is not perfect, neither is it a fully fledged Laser Tag system for commercial use by any means.
But it *did* get the job done for the event, and if you've somehow wound up on this page, might be that it could help you as well!
Feel free to take a look at the code, use it as a base reference for your project or edit it however you see fit.

## Building the project

The system consists of nothing but a pairs of taggers and armour vests.
The armour vests would be decorated with LDRs on the front and back.

For the guns themself, you can either 3D print them or pry open a ready-made Toy Gun brought from the market.
There are several cad files available online if you want to pick the 3D printing route.

The circuit I made consisted of LDRs, a button (Trigger), vibrational motors for haptic feedback, a Laser module, a sound emitting buzzer, and ofcourse, the esp32.
