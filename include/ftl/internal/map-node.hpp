// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_INTERNAL_MAP_NODE_HPP
#define FTL_INTERNAL_MAP_NODE_HPP

#include <array>
#include <cassert>
#include <ftl/algorithm.hpp>

namespace ftl::detail {
  constexpr std::size_t maxValues = 3;
  constexpr std::size_t maxChildren = 4;

  template <typename NodePtr>
  void clear(NodePtr) noexcept;

  template <typename T>
  struct Node final
  {
    alignas(T) char buffer[maxValues * sizeof(T)];
    T* begin = reinterpret_cast<T*>(buffer);
    T* end = reinterpret_cast<T*>(buffer);
    std::array<Node*, maxChildren> children{};
    Node* parent = nullptr;

    Node() = default;
    Node(const Node&) = delete;
    ~Node() { clear(this); }
    Node& operator=(const Node&) = delete;
  };

  template <typename NodePtr>
  std::size_t size(NodePtr) noexcept;
  template <typename NodePtr>
  bool is_empty(NodePtr) noexcept;
  template <typename NodePtr>
  bool is_three(NodePtr) noexcept;

  template <typename NodePtr>
  bool is_leaf(NodePtr) noexcept;
  template <typename NodePtr>
  bool is_left(NodePtr) noexcept;
  template <typename NodePtr>
  bool is_middle(NodePtr) noexcept;
  template <typename NodePtr>
  bool is_right(NodePtr) noexcept;

  template <typename NodePtr>
  NodePtr get_right_sibling(NodePtr) noexcept;
  template <typename NodePtr>
  NodePtr get_left_sibling(NodePtr) noexcept;

  template <typename NodePtr>
  NodePtr tree_min(NodePtr) noexcept;
  template <typename NodePtr>
  NodePtr tree_max(NodePtr) noexcept;

  template <typename NodePtr>
  NodePtr update_parent(NodePtr) noexcept;
  template <typename NodePtr>
  void relink(NodePtr, NodePtr) noexcept;

  template <typename NodePtr, typename ValPtr>
  std::tuple<NodePtr, ValPtr> next_iter(NodePtr, ValPtr);
  template <typename NodePtr, typename ValPtr>
  std::tuple<NodePtr, ValPtr> prev_iter(NodePtr, ValPtr);

  template <typename NodePtr, typename... Args>
  void emplace_back(NodePtr, Args&&...);
  template <typename NodePtr, typename Cmp, typename... Args>
  void emplace(NodePtr src, NodePtr dest, Cmp, Args&&...);

  template <typename NodePtr>
  void pop_back(NodePtr) noexcept;
  template <typename NodePtr, typename ValPtr>
  void pop(NodePtr src, NodePtr dest, ValPtr);
}

template <typename NodePtr>
void ftl::detail::clear(NodePtr node) noexcept
{
  assert(node && "Clear: nullptr node given");
  ftl::destroy(node->begin, node->end);
  node->end = node->begin;
}

template <typename NodePtr>
std::size_t ftl::detail::size(NodePtr node) noexcept
{
  assert(node && "Size: nullptr node given");
  return node->end - node->begin;
}

template <typename NodePtr>
bool ftl::detail::is_empty(NodePtr node) noexcept
{
  assert(node && "IsEmpty: nullptr node given");
  return node->end == node->begin;
}

template <typename NodePtr>
bool ftl::detail::is_three(NodePtr node) noexcept
{
  assert(node && "IsThree: nullptr node given");
  return size(node) == 2;
}

template <typename NodePtr>
bool ftl::detail::is_leaf(NodePtr node) noexcept
{
  assert(node && "IsLeaf: nullptr node given");
  return !node->children[0] || is_empty(node->children[0]);
}

template <typename NodePtr>
bool ftl::detail::is_left(NodePtr node) noexcept
{
  assert(node && "IsLeft: nullptr node given");
  assert(node->parent && "IsLeft: root node given");
  return node == node->parent->children[0];
}

template <typename NodePtr>
bool ftl::detail::is_middle(NodePtr node) noexcept
{
  assert(node && "IsMiddle: nullptr node given");
  assert(node->parent && "IsMiddle: root node given");
  NodePtr parent = node->parent;
  return is_three(parent) && node == parent->children[1];
}

template <typename NodePtr>
bool ftl::detail::is_right(NodePtr node) noexcept
{
  assert(node && "IsRight: nullptr node given");
  assert(node->parent && "IsRight: root node given");
  NodePtr parent = node->parent;
  return node == parent->children[size(parent)];
}

template <typename NodePtr>
NodePtr ftl::detail::get_right_sibling(NodePtr node) noexcept
{
  assert(node && "GetSibling: nullptr node given");
  assert(node->parent && "GetSibling: root node given");
  assert(!isRight(node) && "GetRightSibling: right node given");
  const NodePtr parent = node->parent;
  if (is_left(node)) {
    return parent->children[1];
  }
  return parent->children[2];
}

template <typename NodePtr>
NodePtr ftl::detail::get_left_sibling(NodePtr node) noexcept
{
  assert(node && "GetSibling: nullptr node given");
  assert(node->parent && "GetSibling: root node given");
  assert(!isLeft(node) && "GetLeftSibling: left node given");
  const NodePtr parent = node->parent;
  if (is_right(node)) {
    return parent->children[size(parent) - 1];
  }
  return parent->children[0];
}

template <typename NodePtr>
NodePtr ftl::detail::tree_min(NodePtr root) noexcept
{
  assert(root && "TreeMin: nullptr node given");
  while (!is_leaf(root)) {
    root = root->children[0];
  }
  return root;
}

