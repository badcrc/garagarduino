# Version 3.0

- Now uses MQTT instead HTTP for sending data
- By default now uses a js library instead of graphite for plotting graphs
- Done at El Comité (2016-11-19)
- Brewed: None for now, will be used for my first All-Grain recipe

# Version 2.0

- Upgraded to an Arduino Mega
- Now uses an arduino Ethernet Shield, which includes microSD support as well and sends data over the net via HTTP to a server
- Also uses an external IIC Real Time Clock
- That server stores data to graphite via StatsD, included is the code for that web as well
- Brewed: Black Rock's "Whispering Wheat"

# Version 1.0

- First release! 
- Just logs to a SD card using the arduino SD shield the date, sensors and relays data
- Brewed: Brewferm's "Abdij"