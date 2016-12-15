#pragma once

// connection init handshake
#define HANDSHAKE_COMMAND   ""

class Command
{
public:
    static constexpr int handshakeCommandLEN =
        sizeof(HANDSHAKE_COMMAND) - 1;
};