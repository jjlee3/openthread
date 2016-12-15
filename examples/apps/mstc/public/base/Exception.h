#pragma once
// refer to boost exception

#include <string>
#include <utility>
#include <memory>
#include <exception>
#include <typeindex>
#include <unordered_map>
#include <base/ErrorInfo.h>

namespace mstc
{
namespace base
{
class Exception : public virtual std::exception
{
public:
    virtual const char* what() const override;
    const std::string& whats() const { return what_; }
    std::string diagnose() const;

protected:
    using this_t = Exception;
    using type_index = std::type_index;
    using error_info_base_sptr = std::shared_ptr<error_info_base>;
    using error_info_base_csptr = error_info_base_sptr;
    using map_t = std::unordered_map<type_index, error_info_base_csptr>;

    explicit Exception(std::string what, const char* throwCls) :
        what_{std::move(what)}
    {
        set(typeid(throw_class), std::make_shared<throw_class>(throwCls));
    }

    void set(const type_index& tyIndex, error_info_base_csptr v)
    {
        map_[tyIndex] = std::move(v);
    }

    error_info_base_csptr get(const type_index& tyIndex) const
    {
        auto it = map_.find(tyIndex);
        if (it == map_.end()) { return error_info_base_csptr(); }

        return it->second;
    }

    template <class ErrorInfo>
    Exception& set_info(const ErrorInfo& i)
    {
        const auto& tyInfo = typeid(ErrorInfo);
        set(tyInfo, std::make_shared<ErrorInfo>(i));
        return *this;
    }

    template <class ErrorInfo>
    const typename ErrorInfo::value_t* get_info() const
    {
        const auto& tyInfo = typeid(ErrorInfo);
        auto csptr = get(tyInfo);
        if (!csptr) { return nullptr; }

        return &static_cast<ErrorInfo*>(csptr.get())->value_;
    }

    // prefer UTF-8
    std::string what_;
    map_t       map_;
}; // Exception

#define SET_OPERATOR(ExceptCLASS)                           \
    /* derived class please return a covariant type!!! */   \
    template <class ErrorInfo>                              \
    ExceptCLASS& operator <<(const ErrorInfo& i)            \
    {                                                       \
        set_info(i);                                        \
        return *this;                                       \
    }

#define EXCEPT_DERIVED(ExceptCLASS, ExceptBASE)                     \
    struct ExceptCLASS : public ExceptBASE                          \
    {                                                               \
        using base_t = ExceptBASE;                                  \
    public:                                                         \
        explicit ExceptCLASS(std::string what = std::string(),      \
            const char* throwCls = typeid(ExceptCLASS).name()) :    \
            base_t{std::move(what), throwCls}                       \
        {                                                           \
        }                                                           \
                                                                    \
        SET_OPERATOR(ExceptCLASS)                                   \
    }

#define EXCEPT_BASE(ExceptCLASS)                        \
    EXCEPT_DERIVED(ExceptCLASS, mstc::base::Exception)

/// x : throw class
#define MSTC_THROW_EXCEPTION(x)                             \
    throw(x)                                                \
    << mstc::base::throw_function(__FUNCTION__)             \
    << mstc::base::throw_file(__FILE__)                     \
    << mstc::base::throw_line(static_cast<int>(__LINE__))

/// x : throw class
/// args : function arguments
#define MSTC_THROW_EXCEPTION_WITH_ARGS(x, args)             \
    throw(x)                                                \
    << mstc::base::throw_function(__FUNCTION__)             \
    << mstc::base::throw_args(args)                         \
    << mstc::base::throw_file(__FILE__)                     \
    << mstc::base::throw_line(static_cast<int>(__LINE__))

} // namespace mstc
} // namespace base