#pragma once

#ifndef _OT_FAILURE_H_
#define _OT_FAILURE_H_

#include <stdexcept>

namespace ot
{
    class KFailureException : public std::runtime_error
    {
    public:
        using base_t = std::runtime_error;
        using base_t::base_t;
    };
}

#endif // _OT_FAILURE_H_