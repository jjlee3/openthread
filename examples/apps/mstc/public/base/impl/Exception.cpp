#include "pch/pch.h"
#include <base/Exception.h>

const char* mstc::base::Exception::what() const
{
    return what_.c_str();
}

std::string mstc::base::Exception::diagnose() const
{
    std::string info;

    if (auto pfile = get_info<throw_file>())
    {
        if (!info.empty()) { info += " "; }
        info += *pfile;
    }

    if (auto pline = get_info<throw_line>())
    {
        info += "(" + std::to_string(*pline) + ")";
    }

    if (auto pfunc = get_info<throw_function>())
    {
        if (!info.empty()) { info += " "; }
        info += *pfunc;
    }

    if (auto pargs = get_info<throw_args>())
    {
        info += "(" + *pargs + ")";
    }

    if (auto pcls = get_info<throw_class>())
    {
        if (!info.empty()) { info += " "; }
        info += std::string("throwing ") + *pcls;
    }

    if (auto pclsArgs = get_info<throw_class_args>())
    {
        info += "(";
        info += *pclsArgs;
        info += ")";
    }

    if (auto perror = get_info<throw_error>())
    {
        if (!info.empty()) { info += " : "; }
        info += *perror;
    }

    if (!what_.empty())
    {
        if (!info.empty()) { info += " - "; }
        info += what_;
    }

    return info;
}