# 03_isotp - ISOTP Multi-Frame UDS Communication

## Overview
Implements ISO 15765-2 Transport Protocol (ISOTP)
for sending and receiving multi-frame UDS messages
over CAN bus.

Solves the 8 byte CAN frame limitation by splitting
large UDS messages into multiple frames automatically.

## AUTOSAR Mapping
| Code | AUTOSAR Equivalent |
|------|--------------------|
| socket(AF_CAN, SOCK_DGRAM, CAN_ISOTP) | CanTp module init |
| addr.can_addr.tp.tx_id | CanTp TX N_SA |
| addr.can_addr.tp.rx_id | CanTp RX N_TA |
| write() | CanTp_Transmit() |
| read() | CanTp_RxIndication() |
| kernel FC handling | CanTp flow control |

## ISOTP Frame Types
```
SF = Single Frame      data[0] = 0x0X   < 7 bytes
FF = First Frame       data[0] = 0x1X   start of long message
CF = Consecutive Frame data[0] = 0x2X   remaining chunks
FC = Flow Control      data[0] = 0x30   receiver controls speed
```

## UDS Flow Diagram
```
Can_isotp_sender.c          Can_isotp_receiver.c
(ECU side)                  (Tester side)
TX: 0x7E8                   TX: 0x7DF
RX: 0x7DF                   RX: 0x7E8

write(12 bytes)
      |
      | 7E8 FF (first 6 bytes)
      |------------------------->
                                 kernel receives FF
                                 sends FC automatically
      | 7DF FC (flow control)
      <-------------------------
      |
      | 7E8 CF (remaining bytes)
      |------------------------->
                                 kernel reassembles
                                 read() returns 12 bytes
```

## Key Difference vs CAN_RAW
```
CAN_RAW:
-> struct can_frame needed
-> you handle SF FF CF FC manually
-> max 8 bytes per frame
-> you see raw CAN frames

CAN_ISOTP:
-> no struct can_frame needed!
-> kernel handles SF FF CF FC!
-> send any size data!
-> you see reassembled data!
```

## ID Setup
```
Can_isotp_sender.c       Can_isotp_receiver.c
tx_id = 0x7E8            tx_id = 0x7DF
rx_id = 0x7DF            rx_id = 0x7E8

IDs are always SWAPPED between sender and receiver!
sender TX = receiver RX
sender RX = receiver TX
```

## Programs

### Can_isotp_sender.c
Sends 12 byte UDS message over ISOTP

- TX ID  : 0x7E8 (ECU side)
- RX ID  : 0x7DF (expects FC here)
- Data   : 12 bytes UDS DiagnosticSessionControl
- Kernel : splits into FF + CF automatically

### Can_isotp_receiver.c
Receives and prints reassembled ISOTP messages

- TX ID  : 0x7DF (tester side)
- RX ID  : 0x7E8 (expects data here)
- Buffer : 4096 bytes (max UDS size)
- Kernel : sends FC and reassembles automatically

## How to Compile
```bash
gcc Can_isotp_sender.c   -o Can_isotp_sender
gcc Can_isotp_receiver.c -o Can_isotp_receiver
```

## How to Run
```bash
# Terminal 1 - start receiver FIRST!
# waits for messages on 7E8
./Can_isotp_receiver

# Terminal 2 - watch raw CAN frames
# shows FF FC CF frames
candump vcan0

# Terminal 3 - send
# sends 12 bytes
./Can_isotp_sender
```

## Expected Output

Terminal 1 (Can_isotp_receiver):
```
Socket created successfully!
Bound to vcan0 | TX: 0x7DF  RX: 0x7E8
Waiting for ISOTP messages...
Received 12 bytes: 02 10 01 11 22 33 44 55 66 77 88 99
```

Terminal 2 (candump):
```
vcan0  7E8  [8]  10 0C 02 10 01 11 22 33  <- FF First Frame
vcan0  7DF  [8]  30 00 00 00 00 00 00 00  <- FC Flow Control
vcan0  7E8  [8]  21 44 55 66 77 88 99 00  <- CF Consecutive
```

Terminal 3 (Can_isotp_sender):
```
Socket created successfully!
Bound to vcan0 | TX: 0x7E8  RX: 0x7DF
Sent 12 bytes via ISOTP!
```

## Why Receiver Must Start First?
```
FC = Flow Control
   = receiver tells sender "ready, continue!"

If sender starts first:
-> sends FF
-> waits for FC
-> nobody sends FC!
-> timeout! transmission fails!

Always start receiver before sender!
```

## Requirements
- Linux Ubuntu 22/24
- can-utils installed
- vcan0 interface running
- ISOTP kernel module loaded

## Setup
```bash
sudo modprobe vcan
sudo modprobe can_isotp
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```