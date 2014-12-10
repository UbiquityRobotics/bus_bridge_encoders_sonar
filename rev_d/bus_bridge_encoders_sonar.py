#!/usr/bin/env python

def main():
    # Quadrature encoders transition between 4 states using a 2 bit
    # Gray code.  The states are:
    #
    #         00
    #         01
    #         11
    #         10
    #
    # The nice thing about Gray codes is that only 1 bit changes
    # at a time between states.
    #
    # The states are arranged in a ring like this:
    #
    #        00 <----> 01
    #        ^          ^
    #        |          |
    #        v          v
    #        10 <----> 11
    #
    # Each state transition around the ring in the clockwise direction
    # results in incrementing the position counter.  Conversely, each
    # state in the counter clockwise direction decrements the position
    # counter.  This is represented as follows:
    #
    #          +1                     -1
    #       00 ---> 01           00 <--- 01
    #       ^        | +1     -1 |        ^
    #       |        |           |        |
    #     +1|        v           v        | -1
    #       10 <--- 11           10 ---> 11
    #            +1                 -1
    #
    # Now the question occurs what happens if somehow because of
    # random glitches you get a two bit transition?  Crash and
    # burn?  Nah!  You assume that you accidently missed the
    # intervening transition and do either an increment by 2 or
    # or a decrement by 2.  But which do you do, an increment or
    # a decrement?
    #
    # The solution to the problem is to add a another state bit
    # which remembers whether or not the last operation was an
    # increment or a decrement and perform the same operation
    # again.
    #
    # So the state of the encoder is represented as "SGG", where
    # "S" is 1 if the last operation was a decrement and 0 if it
    # was an increment and "GG" is the 2-bit Gray code.  For each
    # state there are 4 possible "next" Gray code inputs.  So
    # we figure what we want to do for each possible Gray code
    # input.  The table is shown below:

    transitions = {}
    transitions[0b000] = [(0b00,  0), (0b01, +1), (0b11, +2), (0b10, -1)]
    transitions[0b001] = [(0b00, -1), (0b01,  0), (0b11, +1), (0b10, +2)]
    transitions[0b011] = [(0b00, +2), (0b01, -1), (0b11,  0), (0b10, +1)]
    transitions[0b010] = [(0b00, +1), (0b01, +2), (0b11, -1), (0b10,  0)]
    transitions[0b100] = [(0b00,  0), (0b01, +1), (0b11, -2), (0b10, -1)]
    transitions[0b101] = [(0b00, -1), (0b01,  0), (0b11, +1), (0b10, -2)]
    transitions[0b111] = [(0b00, -2), (0b01, -1), (0b11,  0), (0b10, +1)]
    transitions[0b110] = [(0b00, +1), (0b01, -2), (0b11, -1), (0b10,  0)]

    # Now we want to create a 32-byte state transition table.  The
    # bytes are going to be signed, so that when we do a right shift
    # operation, the most significant bit is sign exteneded.
    # The operation of the state machine is:
    #
    # * SGG is the current state (as described above.)
    # *  gg is the next 2 bits of encoder reading.
    #
    # We will index into the table using "ggSGG" and the output
    # of the table will be "cccccsgg", where
    #
    #    "ccccc" is a 5-bit signed increment/decrement value to
    #           apply to the position counter.
    #    "sgg" is next state.
    #
    # The C code is:
    #
    #    unsigned char state;
    #    signed long position;
    #    unsigned char state_transition_table[32] = {...};
    #
    #    while (1) {
    #        unsigned char index = state;
    #        if (encoder_phase_a) {
    #            index |= (0b01 << 3);
    #        }
    #        if (encoder_phase_b) {
    #            index |= (0b10 << 3);
    #        }
    #        signed char state_transition = state_transition_table[index];
    #        position += (transition >> 3);
    #        state = (unsigned char)transition & 0b111;
    #    }

    # Construct *state_transition_
    state_transition_table = range(32)
    for index in range(32):
        state_transition_table[index] = None

    # Now we fill in the *state_transition_table*:
    for state, quad in transitions.items():
        #print("[{0:3b}]: {1}".format(state, quad))
	for pair in quad:
	    gg = pair[0]
	    ccccc = pair[1]
	    #print("    {0:2b} {1}, {2:5b}".format(gg, ccccc, ccccc & 0x1f))
    
	    # Compute the index *ggSGG*:
            ggSGG = (gg << 3) | state

            # Compute *sgg*:
	    if ccccc == 0:
                # Use previous state for *sgg*:
                sgg = (state & 0b100) | gg
	    else:
	        # Use the count sign for *sgg*:
		if ccccc > 0:
		    sgg = gg
		elif ccccc < 0:
		    sgg = 0b100 | gg
	        else:
		    assert False

	    ccccc &= 0x1f
            cccccsgg = (ccccc << 3) | sgg
	    #print("     {0:5b} {1:3b} {1:8b}".format(ccccc, sgg, cccccsgg))
            state_transition_table[ggSGG] = cccccsgg

    # Now we can print out the *state_transition_table:
    convert = {}
    convert[0b11110] = "-2"
    convert[0b11111] = "-1"
    convert[0b00000] = " 0"
    convert[0b00001] = "+1"
    convert[0b00010] = "+2"

    print("// This table is computed by bus_bridge_encoders_sonar.py")
    print("// Please read the comment in the Python code to understand")
    print("// the structure of the *state_transition_table* below:")
    print("static signed char state_transition_table[32] = {")
    print("    //  ccccc     sgg  // [xx]: SGG[gg] => sgg +/-")
    for ggSGG in range(32):
	gg = ggSGG >> 3
	SGG = ggSGG & 0b111
        cccccsgg = state_transition_table[ggSGG]
        ccccc = cccccsgg >> 3
        sgg = cccccsgg & 0b111
	signed_count = convert[ccccc]
        print(("    (signed char)((0x{0:02x} << 3) | 0x{1:01x})," +
	  " // [{2:02d}]: {2:03b}[{4:02b}] => {5:03b} {6}").
	  format(ccccc, sgg, ggSGG, SGG, gg, sgg, signed_count))
    print("};")

main()

