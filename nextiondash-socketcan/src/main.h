#ifndef MAIN_H
#define MAIN_H

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
int thousand(unsigned char buf[8], int serial_fd);
int thousand1(unsigned char buf[8], int serial_fd);
int thousand3(unsigned char buf[8], int serial_fd);


