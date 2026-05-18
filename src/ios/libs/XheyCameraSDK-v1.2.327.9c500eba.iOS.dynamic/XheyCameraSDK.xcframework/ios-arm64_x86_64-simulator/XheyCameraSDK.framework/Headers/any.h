#pragma once

#include <cassert>
#include <memory>
#include <typeinfo>

#ifdef _WIN32
/**
 * 1. msvc expand __VA_ARGS__ as a single argument, instead of being broken down to multiple ones
 * reference: https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
 * 2. msvc leave a extra ',' if __VA_ARGS__ is none
 * reference:
 * https://social.msdn.microsoft.com/Forums/vstudio/en-US/ef0faf06-e0ee-454b-9742-5f95277295b9/problem-with-vaargs?forum=vcgeneral
 */
#define EXPAND(x) x
#define GET_SOME_DATA(s, ...) EXPAND(GET_SOME_DATA_(s, ##__VA_ARGS__, 4, 4, 3, 3, 2, 2, 1))
#else
#define GET_SOME_DATA(s, t1, v1, ...) GET_SOME_DATA_(s, t1, v1, ##__VA_ARGS__, 4, 4, 3, 3, 2, 2, 1)
#endif

#define GET_SOME_DATA_(s, t1, v1, t2, v2, t3, v3, t4, v4, n, ...) GET_SOME_DATA##n(s, t1, v1, t2, v2, t3, v3, t4, v4)
#define GET_SOME_DATA1(s, t1, v1, ...)                                                                                           \
    t1 v1;                                                                                                                       \
    s.GetAll(v1);
#define GET_SOME_DATA2(s, t1, v1, t2, v2, ...)                                                                                   \
    t1 v1;                                                                                                                       \
    t2 v2;                                                                                                                       \
    s.GetAll(v1, v2);
#define GET_SOME_DATA3(s, t1, v1, t2, v2, t3, v3, ...)                                                                           \
    t1 v1;                                                                                                                       \
    t2 v2;                                                                                                                       \
    t3 v3;                                                                                                                       \
    s.GetAll(v1, v2, v3);
#define GET_SOME_DATA4(s, t1, v1, t2, v2, t3, v3, t4, v4, ...)                                                                   \
    t1 v1;                                                                                                                       \
    t2 v2;                                                                                                                       \
    t3 v3;                                                                                                                       \
    t4 v4;                                                                                                                       \
    s.GetAll(v1, v2, v3, v4);

namespace xhey {
namespace camera {

namespace internal {

struct AnyValue {
    virtual ~AnyValue() {}
    virtual const std::type_info &GetType() const = 0;
    virtual std::shared_ptr<AnyValue> Clone() const = 0;
};

template <class T>
struct SpecificValue : public AnyValue {
    SpecificValue(const T &o) : obj(o) {}
    SpecificValue(const SpecificValue &o) : obj(o.obj) {}
    virtual ~SpecificValue() {}

    virtual const std::type_info &GetType() const { return typeid(T); }
    virtual std::shared_ptr<AnyValue> Clone() const { return std::shared_ptr<AnyValue>(new SpecificValue<T>(obj)); }

    T obj;
};

} // namespace internal

class Any {
public:
    Any() {}

    template <class T>
    explicit Any(const T &o) : ptr(new internal::SpecificValue<T>(o)) {}

    Any(const Any &o) : ptr(o.ptr == nullptr ? nullptr : o.ptr->Clone()) {}

    Any(Any &&o) : ptr(o.ptr) {}

    Any &operator=(const Any &o) {
        ptr = o.ptr->Clone();
        return *this;
    }

    void Reset() { ptr = nullptr; }

    template <class T>
    const T &Cast() const {
        auto real_ptr = std::dynamic_pointer_cast<internal::SpecificValue<T>>(ptr);
        assert(real_ptr != nullptr);
        return real_ptr->obj;
    }

    const std::type_info &Type() const {
        assert(ptr != nullptr);
        return ptr->GetType();
    }

    bool HasValue() const { return ptr != nullptr; }

private:
    std::shared_ptr<internal::AnyValue> ptr;
};

class Some {
public:
    Some() {}

    template <class... Args>
    explicit Some(Args... o) : ptr(new internal::SpecificValue<std::tuple<Args...>>(std::make_tuple(o...))) {}

    Some(const Some &o) : ptr(o.ptr == nullptr ? nullptr : o.ptr->Clone()) {}

    Some(Some &&o) : ptr(o.ptr) {}

    Some &operator=(const Some &o) {
        ptr = o.ptr->Clone();
        return *this;
    }

    void Reset() { ptr.reset(); }

    template <class... Args>
    bool GetAll(Args &...v) const {
        assert(ptr != nullptr);
        assert(ptr->GetType() == typeid(std::tuple<Args...>));
        auto real_ptr = std::dynamic_pointer_cast<internal::SpecificValue<std::tuple<Args...>>>(ptr);
        tie(v...) = real_ptr->obj;
        return true;
    }

    template <class T>
    const T &GetOne() const {
        auto real_ptr = std::dynamic_pointer_cast<internal::SpecificValue<std::tuple<T>>>(ptr);
        assert(real_ptr != nullptr);
        return std::get<0>(real_ptr->obj);
    }

    bool HasValue() const { return ptr != nullptr; }

private:
    std::shared_ptr<internal::AnyValue> ptr;
};

} // namespace camera
} // namespace xhey
