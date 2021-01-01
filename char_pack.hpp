#pragma once

#include <concepts>

template <char... Cs>
struct char_pack {
    using self = char_pack<Cs...>;

    static constexpr size_t size = sizeof...(Cs);

private:
    template <typename>
    struct concat_impl;
    
    template <char... OtherCs>
    struct concat_impl<char_pack<OtherCs...>> : char_pack<Cs..., OtherCs...> {};

public:
    template <typename Other>
    using concat = concat_impl<Other>::self;

    template <char... OtherCs>
    struct append : char_pack<Cs..., OtherCs...> {};
    
    template <typename...>
    static constexpr const char to_string[size + 1] = {Cs..., '\0'};
};

template <auto I>
struct int_to_char_pack;

template <std::integral IT, IT I>
requires(I > 10)
struct int_to_char_pack<I> : int_to_char_pack<I / 10>::append<'0' + (I % 10)> {};

template <std::integral IT, IT I>
requires(I < 10 && I >= 0)
struct int_to_char_pack<I> : char_pack<'0' + (I % 10)> {};

template <std::integral IT, IT I>
requires(I < 0)
struct int_to_char_pack<I> : char_pack<'-'>::concat<int_to_char_pack<-I>> {};
