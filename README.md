# mini_bridge_encoders_sonar

The Bridge/Encoders/Sonar Mini-Shield is a
[Busino](https://github.com/waynegramlich/busino)
compatible mini-shield.  It provides the ability
to connectxyzo to:

* 2 small 1A DC motors,

* 2 shaft encoders, and

* 2 Sonar range finders.

The power comes from the motor power bus and is
fully electrically isolated (using opto-isolators)
from the logic bus.

Currently, everything is located in the `rev_a` directory.

## Revision A

The revision A schematic is available as either
`mini_bridge_encoders_sonar.pdf` or as
`mini_bridge_encoders_sonar.svg`.

The is circuit is broken to electrically isolated sections.
The schematic shows the electrical isolation boundary.

### Logic Side

Logic ground comes in on N2 (pins 5 and 6).  5 Volts
comes in on N2 (pin 3.)  5 volts and ground are routed
to the two shaft encoder connectors N3 and N4 as well
as the two sonar connectors N7 and N8.

The first shaft encoder connects the A and B quadrature
signals to  D5 and D6 on N1.  The second shaft encoder
connects the A and B  quadrature signals to D4 and D2.

{more goes here.}

## Rev. A Issues

* Tie off unused inputs of U7 to 0V or 5V. [x]

* Replace R8, R9, R10, R11, R12, R13 with a 10K ohm SIP package. [x]

* Add protection diodes to the motor outputs.  The 754410 does
  **not** have protection diodes like the L793D did.  [No.  If
  it is an issue, use the L293D instead.]

* Adding a 2x3 pin ISP programming header. [x]

* The U8 TXD (pin 1) and RXD (pin 4) are swapped. [x]

* Replace the R17, R18, R20, R21 voltage dividers with 100K pull-ups. [x]

* Add a 100K pull up U7A pin 1 to protect against U8 not
  being plugged in. [x]

* Combine Encoder and Motor onto a single 2x4 ribbon cable. [x]

* Add an LED that can be blinked. [x]

## Rev. B Issues

* Connected U8 STBY (pin 8) to a processor pin [x]

