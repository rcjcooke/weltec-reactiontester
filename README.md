# weltec-reactiontester

Weltec MG7013 Project 1: Reaction Tester using a Teensy and a "bubble display" - 4-digit-7-segment display

* The system randomly activates an LED
* The user has to press a button as quickly as possible after the LED lights up
* The system will show the user's reaction time in milliseconds on the bubble display

## Design / Assembly

Based on datasheet for the 4-digit-display, the maximum forward current per segment is 110mA. This exceeds the maximum pin current on the Teensy so we need to switch it separately. We achieve this with a MOSFET.

The forward voltage of each segment is typically 1.6V so we need to cut down our 3.3V supply voltage with a voltage divider. The resistance across the diode for it's specified average segment current of 5mA = 1.6/0.005 = 320 Ohm. 

![Equation 1](eq1.PNG)

Using this we can therefore use ratios to calculate the resistor we need. R/(R+320) = (3.3-1.6)/3.3 => R = 340 Ohm per segment.

![Equation 2](eq2.PNG)

Note that we're assuming we add resistors per segment rather than per digit. This is so that we have consistent brightness for a digit regardless of how many segments are lit.

### Components / BoM
| Supplier | Supplier Part # | # | Description            | MNF Part #    |
|:---------|:---------------:|---|:-----------------------|:--------------|
|          |                 | 1 | Teensy 3.2             |               |
|          |                 | 1 | LED                    |               |
|          |                 | 1 | 4-digit LED Display    | QDSP6064      |
|          |                 | 8 | 340 Ohm Resistor       |               |
|          |                 | 4 | 1k Ohm Resistor        |               |
|          |                 | 4 | 10k Ohm Resistor       |               |
|          |                 | 1 | N-Channel Power MOSFET | IRLU8743 IPAK |
|          |                 | 1 | Button                 |               |

### Electronics

The (somewhat difficult to decipher!) breadboard design can be seen below.

![Breadboard design](./electronics/breadboard.png)

Note: Fritzing didn't include QDSP6064 out the box so I used the part available from: https://github.com/RichardBronosky/QDSP-6064

The schematic below shows an example for 2 segments and 2 digits wired up with the external MOSFETs for a complete circuit. In reality, the segments and digits are wired together with a common cathode internally to the seven segment display.

![wiring of digits and segments](circuitwiring.PNG)

The schematic, below, is the external wiring for the componentry.

![circuit schematic](schematic.PNG)

It is worth noting the resistors:
* The 1k resistors are used as current limiting resistors to ensure the microcontroller isn't damaged by excessive current draw when the MOSFET is switched high. At that point it is entirely discharged so will potentially have a very high current draw.
* The 10k resistors are there to ensure that the MOSFET gate is pulled low to ensure that the gate is off.
* The 340 Ohm resistors, as mentioned previously, act as current limiting resistors for the segment LEDs. 

### Software
Of interest in the software is:
* **The switch debouncing** - we intentionally trigger on first interrupt and then lock out to ensure we know instantly when the user presses the button. We then measure the time from the moment the light is turned on to the first interrupt.
* **The Seven Segment library setup** - We use N-TRANSISTOR in the setup since we are using external MOSFETs to switch the digits on. This means that they need to be switched high, despite the fact that the segment LEDs share a common cathode.

## Results
The system works as expected.

An interesting side-effect of the seven segment library is that if the reaction time is longer than 9999 milliseconds, the library realises that this takes more than 4 digits and replaces the output with "`----`".

My reaction times to the rounded down to the nearest milliseconds on 5 successive tests came out as:

| Reaction Time / ms |
|--------------------|
| 231                |
| 181                |
| 189                |
| 255                |
| 204                |

## Conclusions

Although MOSFETs and limiting resisters were used in this case, they were generally not needed since the currents running through this particular display unit were so low, however this circuit can be used as is with much higher power display units with the MOSFETs in place. 

## References
