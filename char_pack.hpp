#pragma once

#include <concepts>
#include <type_traits>

namespace char_pack {

template <char... Cs>
struct from_chars;

template <typename>
struct is_char_pack : std::false_type {};

template <typename T>
requires(std::is_class_v<typename T::self>)
struct is_char_pack<T> : is_char_pack<typename T::self> {};

template <char... Cs>
struct is_char_pack<from_chars<Cs...>> : std::true_type {};

template <typename T>
concept CharPack = is_char_pack<T>::value;

template <char... Cs>
struct from_chars {
    using self = from_chars<Cs...>;

    static constexpr size_t size = sizeof...(Cs);

   private:
    template <CharPack Other>
    struct concat_impl : concat_impl<typename Other::self> {};

    template <char... OtherCs>
    struct concat_impl<from_chars<OtherCs...>> : from_chars<Cs..., OtherCs...> {};

   public:
    template <CharPack Other>
    using concat = concat_impl<Other>;

    template <char... OtherCs>
    using append = from_chars<Cs..., OtherCs...>;
    
    static constexpr const char to_string[size + 1] = {Cs..., '\0'};
};

template <auto I>
struct int_to_char_pack;

template <std::integral IT, IT I>
requires(I >= 10)
struct int_to_char_pack<I> : int_to_char_pack<I / 10>::append<'0' + (I % 10)> {};

template <std::integral IT, IT I>
requires(I < 10 && I >= 0)
struct int_to_char_pack<I> : from_chars<'0' + (I % 10)> {};

template <std::integral IT, IT I>
requires(I < 0)
struct int_to_char_pack<I> : from_chars<'-'>::concat<int_to_char_pack<-I>> {};

}  // namespace char_pack
