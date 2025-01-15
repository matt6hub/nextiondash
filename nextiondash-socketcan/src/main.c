//Install socketcan library with sudo apt-get install libsocketcan

//Dependencies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fnctl.h>
#include <errno.h>
#include <socketcan.h> //For SocketCAN
#include <wiringSerial.h> //For NEXTION

//Interface Definitions
#define IF_CANBUS "vcan0" //interface name for socketcan testing interface
#define IF_NEXTION "/dev/ttyS0" //interface name for nextion testing interface

//Other Variable Definitions
double spdconstant = (2.25 / 256) * 1.609344;

//CAN Related Variables
unsigned char flagRecv, len = 0;
unsigned char buf[8];

//Vehicle Telemetry Variables
unsigned int rpm, tps, gear = 0;
double spd, afr, iat, clt, battery = 0.0;

//Method to send command to Nextion
int next_cmd(int serial_fd, const char *cmd){
    write(serial_fd, cmd, strlen(cmd)); //Send the command string
    write(serial_fd, "\xFF\xFF\xFF", 3); //Send the command terminator
    return 0;
}

//SocketCAN initialization Method
int socketcan_init(const char *iface){
    
    /*
        This chunk of code below creates a socket and checks if the socket creation failed.
        If Socket creation fails, error is thrown and program terminates.
    */
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(s < 0){
        perror("ERROR: Socket Creation Failure");
        exit(1);
    }

    /*
        This code first creates an ifreq struct named ifr to handle network interface requests including manipulation of the interface properties.
        strncpy is used to set the ifr_name property of ifr from iface, while making sure that it will not overflow the buffer. 
        Finally, ioctl is used to get the index of the network name, s for socket opened earlier,  (SIOCGIFINDEX) interface index, all stored in ifr. 
    */

    struct ifreq ifr;
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);
    ioctl(s, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_index;

    if(bind(s, (struct sockaddr * )&addr, sizeof(addr)) < 0){
        perror("ERROR: Socket Bindings Failure");
        close(s);
        exit(1);
    }

    return s;
}

//CAN Data Processing
int thousand(unsigned char buf[8], int serial_fd){
    rpm = (buf[0] << 8) | buf[1];
    tps = buf[6];

    char rpm_cmd[50];
    char tps_cmd[50];

    unsigned int rpm_proc = ((rpm+500)/100);
    snprintf(rpm_cmd, sizeof(rpm_cmd), "tacho.val=%d", rpm_proc);
    next_cmd(serial_fd, rpm_cmd);

    snprintf(tps_cmd, sizeof(tps_cmd), "tps.val=%d", tps);
    next_cmd(serial_fd, tps_cmd);
}

int thousand1(unsigned char buf[8], int serial_fd){
    double spdraw = (buf[2] << 8) | buf[3];
    spd = spdraw * spdconstant;

    afr = ((buf[6] << 8) | buf[7]) / 10;
}