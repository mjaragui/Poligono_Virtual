#include "udp_sender.h"
#include "config.h"
#include <WiFiUdp.h>

WiFiUDP udp;

void initUDP()
{
    udp.begin(UDP_PORT);
}

void sendUDPPacket(UDPPacket &packet)
{
    udp.beginPacket(SERVER_IP, UDP_PORT);
    udp.write((uint8_t*)&packet, sizeof(UDPPacket));
    udp.endPacket();
}