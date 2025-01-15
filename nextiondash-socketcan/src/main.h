#ifndef MAIN_H
#define MAIN_H

//Dependencies
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>
#include <fcntl.h>
#include <errno.h>


//Interface Definitions
#define IF_CANBUS "vcan0" //interface name for socketcan testing interface
#define IF_NEXTION "/dev/ttyS0" //interface name for nextion testing interface

//Other Variable Definitions
#define SPD_CONSTANT (2.25 / 256) * 1.609344  // Constant to convert speed

//CAN Related Variables
unsigned char flagRecv, len;
unsigned char buf[8];

//Vehicle Telemetry Variables
unsigned int rpm, tps, gear;
double spd, afr, iat, clt, battery;

// Function to send commands to the Nextion display
int next_cmd(int serial_fd, const char *cmd);

// SocketCAN initialization function
int socketcan_init(const char *iface);

// CAN Data Processing functions
int thousand(unsigned char buf[8],  struct sp_port *serial);
int thousand1(unsigned char buf[8],  struct sp_port *serial);
int thousand3(unsigned char buf[8],  struct sp_port *serial);

#ENDIF MAIN_H


