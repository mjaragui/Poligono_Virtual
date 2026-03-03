#include "config.h"
#include "packet_structs.h"
#include "ethernet_manager.h"
#include "udp_sender.h"
#include "uwb_driver.h"
#include "sync_manager.h"
#include "ring_buffer.h"
#include "crc16.h"

static uint8_t uwb_rx_temp[256];
static uint16_t uwb_len = 0;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== ANCHOR START ===");

    // 1️⃣ Ethernet (PoE)
    initEthernet();
    initUDP();

    // 2️⃣ Ring buffer alto tráfico
    ringInit();

    // 3️⃣ UWB DW3000 real
    uwbInit();

    // 4️⃣ Sync manager
    initSync();

    Serial.println("=== SYSTEM READY ===");
}

void loop()
{
    // =========================
    // 1️⃣ PROCESAR SYNC (Master o Slave)
    // =========================
    processSync();

    // =========================
    // 2️⃣ SI HAY PACKET UWB
    // =========================
    if (uwbPacketAvailable())
    {
        uwbGetPacket(uwb_rx_temp, &uwb_len);
        uint64_t ts = uwbGetRxTimestamp();

        // Guardar en buffer circular
        ringPush(uwb_rx_temp, uwb_len, ts);
    }

    // =========================
    // 3️⃣ PROCESAR BUFFER Y ENVIAR UDP
    // =========================
    UWBFrame frame;

    while (ringPop(&frame))
    {
        if (frame.len < 4) continue; // evitar basura

        // ---- Interpretación básica ----
        uint16_t tag_id = (frame.data[0] << 8) | frame.data[1];
        uint8_t tag_type = frame.data[2];
        uint8_t sequence = frame.data[3];

        // ---- Construir paquete UDP binario ----
        UDPPacket packet;

        packet.header      = 0xAA55;
        packet.anchor_id   = ANCHOR_ID;
        packet.tag_type    = tag_type;
        packet.tag_id      = tag_id;
        packet.sequence    = sequence;
        packet.rx_timestamp = frame.timestamp + getSyncOffset();
        packet.sync_offset  = getSyncOffset();

        packet.crc = crc16((uint8_t*)&packet, sizeof(UDPPacket) - 2);

        sendUDPPacket(packet);
    }
}
