#include "detail/pch.h"
#include <utility>
#include "detail/Clients.h"

void
Clients::join(
    join_clients&&              joinClients,
    const notify_left_function& f)
{
    for (auto& i : joinClients)
    {
        auto s = static_cast<SOCKET>(i);
        if (s == mstc::network::Ipv6::invalid()) { continue; }

        clientsMap_[s] = ClientService{std::move(i), f};
        clientsMap_[s].start();
    }
}

void
Clients::left(
    const left_clients& clients)
{
    for (const auto& i : clients)
    {
        clientsMap_.erase(i);
    }
}
