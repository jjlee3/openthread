#pragma once

#include <memory>

class Listener;

class IOThreadMgr
{
public:
    using listener_t     = Listener;
    using listener_sptr  = std::shared_ptr<listener_t>;
    using listener_csptr = listener_sptr;

    virtual void SetListener(listener_csptr&&) = 0;
};

extern IOThreadMgr* g_ioThreadMgr;