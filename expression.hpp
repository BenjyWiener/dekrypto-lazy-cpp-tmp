#pragma once

#include <numeric>
#include <type_traits>

template <int N, int D = 1>
struct rational {
    static constexpr int num = N / std::gcd(N, D);
    static constexpr int denom = D / std::gcd(N, D);
    static constexpr double as_double = (double)num / (double)denom;
};

template <int D>
struct rational<0, D> {
    static constexpr int num = 0;
    static constexpr int denom = 1;
    static constexpr double as_double = (double)num / (double)denom;
};

template <>
struct rational<0, 0> {
    static constexpr int num = 0;
    static constexpr int denom = 0;
};

template <typename>
struct is_rational : std::false_type {};

template <int N, int D>
struct is_rational<rational<N, D>> : std::true_type {};

template <typename T>
concept Rational = is_rational<T>::value;

enum operation {
    add,
    subtract,
    multiply,
    divide
};

template <operation, Rational, Rational>
struct apply_op;

template <Rational LHS, Rational RHS>
struct apply_op<add, LHS, RHS> {
    using value = rational<(LHS::num * RHS::denom) + (RHS::num * LHS::denom), LHS::denom * RHS::denom>;
};

template <Rational LHS, Rational RHS>
struct apply_op<multiply, LHS, RHS> {
    using value = rational<LHS::num * RHS::num, LHS::denom * RHS::denom>;
};

template <Rational LHS, Rational RHS>
struct apply_op<subtract, LHS, RHS> {
    using value = rational<(LHS::num * RHS::denom) - (RHS::num * LHS::denom), LHS::denom * RHS::denom>;
};

template <Rational LHS, Rational RHS>
struct apply_op<divide, LHS, RHS> {
    using value = rational<LHS::num * RHS::denom, LHS::denom * RHS::num>;
};

enum class expression_type {
    term,
    compound
};

template <expression_type, auto, typename...>
struct expression;

template <typename>
struct is_expr : std::false_type {};

template <expression_type ExprType, auto FirstParam, typename... Other>
struct is_expr<expression<ExprType, FirstParam, Other...>> : std::true_type {};

template <typename>
struct is_term : std::false_type {};

template <int V>
struct is_term<expression<expression_type::term, V>> : std::true_type {};

template <typename T>
concept Expr = is_expr<T>::value;

template <typename T>
concept Term = Expr<T> && is_term<T>::value;

template <int V>
struct expression<expression_type::term, V> {
    static constexpr int value = V;
    using evaluated = rational<V>;
};

template <int V>
using term = expression<expression_type::term, V>;

template <operation Op, Expr LHS, Expr RHS>
struct expression<expression_type::compound, Op, LHS, RHS> {
    static constexpr operation op = Op;
    using lhs = LHS;
    using rhs = RHS;
    using evaluated = apply_op<op, typename lhs::evaluated, typename rhs::evaluated>::value;
};

template <operation Op, Expr LHS, Expr RHS>
using expr = expression<expression_type::compound, Op, LHS, RHS>;
