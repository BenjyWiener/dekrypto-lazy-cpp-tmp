#pragma once

#include <type_traits>

namespace generator {

struct End {};

template <typename T>
concept ExhaustedGenerator = std::is_base_of_v<End, T>;

template <template <typename...> class TT>
struct is_next_template : std::true_type {};

template <typename T>
concept Generator = ExhaustedGenerator<T> ||
                    (requires { typename T::value; } && is_next_template<T::template next>::value);

template <typename T>
concept SingleGenerator = Generator<T> && !ExhaustedGenerator<T> && ExhaustedGenerator<typename T::next<>>;

template <auto V>
struct box {
    static constexpr decltype(V) value = V;
};

template <typename V>
struct single {
    using value = V;

    template <typename...>
    using next = End;
};

template <typename...>
struct generator_list;

template <typename V, typename... Rest>
struct generator_list<V, Rest...> {
    using value = V;

    template <typename...>
    using next = generator_list<Rest...>;
};

template <>
struct generator_list<> : End {};

template <auto... Vs>
using generator_box_list = generator_list<box<Vs>...>;

template <Generator G1, Generator G2>
struct concat {
    using value = G1::value;

    template <typename...>
    using next = concat<typename G1::next<>, G2>;
};

template <ExhaustedGenerator G1, Generator G2>
struct concat<G1, G2> : G2 {};

template <Generator G, typename NewV>
struct append {
    using value = G::value;

    template <typename...>
    using next = append<typename G::next<>, NewV>;
};

template <ExhaustedGenerator G, typename NewV>
struct append<G, NewV> : single<NewV> {};

template <Generator G, typename NewV>
struct prepend {
    using value = NewV;

    template <typename...>
    using next = G;
};

template <int N, Generator G>
struct take {
    using value = G::value;

    template <typename...>
    using next = take<N - 1, typename G::next<>>;
};

template <Generator G>
struct take<0, G> : End {};

}  // namespace generators
