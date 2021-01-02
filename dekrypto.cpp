#include <iostream>

#include "expression.hpp"
#include "generator.hpp"
#include "generator_utils.hpp"
#include "output.hpp"

using namespace generator;

template <typename T1, typename T2>
struct pair {
    using first = T1;
    using second = T2;
};

template <Generator R, Generator L = End>
struct halves {
    using value = pair<append<L, typename R::value>,
                       typename R::next<>>;

    template <typename...>
    using next = halves<typename R::next<>, append<L, typename R::value>>;
};

template <Generator R, ExhaustedGenerator L>
struct halves<R, L> {
    using value = pair<single<typename R::value>,
                       typename R::next<>>;

    template <typename...>
    using next = halves<typename R::next<>, single<typename R::value>>;
};

template <SingleGenerator R, Generator L>
struct halves<R, L> : End {};

template <typename First, Generator G>
struct make_pairs {
    using value = pair<First, typename G::value>;

    template <typename...>
    using next = make_pairs<First, typename G::next<>>;
};

template <typename First, ExhaustedGenerator G>
struct make_pairs<First, G> : End {};

template <Generator G1, Generator G2>
struct combinations : concat<make_pairs<typename G1::value, G2>, combinations<typename G1::next<>, G2>> {};

template <SingleGenerator G1, Generator G2>
struct combinations<G1, G2> : make_pairs<typename G1::value, G2> {};

template <Generator G>
struct insert_ops : concat<generator_list<
                               expr<add, typename G::value::first, typename G::value::second>,
                               expr<subtract, typename G::value::first, typename G::value::second>,
                               expr<multiply, typename G::value::first, typename G::value::second>,
                               expr<divide, typename G::value::first, typename G::value::second>>,
                           insert_ops<typename G::next<>>> {};

template <SingleGenerator G>
struct insert_ops<G> : generator_list<
                           expr<add, typename G::value::first, typename G::value::second>,
                           expr<subtract, typename G::value::first, typename G::value::second>,
                           expr<multiply, typename G::value::first, typename G::value::second>,
                           expr<divide, typename G::value::first, typename G::value::second>> {};

template <Generator G>
struct expressions;

template <typename P>
struct expressions_inner : insert_ops<combinations<
                               expressions<typename P::first>,
                               expressions<typename P::second>>> {};

template <Generator G>
struct expressions : flatten<map<halves<G>, expressions_inner>> {};

template <SingleGenerator G>
struct expressions<G> : single<term<G::value::value>> {};

template <int Goal>
struct is_solution_predicate {
    template <Expr E>
    using apply = box<E::evaluated::num == Goal && E::evaluated::denom == 1>;
};

template <Generator Cards, int Goal>
using solve = filter<flatten<map<permute<Cards>,
                                 expressions>>,
                     is_solution_predicate<Goal>::template apply>;

int main() {
    using solutions = solve<generator_box_list<
#include "./input/cards.txt"
                                >,
#include "./input/goal.txt"
                            >;

    std::cout << solutions_to_char_pack<solutions>::to_string << '\n';

    return 0;
}