template <typename NodePtr>
NodePtr ftl::detail::tree_max(NodePtr root) noexcept
{
  assert(root && "TreeMax: nullptr node given");
  while (!is_leaf(root)) {
    root = root->children[size(root)];
  }
  return root;
}

template <typename NodePtr>
NodePtr ftl::detail::update_parent(NodePtr parent) noexcept
{
  assert(parent && "UpdateParent: nullptr node given");
  for (auto child : parent->children) {
    if (child) {
      child->parent = parent;
    }
  }
  return parent;
}

template <typename NodePtr>
void ftl::detail::relink(NodePtr lhs, NodePtr rhs) noexcept
{
  assert(lhs && rhs && "Relink: nullptr node given");
  if (lhs->parent) {
    auto& children = lhs->parent->children;
    auto pos = std::find(children.begin(), children.end(), lhs);
    assert(pos != children.end() && *pos == lhs);
    *pos = rhs;
  }
  if (rhs->parent) {
    auto& children = rhs->parent->children;
    auto pos = std::find(children.begin(), children.end(), lhs);
    assert(pos != children.end() && *pos == rhs);
    *pos = lhs;
  }
  std::swap(lhs->parent, rhs->parent);
  std::swap(lhs->children, rhs->children);
  update_parent(lhs);
  update_parent(rhs);
}

template <typename NodePtr, typename ValPtr>
std::tuple<NodePtr, ValPtr>
ftl::detail::next_iter(NodePtr node, ValPtr valuePtr)
{
  assert(node && valuePtr && "Incrementing empty iterator");
  if (!is_leaf(node)) {
    node = tree_min(node->children[(valuePtr - node->begin) + 1]);
    return std::make_tuple(node, node->begin);
  }
  if (valuePtr + 1 != node->end) {
    return std::make_tuple(node, ++valuePtr);
  }
  NodePtr firstChild = node->children[1];
  if (firstChild && is_empty(firstChild)) {
    return std::make_tuple(firstChild, firstChild->begin);
  }
  while (node->parent) {
    NodePtr parent = node->parent;
    if (is_left(node) || (is_three(parent) && is_middle(node))) {
      valuePtr = is_left(node) ? parent->begin : parent->begin + 1;
      return std::make_tuple(parent, valuePtr);
    }
    node = parent;
  }
  return std::make_tuple(nullptr, nullptr);
}

template <typename NodePtr, typename ValPtr>
std::tuple<NodePtr, ValPtr>
ftl::detail::prev_iter(NodePtr node, ValPtr valuePtr)
{
  assert(node && valuePtr && "Decrementing empty iterator");
  if (is_empty(node)) {
    return std::make_tuple(node->parent, node->parent->end - 1);
  }
  if (!is_leaf(node)) {
    node = tree_max(node->children[valuePtr - node->begin]);
    return std::make_tuple(node, node->end - 1);
  }
  if (valuePtr != node->begin) {
    return std::make_tuple(node, --valuePtr);
  }
  while (node->parent) {
    NodePtr parent = node->parent;
    if (!is_left(node)) {
      const bool useBegin = !(size(parent) == 1 || is_middle(node));
      return std::make_tuple(parent, parent->begin + useBegin);
    }
    node = parent;
  }
  return std::make_tuple(nullptr, nullptr);
}

template <typename NodePtr, typename... Args>
void ftl::detail::emplace_back(NodePtr node, Args&&... args)
{
  assert(node && "EmplaceBack: nullptr node given");
  assert(size(node) < maxValues && "EmplaceBack: filled node given");
  using value_type = std::remove_reference_t<decltype(*(node->begin))>;
  new (node->end) value_type(std::forward<Args>(args)...);
  ++node->end;
}

template <typename NodePtr, typename Cmp, typename... Args>
void ftl::detail::emplace(NodePtr src, NodePtr dest, Cmp cmp, Args&&... args)
{
  assert(src && dest && "Emplace: nullptr node given");
  assert(size(src) < maxValues && "Emplace: filled node given");
  assert(isEmpty(dest) && "Emplace: non-empty node given");
  using value_type = std::remove_reference_t<decltype(*(src->begin))>;
  value_type val(std::forward<Args>(args)...);
  auto i = src->begin;
  while (i != src->end && cmp(*i, val)) {
    emplace_back(dest, std::move_if_noexcept(*(i++)));
  }
  emplace_back(dest, std::move(val));
  while (i != src->end) {
    emplace_back(dest, std::move_if_noexcept(*(i++)));
  }
}

template <typename NodePtr>
void ftl::detail::pop_back(NodePtr node) noexcept
{
  assert(node && "PopBack: nullptr node given");
  assert(!isEmpty(node) && "PopBack: non-empty node given");
  using value_type = std::remove_reference_t<decltype(*(node->begin))>;
  (node->end - 1)->~value_type();
  --node->end;
}

template <typename NodePtr, typename ValPtr>
void ftl::detail::pop(NodePtr src, NodePtr dest, ValPtr val)
{
  assert(src && dest && "Pop: nullptr node given");
  assert(isEmpty(dest) && "Pop: non-empty node given");
  assert(val >= src->begin && val < src->end && "Pop: invalid val given");
  auto i = src->begin;
  while (i != val) {
    emplace_back(dest, std::move_if_noexcept(*(i++)));
  }
  ++i;
  while (i != src->end) {
    emplace_back(dest, std::move_if_noexcept(*(i++)));
  }
}

#endif
