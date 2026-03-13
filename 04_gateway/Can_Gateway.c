/**
 * can_gateway.c
 * CAN Gateway using SocketCAN
 *
 * Forwards CAN frames from vcan0 to vcan1
 * Simulates a real automotive gateway ECU
 *
 * Network topology:
 * vcan0 (Powertrain bus) -> Gateway -> vcan1 (Body bus)
 *
 * AUTOSAR Equivalent: PduR routing table
 * read()  = CanIf_RxIndication()
 * write() = CanIf_Transmit()
 */

#include <stdio.h>          // printf, perror
#include <string.h>         // memset
#include <unistd.h>         // read, write, close
#include <sys/socket.h>     // socket, bind
#include <net/if.h>         // if_nametoindex
#include <linux/can.h>      // can_frame, sockaddr_can
#include <linux/can/raw.h>  // CAN_RAW

int main()
{
    /*
     * STEP 1 - Create two CAN sockets
     *
     * socket1 - RX socket bound to vcan0 (source bus)
     * socket2 - TX socket bound to vcan1 (destination bus)
     *
     * AUTOSAR Equivalent:
     * socket1 = CanIf RX channel 0 (Powertrain)
     * socket2 = CanIf TX channel 1 (Body)
     */
    int socket1 = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    int socket2 = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    if(socket1 < 0 || socket2 < 0) {
        perror("Socket creation failed");
        return 1;
    }

    /*
     * STEP 2 - Bind sockets to CAN interfaces
     *
     * addr1 -> vcan0 (Powertrain bus - source)
     * addr2 -> vcan1 (Body bus - destination)
     *
     * memset clears garbage values before use
     * AUTOSAR Equivalent: CanIf controller binding
     */
    struct sockaddr_can addr1, addr2;

    // clear both address structures
    memset(&addr1, 0, sizeof(addr1));
    memset(&addr2, 0, sizeof(addr2));

    // configure vcan0 - source bus
    addr1.can_family  = AF_CAN;
    addr1.can_ifindex = if_nametoindex("vcan0");

    // configure vcan1 - destination bus
    addr2.can_family  = AF_CAN;
    addr2.can_ifindex = if_nametoindex("vcan1");

    // bind socket1 to vcan0
    bind(socket1, (struct sockaddr*)&addr1, sizeof(addr1));

    // bind socket2 to vcan1
    bind(socket2, (struct sockaddr*)&addr2, sizeof(addr2));

    printf("Gateway ready: vcan0 -> vcan1\n");
    printf("Waiting for frames on vcan0...\n");

    /*
     * STEP 3 - Gateway routing loop
     *
     * Rx_frame - stores incoming frame from vcan0
     * Tx_frame - stores outgoing frame to vcan1
     *
     * No memset needed for Rx_frame:
     * -> read() fills it completely
     *
     * No memset needed for Tx_frame:
     * -> Tx_frame = Rx_frame copies everything
     *
     * AUTOSAR Equivalent: PduR_CanIfRxIndication()
     * routing table forwards PDU to destination
     */
    struct can_frame Rx_frame, Tx_frame;

    while(1)
    {
        /*
         * read() is BLOCKING
         * program waits here until frame arrives on vcan0
         * no CPU usage while waiting
         * AUTOSAR Equivalent: CanIf_RxIndication() callback
         */
        int bytes = read(socket1, &Rx_frame, sizeof(Rx_frame));

        if(bytes > 0)
        {
            // print received frame details
            printf("vcan0 RX: ID: 0x%X  DLC: %d  Data: ",
                   Rx_frame.can_id, Rx_frame.can_dlc);
            for(int j = 0; j < Rx_frame.can_dlc; j++)
                printf("%02X ", Rx_frame.data[j]);
            printf("\n");
            fflush(stdout);

            /*
             * Copy entire frame to Tx_frame
             * copies can_id, can_dlc and all data bytes
             * AUTOSAR Equivalent: PduR copies PDU data
             */
            Tx_frame = Rx_frame;

            /*
             * Forward frame to vcan1
             * AUTOSAR Equivalent: CanIf_Transmit()
             */
            write(socket2, &Tx_frame, sizeof(Tx_frame));
            printf("vcan1 TX: ID: 0x%X  forwarded!\n", Tx_frame.can_id);
            fflush(stdout);
        }
    }

    /*
     * STEP 4 - Cleanup
     * Close both sockets
     * AUTOSAR Equivalent: CanIf_DeInit()
     */
    close(socket1);
    close(socket2);
    return 0;
}