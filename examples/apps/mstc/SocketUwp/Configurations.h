#pragma once

namespace SocketUwp
{
    enum class Protocol
    {
        TCP,
        UDP,
    };

    struct Configurations
    {
        Protocol protocol = Protocol::TCP;
    };

}

extern SocketUwp::Configurations g_configurations;
