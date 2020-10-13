#pragma once

#include <iostream>

#include "expression.hpp"
#include "generator.hpp"

namespace detail {

struct non_constructable {
    non_constructable() = delete;
};

template <generator::Generator G>
struct print_all_impl : non_constructable {
    static inline void f() {
        std::cout << G::value::value << ' ';
        print_all_impl<typename G::next<>>::f();
    }
};

template <generator::ExhaustedGenerator G>
struct print_all_impl<G> : non_constructable {
    static inline void f() { std::cout << std::endl; }
};

template <generator::Generator G>
inline void print_all() { print_all_impl<G>::f(); }

template <generator::Generator G>
struct print_all_2d_impl : non_constructable {
    static inline void f() {
        print_all<typename G::value>();
        print_all_2d_impl<typename G::next<>>::f();
    }
};

template <generator::ExhaustedGenerator G>
struct print_all_2d_impl<G> : non_constructable {
    static inline void f() { std::cout << std::endl; }
};

template <generator::Generator G>
inline void print_all_2d() { print_all_2d_impl<G>::f(); }

template <generator::Generator G>
struct print_all_pairs_impl : non_constructable {
    static inline void f() {
        print_all<typename G::value::first>();
        print_all<typename G::value::second>();
        std::cout << std::endl;
        print_all_pairs_impl<typename G::next<>>::f();
    }
};

template <generator::ExhaustedGenerator G>
struct print_all_pairs_impl<G> : non_constructable {
    static inline void f() {}
};

template <generator::Generator G>
inline void print_all_pairs() { print_all_pairs_impl<G>::f(); }

template <operation Op>
struct show_op_impl;

template <>
struct show_op_impl<add> {
    static constexpr const char *value = " + ";
};

template <>
struct show_op_impl<subtract> {
    static constexpr const char *value = " - ";
};

template <>
struct show_op_impl<multiply> {
    static constexpr const char *value = " * ";
};

template <>
struct show_op_impl<divide> {
    static constexpr const char *value = " / ";
};

template <operation Op>
constexpr const char *show_op = show_op_impl<Op>::value;

template <Expr E>
struct print_expr_impl : non_constructable {
    static inline void f() {
        std::cout << '(';
        print_expr_impl<typename E::lhs>::f();
        std::cout << show_op<E::op>;
        print_expr_impl<typename E::rhs>::f();
        std::cout << ')';
    }
};

template <Term T>
struct print_expr_impl<T> : non_constructable {
    static inline void f() { std::cout << T::value; }
};

template <Expr E>
inline void print_expr() {
    print_expr_impl<E>::f();
    std::cout << " = " << E::evaluated::as_double << std::endl;
};

template <generator::Generator G>
struct print_all_exprs_impl : non_constructable {
    static inline void f() {
        print_expr<typename G::value>();
        print_all_exprs_impl<typename G::next<>>::f();
    }
};

template <generator::ExhaustedGenerator G>
struct print_all_exprs_impl<G> : non_constructable {
    static inline void f() {
        std::cout << std::endl;
    }
};

template <generator::Generator G>
inline void print_all_exprs() { print_all_exprs_impl<G>::f(); }

template <generator::Generator Solutions>
struct print_solution_impl : non_constructable {
    static inline void f() {
        print_expr<typename Solutions::value>();
    }
};

template <generator::ExhaustedGenerator Solutions>
struct print_solution_impl<Solutions> : non_constructable {
    static inline void f() {
        std::cout << "No solutions found." << std::endl;
    }
};

template <generator::Generator Solutions>
inline void print_solution() { print_solution_impl<Solutions>::f(); }

}  // namespace detail

using ::detail::print_all;
using ::detail::print_all_2d;
using ::detail::print_all_exprs;
using ::detail::print_all_pairs;
using ::detail::print_expr;
using ::detail::print_solution;
