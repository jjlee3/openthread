#include "pch/pch.h"
#include <tcKernel/Lang.h>

LANGID mstc::tckernel::Lang::s_langId_ = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

void
mstc::tckernel::Lang::initId()
{
    setId(::GetUserDefaultLangID());
}

void
mstc::tckernel::Lang::setId(
    LANGID langId)
{
    if (s_langId_ == langId) { return; }

    s_langId_ = langId;
}