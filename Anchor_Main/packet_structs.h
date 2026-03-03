#ifndef PACKET_STRUCTS_H
#define PACKET_STRUCTS_H

#include <Arduino.h>

#pragma pack(push,1)

typedef struct {
    uint16_t header;
    uint8_t anchor_id;
    uint8_t tag_type;
    uint16_t tag_id;
    uint32_t sequence;
    uint64_t rx_timestamp;
    int32_t sync_offset;
    uint16_t crc;
} UDPPacket;

#pragma pack(pop)

#endif