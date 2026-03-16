/**
 * can_filter.c
 * CAN Frame Filter using SocketCAN
 * 
 * Accepts ONLY specific CAN IDs
 * Filters: 0x123 and 0x7DF (UDS functional address)
 * 
 * AUTOSAR Equivalent: CanIf HW Filter configuration
 * Formula: (received_id & mask) == (filter_id & mask)
 */

#include <stdio.h>       // printf, perror
#include <string.h>      // memset
#include <unistd.h>      // read, write, close
#include <net/if.h>      // if_nametoindex
#include <sys/socket.h>  // socket, bind, setsockopt
#include <linux/can.h>   // can_frame, sockaddr_can
#include <linux/can/raw.h> // CAN_RAW_FILTER, CAN_SFF_MASK

int main()
{
    /* ================================================
     * STEP 1 — Create CAN Socket
     * AUTOSAR Equivalent: CanIf_Init()
     * ================================================ */
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if(sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    /* ================================================
     * STEP 2 — Bind to vcan0 interface
     * AUTOSAR Equivalent: CanIf controller binding
     * ================================================ */
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));      // clear garbage
    addr.can_family  = AF_CAN;           // CAN address family
    addr.can_ifindex = if_nametoindex("vcan0"); // interface index
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    printf("Socket created: %d\n", sock);
    printf("Bound to vcan0 successfully!\n");

    /* ================================================
     * STEP 3 — Configure CAN Filters
     * AUTOSAR Equivalent: CanIf HW Filter config
     * Formula: (rx_id & mask) == (filter_id & mask)
     * ================================================ */
    struct can_filter filters[2];

    // Filter 1 — accept 0x123 only (exact match)
    filters[0].can_id   = 0x123;
    filters[0].can_mask = CAN_SFF_MASK;  // 0x7FF — all bits checked

    // Filter 2 — accept 0x7DF only (UDS functional address)
    filters[1].can_id   = 0x7DF;
    filters[1].can_mask = CAN_SFF_MASK;  // 0x7FF — all bits checked

    // Apply filters to socket
    setsockopt(sock,
               SOL_CAN_RAW,
               CAN_RAW_FILTER,
               &filters,
               sizeof(filters));

    printf("Filter active: accepting 0x123 and 0x7DF only\n");

    /* ================================================
     * STEP 4 — Receive Loop
     * AUTOSAR Equivalent: CanIf_RxIndication callback
     * ================================================ */
    struct can_frame rx_frame;

    while(1)
    {
        // Block until frame arrives — filtered by kernel
        int bytes = read(sock, &rx_frame, sizeof(rx_frame));

        if(bytes > 0)
        {
            // Print CAN ID
            printf("ID: 0x%X  DLC: %d  Data: ",
                   rx_frame.can_id,
                   rx_frame.can_dlc);

            // Print data bytes
            for(int i = 0; i < rx_frame.can_dlc; i++)
                printf("%02X ", rx_frame.data[i]);

            printf("\n"); 
            fflush(stdout);  // force immediate print
        }
    }

    /* ================================================
     * STEP 5 — Cleanup
     * AUTOSAR Equivalent: CanIf_DeInit()
     * ================================================ */
    close(sock);
    return 0;
}
