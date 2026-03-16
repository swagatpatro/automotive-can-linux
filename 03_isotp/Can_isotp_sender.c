/**
 * Can_isotp_sender.c
 * ISOTP Message Sender using SocketCAN
 *
 * Sends multi-frame UDS messages over CAN bus
 * Kernel automatically handles:
 * -> SF (Single Frame)    if data < 7 bytes
 * -> FF (First Frame)     first chunk
 * -> CF (Consecutive Frame) remaining chunks
 * -> FC (Flow Control)    received from receiver
 *
 * UDS Flow:
 * Sender -> 7E8 FF + CF  (sends data)
 * Receiver-> 7DF FC      (kernel handles)
 *
 * AUTOSAR Equivalent: CanTp_Transmit()
 */

#include <stdio.h>          // printf, perror
#include <string.h>         // memset
#include <unistd.h>         // write, close
#include <net/if.h>         // if_nametoindex
#include <sys/socket.h>     // socket, bind
#include <linux/can.h>      // sockaddr_can
#include <linux/can/raw.h>  // CAN_RAW
#include <linux/can/isotp.h>// CAN_ISOTP, SOCK_DGRAM

int main()
{
    /*
     * STEP 1 - Create ISOTP Socket
     *
     * SOCK_DGRAM  = datagram socket (not raw frames!)
     * CAN_ISOTP   = ISO 15765-2 transport protocol
     *
     * Difference from CAN_RAW:
     * CAN_RAW   -> you handle each frame manually
     * CAN_ISOTP -> kernel handles FF, CF, FC for you!
     *
     * AUTOSAR Equivalent: CanTp module initialization
     */
    int sock = socket(AF_CAN, SOCK_DGRAM, CAN_ISOTP);
    if(sock < 0) {
        perror("Socket creation failed");
        return 1;
    }
    printf("Socket created successfully!\n");

    /*
     * STEP 2 - Bind to vcan0
     *
     * tx_id = 0x7E8  -> ECU side transmit ID
     * rx_id = 0x7DF  -> expect FC from tester on 7DF
     *
     * NOTE: tx_id and rx_id are SWAPPED vs Can_isotp_receiver.c!
     * Can_isotp_sender.c   -> tx=7E8, rx=7DF  (ECU side)
     * Can_isotp_receiver.c -> tx=7DF, rx=7E8  (Tester side)
     *
     * AUTOSAR Equivalent: CanTp connection parameter setup
     */
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));         // clear garbage values
    addr.can_family          = AF_CAN;
    addr.can_ifindex         = if_nametoindex("vcan0");
    addr.can_addr.tp.tx_id   = 0x7E8;      // ECU TX ID
    addr.can_addr.tp.rx_id   = 0x7DF;      // expect FC from tester

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Bound to vcan0 | TX: 0x7E8  RX: 0x7DF\n");

    /*
     * STEP 3 - Build and Send UDS Data
     *
     * 12 bytes = more than 8 bytes max CAN frame!
     * kernel automatically splits into:
     * -> FF (First Frame)  first 6 bytes
     * -> CF (Consecutive)  remaining bytes
     *
     * No struct can_frame needed!
     * -> just pass raw data to write()
     * -> kernel handles framing!
     *
     * UDS DiagnosticSessionControl request:
     * data[0] = 0x02  length
     * data[1] = 0x10  service ID
     * data[2] = 0x01  default session
     * data[3-11]      extra bytes to force multiframe
     *
     * AUTOSAR Equivalent: CanTp_Transmit()
     */
    unsigned char data[] = {
        0x02, 0x10, 0x01,   // UDS DiagnosticSessionControl
        0x11, 0x22, 0x33,   // extra bytes
        0x44, 0x55, 0x66,   // to force
        0x77, 0x88, 0x99    // multiframe!
    };

    int bytes = write(sock, data, sizeof(data));
    if(bytes > 0)
        printf("Sent %d bytes via ISOTP!\n", bytes);
    else {
        perror("Send failed");
        return 1;
    }

    /*
     * STEP 4 - Cleanup
     * AUTOSAR Equivalent: CanTp_Shutdown()
     */
    close(sock);
    return 0;
}