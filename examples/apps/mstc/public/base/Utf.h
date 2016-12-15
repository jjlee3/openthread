#pragma once

#include <iterator>
#include <base/Exception.h>

namespace mstc
{
namespace base
{
class Utf
{
public:
    EXCEPT_BASE   (Failure);
    EXCEPT_DERIVED(OutOfUtf16Range               , Failure);
    EXCEPT_DERIVED(Utf8Length5                   , Failure);
    EXCEPT_DERIVED(Utf8Length6                   , Failure);
    EXCEPT_DERIVED(Utf8Length7                   , Failure);
    EXCEPT_DERIVED(IncompleteUtf8Sequence        , Failure);
    EXCEPT_DERIVED(InvalidUtf8Sequence           , Failure);
    EXCEPT_DERIVED(EndOfUtf16                    , Failure);
    EXCEPT_DERIVED(IncompleteUtf16Surrogates     , Failure);
    EXCEPT_DERIVED(InvalidUtf16TrailingSurrogates, Failure);
    EXCEPT_DERIVED(InvalidUtf16LeadingSurrogates , Failure);
    EXCEPT_DERIVED(InvalidUtf16                  , Failure);

    template <typename IIt, typename OIt>
    static OIt utf8to16(IIt first, IIt last, OIt dest)
    {
        for (; first != last;)
        {
            detail::utf8to16(first, last, dest);
        }

        return dest;
    }

    static void utf8to16(const std::string& utf8, std::wstring& utf16)
    {
        utf8to16(utf8.cbegin(), utf8.cend(), std::back_inserter(utf16));
    } // utf8to16

    static std::wstring utf8to16(const std::string& utf8)
    {
        std::wstring utf16;
        utf8to16(utf8.cbegin(), utf8.cend(), std::back_inserter(utf16));
        return utf16;
    } // utf8to16

    template <typename IIt, typename OIt>
    static OIt utf16to8(IIt first, IIt last, OIt dest)
    {
        for (; first != last;)
        {
            detail::utf16to8(first, last, dest);
        }

        return dest;
    }

    static void utf16to8(const std::wstring& utf16, std::string& utf8)
    {
        utf16to8(utf16.cbegin(), utf16.cend(), std::back_inserter(utf8));
    } // utf16to8

    static std::string utf16to8(const std::wstring& utf16)
    {
        std::string utf8;
        utf16to8(utf16.cbegin(), utf16.cend(), std::back_inserter(utf8));
        return utf8;
    } // utf16to8

protected:
    struct detail // one code point
    {
        template <typename IIt, typename OIt>
        static void utf8to16(IIt& it, IIt last, OIt& dest)
        {
            auto curr = *it++;

            if ((curr & 0x80) == 0)
            {
                *dest++ = curr;
            }
            else if ((curr & 0xe0) == 0xc0)
            {
                // two fields
                uint16_t field1 = curr & 0x1f;
                uint16_t field2 = utf8to16_mb(it, last);
                *dest++ = (field1 << 6) | field2;
            }
            else if ((curr & 0xf0) == 0xe0)
            {
                // three fields
                uint16_t field1 = curr & 0x0f;
                uint16_t field2 = utf8to16_mb(it, last);
                uint16_t field3 = utf8to16_mb(it, last);
                *dest++ = (field1 << 12) | (field2 << 6) | field3;
            }
            else if ((curr & 0xf8) == 0xf0)
            {
                // four fields - jjlee
                uint16_t field1 = curr & 0x07;
                uint16_t field2 = utf8to16_mb(it, last);
                uint16_t field3 = utf8to16_mb(it, last);
                uint16_t field4 = utf8to16_mb(it, last);
                uint32_t cp = (static_cast<uint32_t>(field1) << 18) |
                    (static_cast<uint32_t>(field2) << 12) |
                    (static_cast<uint32_t>(field3) << 6) |
                    static_cast<uint32_t>(field4);
                if (cp > 0x10FFFF) { MSTC_THROW_EXCEPTION(OutOfUtf16Range{}); }

                cp -= 0x10000;
                *dest++ = 0xD800 + static_cast<uint16_t>(cp >> 10);
                *dest++ = 0xDC00 + static_cast<uint16_t>(cp & 0x03FF);
            }
            else if ((curr & 0xfc) == 0xf8)
            {
                // five fields - jjlee
                it += 4;
                MSTC_THROW_EXCEPTION(Utf8Length5{});
            }
            else if ((curr & 0xfe) == 0xfc)
            {
                // six fields - jjlee
                it += 5;
                MSTC_THROW_EXCEPTION(Utf8Length6{});
            }
            else
            {
                // unknown fields - jjlee
                it += 6;
                MSTC_THROW_EXCEPTION(Utf8Length7{});
            }
        } // utf8to16

