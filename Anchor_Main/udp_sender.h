#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include "packet_structs.h"

void initUDP();
void sendUDPPacket(UDPPacket &packet);

#endif