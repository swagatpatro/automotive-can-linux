#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
int main()
{
    int sock = socket ( AF_CAN, SOCK_RAW, CAN_RAW);//socket creation

    //bind to vacn0
    struct sockaddr_can addr; 
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex("vcan0");
    bind (sock, (struct sockaddr*)&addr, sizeof (addr));
    printf("Socket created: %d\n", sock);
    printf("Bound to vcan0 successfully!\n");

    // filter setup
    struct can_filter filters[2];

    filters[0].can_id = 0x123;
    filters[0].can_mask = CAN_SFF_MASK;

    filters[1].can_id = 0x7DF;
    filters[1].can_mask = CAN_SFF_MASK;

    setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER, &filters, sizeof(filters));

    //loop read and print
    struct can_frame rx_frame;
    while(1)
    {

        int bytes = read(sock, &rx_frame, sizeof(rx_frame));
        if (bytes >0)
        {
            printf("ID: 0x%X Data:",rx_frame.can_id);

            for (int i =0; i < rx_frame.can_dlc; i++)
            {
                printf("%02X ", rx_frame.data[i]);
            }
            printf("\n");
            fflush(stdout);
        }

    }
    close (sock);
    return 0;
}
