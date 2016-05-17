About poseidon
==============

poseidon is a very generic scheduler for time division multiplexing (TDM) network on chip.
poseidon uses Metaheuristics to optimize the schedule.

poseidon generates a schedule from a specification of the platform and a specification of the communication requirements.


Getting Started
===============
poseidon is integrated with the T-CREST project and is checked out by the Patmos configuration.
Because poseidon is very generic it can be used by other platforms as well.
Checking out and building poseidon can be done as follows:

    git clone https://github.com/t-crest/poseidon.git poseidon
    cd poseidon && make


Platform Specification
----------------------
poseidon needs a specification of the platform, and how it is structured at the pipeline level.
Below we show how an N by M bitorus network can be specified. Other types of topologies are "mesh" and "custom".
If "custom" is specified then each unidirectional link must be specified with a sink and a source node.
Links must be connected north-south or east-west, no diagonal links.
To verify a custom topology poseidon can draw the specified topology with the '-d' option.


    <?xmlversion="1.0" encoding="UTF-8"?>
    <platform width="N" height="M">
        <topology type="bitorus" routerDepth="X" linkDepth="Y" ></topology>
    </platform>


Communication Specification
---------------------------
If the programmer needs a communication pattern different from all-to-all with one phit per packet, poseidon needs a specification of the communication pattern that is requested to be scheduled onto the platform.
Below we show the structure of a custom communication pattern.
The number of phits can also be specified on individual channels, as shown on the last channel.

    <communication type="custom" phits="3" >
        <channel from="(0,0)" to="(1,1)" bandwidth="10" />
        <channel from="(0,1)" to="(2,2)" bandwidth="2" />
        <channel from="(0,2)" to="(2,2)" bandwidth="2" />
        <channel from="(1,1)" to="(2,2)" bandwidth="2" phits="4" />
    </communication>

TODO
----
- Calculation of the bisection bound for arbitrary communication patterns.
- Check local source and destination links before other links.
- Predecessor structure with constant lookup time for next free time slot.
- Termination of program when solution falls in some interval of lower bound.
