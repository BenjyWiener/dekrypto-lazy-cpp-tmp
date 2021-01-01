#pragma once

#include <iostream>

#include "char_pack.hpp"
#include "expression.hpp"
#include "generator.hpp"

template <operation Op>
struct op_to_char_pack;

template <>
struct op_to_char_pack<add> : char_pack<' ', '+', ' '> {};

template <>
struct op_to_char_pack<subtract> : char_pack<' ', '-', ' '> {};

template <>
struct op_to_char_pack<multiply> : char_pack<' ', '*', ' '> {};

template <>
struct op_to_char_pack<divide> : char_pack<' ', '/', ' '> {};

template <Expr>
struct expr_to_char_pack;

template <Expr E>
struct expr_to_char_pack : char_pack<'('>::concat<
                           typename expr_to_char_pack<typename E::lhs>::self>::concat<
                           typename op_to_char_pack<E::op>::self>::concat<
                           typename expr_to_char_pack<typename E::rhs>::self>::append<
                           ')'>::self {};

template <Term T>
struct expr_to_char_pack<T> : int_to_char_pack<T::value>::self {};

template <Expr E>
struct solution_to_char_pack : expr_to_char_pack<E>::append<' ', '=', ' '>::concat<typename int_to_char_pack<E::evaluated::num>::self> {};

template <generator::Generator Solutions, int N = 1, bool IsFirst = true>
struct solutions_to_char_pack : solution_to_char_pack<typename Solutions::value>::concat<typename solutions_to_char_pack<typename Solutions::next<>, N - 1, false>::self> {};

template <generator::Generator Solutions, int N>
requires(N > 0)
struct solutions_to_char_pack<Solutions, N, false> : char_pack<'\n'>::concat<typename solution_to_char_pack<typename Solutions::value>::self>::concat<typename solutions_to_char_pack<typename Solutions::next<>, N - 1, false>::self> {};

template <generator::Generator Solutions, bool IsFirst>
struct solutions_to_char_pack<Solutions, 0, IsFirst> : char_pack<> {};

template <generator::ExhaustedGenerator Solutions, int N>
requires(N > 0)
struct solutions_to_char_pack<Solutions, N, true> : char_pack<'N', 'o', ' ', 's', 'o', 'l', 'u', 't', 'i', 'o', 'n', 's', ' ', 'f', 'o', 'u', 'n', 'd', '.'> {};

template <generator::ExhaustedGenerator Solutions, int N>
requires(N > 0)
struct solutions_to_char_pack<Solutions, N, false> : char_pack<> {};
