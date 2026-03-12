// Hints only — you fill the code!

// Step 1: include 7 headers
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
// Step 2: main function
int main()
{

// Step 3: create socket
//         socket(?, ?, ?);
int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

// Step 4: bind to vcan0
//         sockaddr_can structure
struct sockaddr_can addr;
memset(&addr, 0, sizeof(addr));
//         memset
//         can_family
addr.can_family = AF_CAN;
//         can_ifindex
addr.can_ifindex = if_nametoindex("vcan0");

bind(sock,(struct sockaddr*)&addr, sizeof (addr));
printf("Socket created: %d\n", sock);
printf("Bound to vcan0 successfully!\n");
// Step 5: loop forever
int frame_count = 0;
struct can_frame frame;
while (1)
{
//         read() each frame
int bytes = read (sock, &frame, sizeof(frame));
if (bytes > 0)
{    
    frame_count++;
//         print can_id
printf("Frame %d: ID : 0x%X Data :", frame_count, frame.can_id);
//         print data bytes
for (int i =0; i < frame.can_dlc; i++)
{
    printf("%02X ",frame.data[i]);
}
printf("\n");
        fflush(stdout);
}
}

// Step 6: close socket
close (sock);

return 0;
}