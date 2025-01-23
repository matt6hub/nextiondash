#ifndef MAIN_H
#define MAIN_H

//Dependencies
#include <errno.h>
#include <fcntl.h>
#include <libserialport.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define IF_NEXTION "/dev/ttyS0" //Interface name for nextion testing interface
#define IF_MEMS "/dev/ttyUSB0" //Interface name for ECU Serial Port

#define ECU_ADDRESS 0x16
#define ECU_REPLY_EXPECTED_LENGTH 5

//MEMS19 Commands
unsigned char ecuSpecificInitCommand = 0x7C;
unsigned char ecuWokeResponse[3] = {0x55, 0x76, 0x83};
unsigned char ecuSpecificInitResponse[2] = {0x55, 0xE9};

bool ecuConnected = false;

//Vehicle Telemetry Variables
float rpm, tps, iacv, spd, afr, iat, clt, battery;

int next_cmd(struct sp_port *serial, const char *cmd);

#endif