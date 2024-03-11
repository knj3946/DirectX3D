#pragma once
#include <string>
#include <stdexcept>
#include <cstdio>

template<class char_t>
struct _FormatFunction {};

template<>
struct _FormatFunction<char>
{
    template<class ... Args>
    int operator()(Args&&... args)
    {
        return std::snprintf(std::forward<Args>(args)...);
    }
};

template<>
struct _FormatFunction<wchar_t>
{
    template<class ... Args>
    int operator()(Args&&... args)
    {
        return std::swprintf(std::forward<Args>(args)...);
    }
};

template<class char_t, typename... Args>
std::basic_string<char_t> format(const std::basic_string<char_t>& fmt, Args&&... args)
{
    if (fmt.size() == 0 || sizeof...(Args) == 0)
        return fmt;

    _FormatFunction<char_t> func{};

    int size_s = func(nullptr, 0, fmt.c_str(), args...);
    if (size_s <= 0)
        throw std::runtime_error("Error during formatting.");

    // null-terminator 가 포함되지 않은 사이즈
    auto size = static_cast<size_t>(size_s);

    // ret의 실제 버퍼에는 null-terminator가 포함된다
    std::basic_string<char_t> ret(size, 0);

    // 버퍼 사이즈는 null-terminator 를 포함한 사이즈 (size + 1)를 전달해야 한다.
    func(&ret[0], (size + 1) * sizeof(char_t), fmt.c_str(), args...);
    return ret;
}