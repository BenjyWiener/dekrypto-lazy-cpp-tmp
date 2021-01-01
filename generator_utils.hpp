#pragma once

#include "generator.hpp"

namespace generator {

// Creates a Generator that emits variations of R with V
// inserted at each possible location.
//
// Example:
//    interleave<generator_list<A, B, C>,
//               X>
//    -> {X, A, B, C}, {A, X, B, C}, {A, B, X, C}, {A, B, C, X}
template <Generator R, typename V, Generator L = End>
struct interleave {
    using value = concat<append<L, V>, R>;

    template <typename...>
    using next = interleave<typename R::next<>, V,
                            append<L, typename R::value>>;
};

template <Generator R, typename V, ExhaustedGenerator L>
struct interleave<R, V, L> {
    using value = prepend<R, V>;

    template <typename...>
    using next = interleave<typename R::next<>, V,
                            single<typename R::value>>;
};

template <ExhaustedGenerator R, typename V, Generator L>
struct interleave<R, V, L> {
    using value = append<L, V>;

    template <typename...>
    using next = End;
};

template <ExhaustedGenerator R, typename V, ExhaustedGenerator L>
struct interleave<R, V, L> {
    using value = single<V>;

    template <typename...>
    using next = End;
};

// Creates a Generator that emits values from the
// Generators emitted by ParentG.
//
// Example:
//    flatten<generator_list<generator_list<A, B, C>,
//                           generator_list<D, E, F>,
//                           generator_list<G, H, I>>>
//    -> A, B, C, D, E, F, G, H, I
template <Generator ParentG, Generator CurrentG = End>
struct flatten {
    using value = CurrentG::value;

    template <typename...>
    using next = flatten<ParentG, typename CurrentG::next<>>;
};

template <Generator ParentG, ExhaustedGenerator CurrentG>
struct flatten<ParentG, CurrentG> : flatten<typename ParentG::next<>, typename ParentG::value> {};

template <ExhaustedGenerator ParentG, ExhaustedGenerator CurrentG>
struct flatten<ParentG, CurrentG> : End {};

// Creates a Generator that emits F<value> for each value
// emitted by G.
//
// Example:
//    map<generator_list<A, B, C>,
//        F>
//    -> F<A>, F<B>, F<C>
template <Generator G, template <typename> class F>
struct map {
    using value = F<typename G::value>;

    template <typename...>
    using next = map<typename G::next<>, F>;
};

template <ExhaustedGenerator G, template <typename> class F>
struct map<G, F> : End {};

namespace detail {

template <typename Head>
struct interleave_head {
    template <Generator G>
    using apply = interleave<G, Head>;
};

}  // namespace detail

// Creates a Generator that emits every permutation of G.
//
// Example:
//    permute<generator_list<A, B, C>>
//    -> {A, B, C}, {B, A, C}, {B, C, A},
//       {A, C, B}, {C, A, B}, {C, B, A}
template <Generator G>
struct permute : flatten<map<permute<typename G::next<>>,
                             detail::interleave_head<typename G::value>::template apply>> {};

template <SingleGenerator G>
struct permute<G> : single<single<typename G::value>> {};

template <ExhaustedGenerator G>
struct permute<G> : End {};

// Creates a Generator that emits all values emitted by
// G for which Predicate<value>::value is true.
//
// Example:
//    filter<generator_list<A, B *, C *, D, E, F *>,
//           std::is_pointer>
//    -> B *, C *, F *
template <Generator G, template <typename> class Predicate>
struct filter : filter<typename G::next<>, Predicate> {};

template <Generator G, template <typename> class Predicate>
requires(Predicate<typename G::value>::value)
struct filter<G, Predicate> {
    using value = G::value;

    template <typename...>
    using next = filter<typename G::next<>, Predicate>;
};

template <ExhaustedGenerator G, template <typename> class Predicate>
struct filter<G, Predicate> : End {};

}  // namespace generators
