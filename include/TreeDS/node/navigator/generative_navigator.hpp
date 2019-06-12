#pragma once

#include <type_traits>

#include <TreeDS/node/multiple_node_pointer.hpp>
#include <TreeDS/node/navigator/node_pred_navigator.hpp>
#include <TreeDS/utility.hpp>

namespace md {

template <typename NodeAllocator, typename Predicate, typename TargetNodePtr, typename GeneratedNodePtr>
class generative_pred_navigator
        : public node_pred_navigator<
              multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>,
              Predicate> {

    /*   ---   FRIENDS   ---   */
    template <typename>
    friend class node_navigator;

    using node_ptrs_t = multiple_node_pointer<TargetNodePtr, GeneratedNodePtr>;

    /*   ---   ATTRIBUTES   ---   */
    NodeAllocator& allocator;

    /*   ---   CONSTRUCTORS   ---   */
    public:
    template <typename Pred = Predicate, typename = std::enable_if_t<std::is_default_constructible_v<Pred>>>
    generative_pred_navigator() :
            node_pred_navigator<node_ptrs_t, Predicate>() {
    }

    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, TargetNodePtr>>>
    generative_pred_navigator(
        const generative_pred_navigator<NodeAllocator, Predicate, OtherNodePtr, GeneratedNodePtr>& other) :
            generative_pred_navigator(static_cast<node_ptrs_t>(other.root, other.is_subtree)) {
    }

    generative_pred_navigator(
        NodeAllocator& allocator,
        multiple_node_pointer<TargetNodePtr, GeneratedNodePtr> root,
        Predicate predicate,
        bool is_subtree = true) :
            node_pred_navigator<node_ptrs_t, Predicate>(root, predicate, is_subtree),
            allocator(allocator) {
    }

    /*   ---   ASSIGNMENT   ---   */
    template <typename OtherNodePtr, typename = std::enable_if_t<is_const_cast_equivalent<OtherNodePtr, TargetNodePtr>>>
    generative_pred_navigator& operator=(
        const generative_pred_navigator<NodeAllocator, Predicate, OtherNodePtr, GeneratedNodePtr>& other) {
        this->is_subtree = other.is_subtree;
        this->root       = static_cast<node_ptrs_t>(other.root);
    }

    /*   ---   METHODS   ---   */
    public:
    node_ptrs_t get_prev_sibling(node_ptrs_t node) const {
        node_ptrs_t referred(this->node_pred_navigator<node_ptrs_t, Predicate>::get_prev_sibling(node));
    }

    node_ptrs_t get_next_sibling(node_ptrs_t node) const {
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_next_sibling(node));
    }

    node_ptrs_t get_first_child(node_ptrs_t node) const {
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_first_child(node));
    }

    node_ptrs_t get_last_child(node_ptrs_t node) const {
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_last_child(node));
    }

    template <
        typename N = TargetNodePtr,
        typename   = std::enable_if_t<is_same_template<std::decay_t<std::remove_pointer_t<N>>, binary_node<void>>>>
    node_ptrs_t get_left_child(N node) const {
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_left_child(node));
    }

    template <
        typename N = TargetNodePtr,
        typename   = std::enable_if_t<is_same_template<std::decay_t<std::remove_pointer_t<N>>, binary_node<void>>>>
    node_ptrs_t get_right_child(N node) const {
        node_ptrs_t result(this->node_pred_navigator<node_ptrs_t, Predicate>::get_right_child(node));
    }
};

} // namespace md