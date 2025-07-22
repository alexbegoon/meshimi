#pragma once

typedef enum {
    MeshimiEventMode = 0,
    MeshimiEventSpreadingFactor,
    MeshimiEventBandwidth,
    MeshimiEventCodingRate,
    MeshimiEventLDRO,
    MeshimiEventFrequency,

    // Custom events for Meshtastic
    MeshimiEventMeshtasticNewMessage,
    MeshimiEventMeshtasticNewMessageReceived,
} MeshimiCustomEvent;