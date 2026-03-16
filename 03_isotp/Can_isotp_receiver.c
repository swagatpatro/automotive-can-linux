/**
 * Can_isotp_receiver.c
 * ISOTP Message Receiver using SocketCAN
 *
 * Receives multi-frame UDS messages over CAN bus
 * Kernel automatically handles:
 * -> FF (First Frame)      reassembly
 * -> CF (Consecutive Frame) reassembly
 * -> FC (Flow Control)     sending
 *
 * UDS Flow:
 * ECU      -> 7E8 FF + CF  (sends data)
 * Receiver -> 7DF FC       (kernel sends automatically)
 * Receiver -> prints reassembled data
 *
 * AUTOSAR Equivalent: CanTp_RxIndication()
 */

#include <stdio.h>          // printf, perror
#include <string.h>         // memset
#include <unistd.h>         // read, close
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
     * tx_id = 0x7DF  -> tester side transmit ID
     * rx_id = 0x7E8  -> expect data from ECU on 7E8
     *
     * NOTE: tx_id and rx_id SWAPPED vs Can_isotp_sender.c!
     * Can_isotp_sender.c   -> tx=7E8, rx=7DF  (ECU side)
     * Can_isotp_receiver.c -> tx=7DF, rx=7E8  (Tester side)
     *
     * AUTOSAR Equivalent: CanTp connection parameter setup
     */
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));         // clear garbage values
    addr.can_family          = AF_CAN;
    addr.can_ifindex         = if_nametoindex("vcan0");
    addr.can_addr.tp.tx_id   = 0x7DF;      // tester TX ID
    addr.can_addr.tp.rx_id   = 0x7E8;      // expect from ECU

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("Bound to vcan0 | TX: 0x7DF  RX: 0x7E8\n");
    printf("Waiting for ISOTP messages...\n");

    /*
     * STEP 3 - Receive Loop
     *
     * data[4096] = large buffer for any UDS message
     * 4096 bytes = max UDS message size
     *
     * read() is BLOCKING:
     * -> program waits here until message arrives
     * -> kernel reassembles FF + CF into complete data
     * -> read() returns only when full message received!
     * -> bytes = total reassembled data length
     *
     * No struct can_frame needed!
     * -> CAN_ISOTP kernel handles raw frames
     * -> you only see final reassembled data!
     *
     * AUTOSAR Equivalent: CanTp_RxIndication() callback
     */
    unsigned char data[4096];

    while(1)
    {
        // block until complete ISOTP message arrives
        int bytes = read(sock, data, sizeof(data));

        if(bytes > 0)
        {
            // print total bytes received and data
            printf("Received %d bytes: ", bytes);
            for(int i = 0; i < bytes; i++)
                printf("%02X ", data[i]);
            printf("\n");
            fflush(stdout);  // force immediate print
        }
    }

    /*
     * STEP 4 - Cleanup
     * AUTOSAR Equivalent: CanTp_Shutdown()
     */
    close(sock);
    return 0;
}