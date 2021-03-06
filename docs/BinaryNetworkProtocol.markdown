# Binary network protocol specification (version 2)

## Motivation

* `QLinkedList` can't be serialized into JSON using `qjson` (not with
  the standard capabilities anyway, and I, Alexander Batischev, don't
  really want to implement serializing by myself)
* Client said that JSON is too inefficient and he wants some binary
  protocol

So here we go!

## Overview

UDP is used as underlying protocol to transmit binary-encoded messages.

Each message consists of two parts: the header and the body. Header
carries information about sender, while body contains command and some
parameters.

Upon processing the message simulator may send some response. For
`move` it may be either `bump` or nothing (if bump didn't happen). For
`who is there?` it would be `there you see`. Look up relevant sections
for details on those messages.

There are 10 types of messages:

* `move`
* `turn`
* `change size`
* `change color`
* `who is there?`
* `parameter report`
* `bump`
* `there you see`
* `start`
* `pause`

Out of those, only first 6 can be sent by agent, and the last 4 can be
sent only by the simulator.

Following are formal specification of how does header and each message
type look like.

## Header format

Header contains:

* protocol version, *1 octet*
* message's sequential number, *4 octets*, unsigned integer
* environment object's ID, *1 octet*, unsigned integer
* agent's port, *2 octets*, unsigned integer
* message type, *1 octet*

This document describes protocol version 2, so the first octet of the
message should contain 2.

Sequential numbers are set by the agent and required by him to make
sense out of responses. Simulator should just copy that field from the
message it is responding to.

Environment object's ID should be 0 when message is sent by/to the robot and
non-zero when message is sent by/to the environment controller.

Message types are mapped from names to numbers as follows:

* 0: `move`
* 1: `turn`
* 2: `change size`
* 3: `change color`
* 4: `who is there?`
* 5: `bump`
* 6: `there you see`
* 7: `parameter report`
* 8: `start`
* 9: `pause`

## `move` message

Agent sends that message to move to somewhere else.

Message contains:

* X coordinate, *4 octets*, unsigned integer
* Y coordinate, *4 octets*, unsigned integer

## `turn` message

Agent sends that message to change its orientation.

Message contains:

* seconds, *4 octets*, unsigned integer

second is 1/60 of minute, and minute is, in turn, 1/60 of degree. That
field specify new orientation of the agent. It is an absolute value
counted from the north direction, which is at the top of the map.

## `change size` message

Agent sends that message to change its size.

Message contains:

* diameter, *4 octets*, unsigned integer

## `change color` message

Agent sends that message to change its color.

Message contains:

* red value, *1 octet*
* green value, *1 octet*
* blue value, *1 octet*

## `who is there?` message

Agent sends that message to get a list of object situated inside a
circle with the given centre and radius.

Message contains:

* X coordinate, *4 octets*, unsigned integer
* Y coordinate, *4 octets*, unsigned integer
* radius, *4 octets*, unsigned integer

## `parameter report` message

Agent sends that message to report the state of one of his current
parameters.

Mesage contains:

* parameter id, *1 octet*
* integral part of the value, *4 octets*, signed integer
* real part of the value, *4 octets*, unsigned integer

Parameter id is an unsigned integer number in the range of [0; 15].

Parameter's value is split into integral part and first 6 digits of real part.
Note that real part is unsigned - sign is carried with integral part.

## `bump` message

Simulator returns that message when agent bumps into something while
moving to the point specified by `move` command. It contains new
coordinates of the agent.

Message contains:

* X coordinate, *4 octets*, unsigned integer
* Y coordinate, *4 octets*, unsigned integer

## `there you see` message

Simulator sends that message in response to `who is there?` message.

Message contains:

* number of objects found, *4 octets*, unsigned integer
* list of objects

Each object is represented as follows:

* X coordinate, *4 octets*, unsigned integer
* Y coordinate, *4 octets*, unsigned integer
* diameter, *4 octets*, unsigned integer
* seconds, *4 octets*, unsigned integer
* red, green and blue components of color, *1 octet* each

Seconds field indidates orientation of object.

List of objects is just a stream of objects descriptions.

## `start` and `pause` messages

Those are used to control simulation flow. Agent program should start
doing its work (i.e. moving agent around) upon receiving `start`
message. If `pause` is sent, agent should pause and wait for the
`start`. There's no `stop` message because agent processes are being
killed by the simulator when user wants to stop simulation.

Those messages doesn't contain anything other than header.

## Example messages

### `move` message

```
0x02                  -- 2, protocol version
0x00 0x00 0x00 0x00   -- message's sequential number
0x00                  -- env. obj. ID is 0, thus message is for robot
0x04 0x01             -- agent's port, 1025
0x00                  -- "move" message
0x00 0x00 0x00 0x15   -- x coordinate, 21
0x00 0x00 0x00 0x2a   -- y coordinate, 42
```

### `bump` message

```
0x02                  -- 2, protocol version
0x00 0x00 0x00 0x00   -- message's sequential number, copied from the
                      -- original message
0x00                  -- env. obj. ID, zero means we're talking about robot
0x04 0x01             -- agent's port, 1025
0x06                  -- "bump" message
0x00 0x00 0x00 0x0b   -- x coordinate, 11
0x00 0x00 0x00 0x0f   -- y coordinate, 15
```

### `change color` message

```
0x02                  -- 2, protocol version
0x00 0x00 0x00 0x01   -- message's sequential number, 1
0x03                  -- 3, env. object's ID
0x04 0x01             -- agent's port, 1025
                      -- agent here is environment controller application
0x03                  -- "change color" message
0xa1                  -- red component, 161
0xb2                  -- green component, 178
0xc3                  -- blue component, 195
```

### `there you see` message

```
0x02                  -- 2, protocol version
0x00 0x00 0x00 0x02   -- message's sequential number, 2
                      -- (answering to some hyphotetical request)
0x00                  -- env. obj. ID, zero means robot
0x04 0x01             -- agent's port, 1025
0x07                  -- "there you see" message
0x03                  -- number of objects found, 3
                      -- description of first object starts here
0x00 0x00 0x00 0x10   --     x coordinate, 16
0x00 0x00 0x00 0x12   --     y coordinate, 18
0x00 0x00 0x00 0x15   --     diameter, 21
0x00 0x00 0x01 0x68   --     seconds, 360 (one degree)
0x7f                  --     red component, 127
0xda                  --     green component, 218
0x3d                  --     blue component, 61
                      -- description of second object starts here
0x00 0x00 0x00 0x5d   --     x coordinate, 93
0x00 0x00 0x09 0x2a   --     y coordinate, 2346
0x00 0x00 0x00 0x03   --     diameter, 3
0x00 0x01 0xa5 0xe0   --     seconds, 108000 (30 degrees)
0x05                  --     red component, 5
0x07                  --     green component, 7
0x0b                  --     blue component, 11
                      -- description of third object starts here
0x00 0x0d 0xa6 0x3b   --     x coordinate, 894523
0x00 0x08 0xa7 0xfb   --     y coordinate, 567291
0x00 0x00 0x08 0x6c   --     diameter, 2165
0x00 0x03 0x4b 0xc0   --     seconds, 216000 (60 degrees)
0x5c                  --     red component, 92
0x41                  --     green component, 65
0x04                  --     blue component, 4
```

## Note about endianness

All integer values larger than an octet should be converted to network
byte order (big endian).