        template <typename IIt>
        static uint16_t utf8to16_mb(IIt& it, IIt last)
        {
            if (it == last) { MSTC_THROW_EXCEPTION(IncompleteUtf8Sequence{}); }

            auto curr = *it++;
            if ((curr & 0xc0) != 0x80) { MSTC_THROW_EXCEPTION(InvalidUtf8Sequence{}); }

            return curr & 0x3f;
        } // utf8to16_mb

        template <typename IIt, typename OIt>
        static void utf16to8(IIt& it, IIt last, OIt& dest)
        {
            if (it == last) { MSTC_THROW_EXCEPTION(EndOfUtf16{}); }

            auto curr = *it++;

            if (curr <= 0x007F)
            {
                *dest++ = static_cast<char>(curr);
            }
            else if (curr <= 0x07FF)
            {
                *dest++ = 0xC0 | static_cast<char>((curr & 0x07C0) >> 6);
                *dest++ = 0x80 | static_cast<char>(curr & 0x003F);
            }
            else if (curr <= 0xD7FF)
            {
                *dest++ = 0xE0 | static_cast<char>((curr & 0xF000) >> 12);
                *dest++ = 0x80 | static_cast<char>((curr & 0x0FC0) >> 6);
                *dest++ = 0x80 | static_cast<char>(curr & 0x003F);
            }
            else if (curr <= 0xDBFF)
            {
                uint32_t cp = (curr - 0xD800) * 0x0400 + 0x10000;
                if (it == last) { MSTC_THROW_EXCEPTION(IncompleteUtf16Surrogates{}); }

                uint16_t trail = *it++;

                if (trail <= 0xDBFF)
                {
                    MSTC_THROW_EXCEPTION(InvalidUtf16TrailingSurrogates{});
                }
                else if (trail <= 0xDFFF)
                {
                    cp |= (trail - 0xDC00);
                }
                else
                {
                    MSTC_THROW_EXCEPTION(InvalidUtf16TrailingSurrogates{});
                }

                *dest++ = 0xF0 | static_cast<char>((cp & 0x1C0000) >> 18);
                *dest++ = 0x80 | static_cast<char>((cp & 0x3F000) >> 12);
                *dest++ = 0x80 | static_cast<char>((cp & 0x0FC0) >> 6);
                *dest++ = 0x80 | static_cast<char>(cp & 0x003F);
            }
            else if (curr <= 0xDFFF)
            {
                MSTC_THROW_EXCEPTION(InvalidUtf16LeadingSurrogates{});
            }
            else if (curr <= 0xFFFF)
            {
                *dest++ = 0xE0 | static_cast<char>((curr & 0xF000) >> 12);
                *dest++ = 0x80 | static_cast<char>((curr & 0x0FC0) >> 6);
                *dest++ = 0x80 | static_cast<char>(curr & 0x003F);
            }
            else
            {
                MSTC_THROW_EXCEPTION(InvalidUtf16{});
            }
        } // utf16to8
    }; // detail
}; // Utf
} // namespace mstc
} // namespace base