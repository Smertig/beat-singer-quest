#pragma once

#include <utility>
#include <type_traits>

namespace utils {

template <class T, class U = T>
auto scope_exchange(T& ref, U&& new_value) {
    static_assert(std::is_nothrow_assignable_v<T&, T&&>);

    class guard_t {
        T& m_ref;
        T m_old_value;

    public:
        /* implicit */ guard_t(T& ref, T old_value) : m_ref(ref), m_old_value(std::move(old_value)) {
            // nothing
        }

        guard_t(const guard_t&) = delete;
        guard_t(guard_t&&) = delete;
        guard_t& operator=(const guard_t&) = delete;
        guard_t& operator=(guard_t&&) = delete;
        ~guard_t() {
            m_ref = std::move(m_old_value);
        }
    };

    auto old_value = std::exchange(ref, std::forward<U>(new_value));
    return guard_t{ ref, std::move(old_value) };
}

} // namespace utils
