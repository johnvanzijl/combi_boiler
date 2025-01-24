Project for heating 2 hotwater boilers with surplus Solar energy

A esp32 connected to the heater sends the current power as an udp broadcast every 2 seconds to all users in the subnet.
This esp32 recieves this broadcast and adjusts the power output to the boilers.
The output to the boilers is done via 2 PWM output signals to a solid state regulator, basically a big dimmer  (https://www.aliexpress.com/item/1005007809326780.html)
The regulators work from 2 to 5V, so the 3,3V PWM output is not sufficient. A non inverting opamp circuit with an 1.5 times amplification solves this problem.

It should have:
disable inputs
forced setpoints
a smartmeter follow function
a temperature limit
detection of fully charged
display for current power en temperature per boiler
top up function early in the morning for the morning shower
