#pragma once

#include <Windows.h>
#include <string>

namespace mstc
{
namespace tckernel
{
class Lang
{
public:
    static void initId();
    static void setId(LANGID langId);

    static void set(const std::wstring&);
    static LANGID getId() { return s_langId_; }
    static const std::wstring& get() { return s_lang_; }

protected:
    static LANGID       s_langId_;
    static std::wstring s_lang_;
}; // Lang
} // namespace tckernel
} // namespace mstc
