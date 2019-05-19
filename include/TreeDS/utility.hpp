#pragma once

#include <cstddef>     // std::ptr_diff_t
#include <functional>  // std::mem_fn()
#include <type_traits> // std::enable_if, std::is_convertible, std::is_constructible, std::void_t
#include <utility>     // std::forward(), std::declval

namespace md {

/*   ---   FORWARD DECLARATIONS   ---   */
template <typename>
class binary_node;

template <typename>
class nary_node;

template <typename>
class node_navigator;

namespace detail {
    struct empty_t {};
} // namespace detail

template <typename Node, typename Call, typename Test, typename Result>
Node* keep_calling(Node& from, Call call, Test test, Result result) {
    Node* prev = &from;
    Node* next = call(*prev);
    while (next != nullptr) {
        if (test(*prev, *next)) {
            return result(*prev, *next);
        }
        prev = next;
        next = call(*prev);
    }
    return prev; // Returns something only if test() succeeds.
}

/**
 * This function can be used to keep calling a specific lambda {@link Callable}. The passed type must be convertible to
 * a function that take a reference to constant node and returns a pointer to constant node. The best is to pass a
 * lambda so that it can be inlined.
 *
 * The case from == nullptr is correctly managed.
 */
template <typename Node, typename Callable>
Node* keep_calling(Node& from, Callable call) {
    Node* prev = &from;
    Node* next = call(*prev);
    while (next != nullptr) {
        prev = next;
        next = call(*prev);
    }
    return prev;
}

template <typename T>
std::size_t calculate_size(const binary_node<T>& node) {
    return 1
        + (node.get_left_child() != nullptr
               ? calculate_size(*node.get_left_child())
               : 0)
        + (node.get_right_child() != nullptr
               ? calculate_size(*node.get_right_child())
               : 0);
}

template <typename T>
std::size_t calculate_size(const nary_node<T>& node) {
    std::size_t size          = 1;
    const nary_node<T>* child = node.get_first_child();
    while (child != nullptr) {
        size += calculate_size(*child);
        child = child->get_next_sibling();
    }
    return size;
}

template <typename Node>
std::size_t calculate_arity(const Node& node, std::size_t max_expected_arity) {
    const Node* child = node.get_first_child();
    std::size_t arity = child
        ? child->get_following_siblings() + 1
        : 0u;
    while (child) {
        if (arity == max_expected_arity) {
            return arity;
        }
        arity = std::max(arity, calculate_arity(*child, max_expected_arity));
        child = child->get_next_sibling();
    }
    return arity;
}

template <
    typename T,
    typename Tuple,
    typename = void>
constexpr bool is_constructible_from_tuple = false;

template <
    typename T,
    typename Tuple>
constexpr bool is_constructible_from_tuple<
    T,
    Tuple,
    std::enable_if_t<
        std::is_invocable_v<
            std::make_from_tuple<T>,
            const Tuple&>>> = true;

template <typename Policy, typename = void>
constexpr bool is_tag_of_policy = false;

template <typename Policy>
constexpr bool is_tag_of_policy<
    Policy,
    std::void_t<
        decltype(
            std::declval<Policy>()
                .template get_instance<
                    binary_node<int>,
                    node_navigator<binary_node<int>>,
                    std::allocator<binary_node<int>>>(
                    std::declval<binary_node<int>*>(),
                    std::declval<node_navigator<binary_node<int>>>(),
                    std::declval<std::allocator<int>>())),
        Policy>> = true;

// Check method Type::get_resources() exists
template <typename Type, typename = void>
constexpr bool holds_resources = false;

template <typename Type>
constexpr bool holds_resources<
    Type,
    std::void_t<decltype(std::declval<Type>().get_resources())>> = true;

// Check if two types are instantiation of the same template
template <typename T, typename U>
constexpr bool is_same_template = std::is_same_v<T, U>;

template <
    template <typename...> class T,
    typename... A,
    typename... B>
constexpr bool is_same_template<T<A...>, T<B...>> = true;

template <
    template <auto...> class T,
    auto... A,
    auto... B>
constexpr bool is_same_template<T<A...>, T<B...>> = true;

} // namespace md
