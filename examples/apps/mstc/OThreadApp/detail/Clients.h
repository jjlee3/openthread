#pragma once

#include <winsock2.h>
#include <unordered_map>
#include <unordered_set>
#include <network/Ipv6.h>
#include "detail/NotifyFun.h"
#include "detail/ClientService.h"

// server stores all ClientService here
class Clients
{
public:
    using socket_t     = mstc::network::Ipv6;
    using join_clinets = std::vector<socket_t>;
    using left_clients = std::vector<SOCKET>;

    void join(join_clinets&&, const notify_left_function&);
    void left(const left_clients&);

protected:
    using clients_map_t = std::unordered_map<SOCKET, ClientService>;

    clients_map_t clientsMap_;
};