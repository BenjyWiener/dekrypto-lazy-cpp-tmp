#pragma once

#include <iostream>

#include "char_pack.hpp"
#include "expression.hpp"
#include "generator.hpp"

template <operation Op>
struct op_to_char_pack;

template <>
struct op_to_char_pack<add> : char_pack::from_chars<' ', '+', ' '> {};

template <>
struct op_to_char_pack<subtract> : char_pack::from_chars<' ', '-', ' '> {};

template <>
struct op_to_char_pack<multiply> : char_pack::from_chars<' ', '*', ' '> {};

template <>
struct op_to_char_pack<divide> : char_pack::from_chars<' ', '/', ' '> {};

template <Expr E>
struct expr_to_char_pack : char_pack::from_chars<'('>
                        ::concat<expr_to_char_pack<typename E::lhs>>
                        ::concat<op_to_char_pack<E::op>>
                        ::concat<expr_to_char_pack<typename E::rhs>>
                        ::append<')'> {};

template <Term T>
struct expr_to_char_pack<T> : char_pack::int_to_char_pack<T::value> {};

template <Expr E>
struct solution_to_char_pack : expr_to_char_pack<E>::append<' ', '=', ' '>::concat<char_pack::int_to_char_pack<E::evaluated::num>> {};

template <generator::Generator Solutions, int N = 1, bool IsFirst = true>
struct solutions_to_char_pack : solution_to_char_pack<typename Solutions::value>::concat<solutions_to_char_pack<typename Solutions::next<>, N - 1, false>> {};

template <generator::Generator Solutions, int N>
requires(N > 0)
struct solutions_to_char_pack<Solutions, N, false> : char_pack::from_chars<'\n'>::concat<solution_to_char_pack<typename Solutions::value>>::concat<solutions_to_char_pack<typename Solutions::next<>, N - 1, false>> {};

template <generator::Generator Solutions, bool IsFirst>
struct solutions_to_char_pack<Solutions, 0, IsFirst> : char_pack::from_chars<> {};

template <generator::ExhaustedGenerator Solutions, int N>
requires(N > 0)
struct solutions_to_char_pack<Solutions, N, true> : char_pack::from_chars<'N', 'o', ' ', 's', 'o', 'l', 'u', 't', 'i', 'o', 'n', 's', ' ', 'f', 'o', 'u', 'n', 'd', '.'> {};

template <generator::ExhaustedGenerator Solutions, int N>
requires(N > 0)
struct solutions_to_char_pack<Solutions, N, false> : char_pack::from_chars<> {};
