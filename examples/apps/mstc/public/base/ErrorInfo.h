#pragma once
// refer to boost exception

#include <string>

namespace mstc
{
namespace base
{
class error_info_base
{
protected:
    virtual ~error_info_base() = default;
};

template <class Tag, class T>
struct error_info : public error_info_base
{
public:
    using tag_t   = Tag;
    using value_t = T;

    error_info() = default;
    error_info(const error_info& value) = default;
    explicit error_info(const value_t& value) : value_{ value } {}

    value_t value_;
}; // ErrorInfo

using throw_file = error_info<struct throw_file_, const char*>;
using throw_line = error_info<struct throw_line_, int>;
using throw_function = error_info<struct throw_function_, const char*>;
using throw_args = error_info<struct throw_args_, std::string>;
using throw_class = error_info<struct throw_class_, const char*>;
using throw_class_args = error_info<struct throw_class_args_, std::string>;
using throw_error = error_info<struct throw_error_, std::string>;

} // namespace mstc
} // namespace base