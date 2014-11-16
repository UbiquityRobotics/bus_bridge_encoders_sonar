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

## Rev. C

<Table Border="1">
    <TR>
    </TR><TR>
	<TH>Arduino Name</TH>
	<TH>AVR Port</TH>
	<TH>Function</TH>
    </TR><TR>
	<TD>D0</TD>
	<TD>PD0/RXD</TD>
	<TD>USART Receive</TD>
    </TR><TR>
	<TD>D1</TD>
	<TD>PD0/TXD</TD>
	<TD>USART Transmit</TD.
    </TR><TR>
	<TD>D2</TD>
	<TD>PD3</TD>
	<TD>SONAR0_TRIG</TD>
    </TR><TR>
	<TD>D3</TD>
	<TD>PD3/OC2B</TD>
	<TD>SONAR0_ECHO</TD>
    </TR><TR>
	<TD>D4</TD>
	<TD>PD4</TD>
	<TD>SONAR1_TRIG</TD>
    </TR><TR>
	<TD>D5</TD>
	<TD>PD6/OC0B</TD>
	<TD>SONAR1_ECHO</TD>
    </TR><TR>
	<TD>D6</TD>
	<TD>PD6/OC0A</TD>
	<TD>ENC_EN</TD>
    </TR><TR>cc
	<TD>D7</TD>
        <TD>PD7<TD>
	<TD>1A</TD>
    </TR><TR>
	<TD>D8</TD>
	<TD>PB0<TD>
	<TD>4A</TD>
    </TR><TR>
	<TD>D9</TD>
	<TD>PB1/OC1A</TD>
	<TD>EN34</TD>
    </TR><TR>
	<TD>D10</TD>
	<TD>PB2/OC1B</TD>
	<TD>EN12</TD>
    </TR><TR>
	<TD>D11</TD>
	<TD>PB3/OC2A</TD>
	<TD>3A</TD>
    </TR><TR>
	<TD>D12</TD>m
	<TD>PB4</TD>
	<TD>2A</TD>
    </TR><TR>
	<TD>D13</TD>
	<TD>PB5</TD>
	<TD>LED</TD>
    </TR><TR>
	<TD>A0</TD>
	<TD>PC0</TD>
	<TD>ENC1_A</TD>
    </TR><TR>
	<TD>A1</TD>
	<TD>PC1</TD>
	<TD>ENC1_B</TD>
    </TR><TR>
	<TD>A2</TD>
	<TD>PC2</TD>
	<TD>ENC2_A</TD>
    </TR><TR>
	<TD>A3</TD>
	<TD>PC3</TD>
	<TD>ENC2_B</TD>
    </TR><TR>
	<TD>A4</TD>
	<TD>PC4</TD>
	<TD>CAN_STBY</TD>
    </TR><TR>
	<TD>A5</TD>
	<TD>PC5</TD>
	<TD>-</TD>
    </TR>
</Table>


## Rev. C

* Schematic should label C13 as 1uF. [x]

* Schematic should label R4 as 120_1%. [x]

* Schematic has BGND and BPWR swapped on root diagram. [x]

* Make sure regulator input capacitors can take 30V.  Probably switch
  over to aluminum electrolytic capacitor. [x]

* Move ISP connector up to make more room for Atmel-ICE connector. [x]

* Between Dual Slot Encoder (Rev. D) and Rev. C of this
  board, pins 4 and 5 are swapped as are pins 6 and 7.
  [change dual slot encoders instead]
  See below:

<BlockQuote>
<Pre>
     BBES-C       DSE-D
    ===========================
     MOTOR_A   1  MOTOR_A
     MOTOR_A   2  MOTOR_A
     LOG_5V    3  LPWR (=5V)
     LGND      4  QUAD_A
     QUAD_A    5  LGND (not really used)
     QUAD_B    6  LED_EN
     LED_EN    7  QUAD_B
     LED_5V    8  BPWR
     MOTOR_B   9  MOTOR_B
     MOTOR_B  10  MOTOR_B
</Pre>
</BlockQuote>

## Rev. D

* The vertical resistors are hard to solder.  Think about switching

* Update schematic to specifiy electrolytic capacistors for input
  voltage regulators.

* The C5 is too close to the mounting hole.

* Think about adding an LED jumper block.

