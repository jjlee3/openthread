#pragma once

#include <functional>

typedef struct _GUID GUID;

// explicit specialization std::hash<T> for T = GUID
namespace std
{
    template <> struct hash<GUID>
    {
        size_t operator ()(const GUID& guid) const noexcept
        {
            const uint64_t* p = reinterpret_cast<const uint64_t*>(&guid);
            std::hash<uint64_t> hash;
            return hash(p[0]) ^ hash(p[1]);
        }
    };
}

#define GUID_FORMATA "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"
#define GUID_FORMATW L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}"
#define GUID_ARG(guid) guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]

namespace OpenThreadTalk
{
    class GuidHelper
    {
    public:
        // format GUID to string
        static int ToStringA(char* buffer, size_t sizeBuffer, const GUID& guid);

        // format GUID to string
        static int ToStringW(wchar_t* buffer, size_t sizeBuffer, const GUID& guid);
    };
}
