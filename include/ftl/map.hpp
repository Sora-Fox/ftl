// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_MAP_HPP
#define FTL_MAP_HPP

#include <algorithm>
#include <memory>
#include <queue>
#include <stack>
#include <tuple>
#include <type_traits>
#include <utility>
#include <ftl/internal/map-node.hpp>

namespace ftl {
  template <typename Key, typename T, typename Comparator = std::less<Key>>
  class map
  {
  public:
    using value_type = std::pair<const Key, T>;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using key_type = Key;
    using mapped_type = T;
    using size_type = std::size_t;
    using key_compare = Comparator;

  private:
    template <bool isConst>
    class Iterator;

    template <bool isConst>
    class HeavyIterator;
    template <bool isConst>
    class LmrIterator;
    template <bool isConst>
    class RmlIterator;
    template <bool isConst>
    class BfsIterator;

  public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    using lmr_iterator = LmrIterator<false>;
    using const_lmr_iterator = LmrIterator<true>;
    using rml_iterator = RmlIterator<false>;
    using const_rml_iterator = RmlIterator<true>;
    using bfs_iterator = BfsIterator<false>;
    using const_bfs_iterator = BfsIterator<true>;

    class value_compare;

  private:
    static constexpr bool is_nothrow_default_constructible =
        std::is_nothrow_default_constructible_v<key_compare>;
    static constexpr bool is_nothrow_move_constructible =
        std::is_nothrow_move_constructible_v<key_compare>;
    static constexpr bool is_nothrow_copy_constructible =
        std::is_nothrow_copy_constructible_v<key_compare>;
    static constexpr bool is_nothrow_move_assignable =
        std::is_nothrow_move_assignable_v<key_compare>;
    static constexpr bool is_nothrow_swappable =
        std::is_nothrow_swappable_v<key_compare>;

  public:
    map() noexcept(is_nothrow_default_constructible) = default;
    map(const map&);
    map(map&&) noexcept(is_nothrow_move_constructible);
    explicit map(const key_compare&) noexcept(is_nothrow_copy_constructible);
    template <typename InputIt>
    map(InputIt, InputIt, const key_compare& = key_compare{});
    map(std::initializer_list<value_type>, const key_compare& = key_compare{});
    ~map();

    map& operator=(const map&);
    map& operator=(map&&) noexcept(is_nothrow_move_assignable);
    map& operator=(std::initializer_list<value_type>);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;

    lmr_iterator lmr_begin();
    lmr_iterator lmr_end();
    const_lmr_iterator lmr_begin() const;
    const_lmr_iterator lmr_end() const;

    rml_iterator rml_begin();
    rml_iterator rml_end();
    const_rml_iterator rml_begin() const;
    const_rml_iterator rml_end() const;

    bfs_iterator bfs_begin();
    bfs_iterator bfs_end();
    const_bfs_iterator bfs_begin() const;
    const_bfs_iterator bfs_end() const;

    size_type size() const noexcept;
    bool empty() const noexcept;

    mapped_type& operator[](const key_type&);
    mapped_type& operator[](key_type&&);

    mapped_type& at(const key_type&);
    const mapped_type& at(const key_type&) const;

    std::pair<iterator, bool> insert(const_reference);
    std::pair<iterator, bool> insert(value_type&&);
    iterator insert(const_iterator, const_reference);
    iterator insert(const_iterator, value_type&&);
    template <typename InputIt>
    void insert(InputIt, InputIt);
    void insert(std::initializer_list<value_type>);

    iterator erase(const_iterator);
    size_type erase(const key_type&);
    iterator erase(const_iterator, const_iterator);

    void clear() noexcept;
    void swap(map&) noexcept(is_nothrow_swappable);

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&...);
    template <typename... Args>
    iterator emplace_hint(const_iterator, Args&&...);

    key_compare key_comp() const;
    value_compare value_comp() const;

    iterator find(const key_type&);
    const_iterator find(const key_type&) const;

    size_type count(const key_type&) const;

    iterator lower_bound(const key_type&);
    const_iterator lower_bound(const key_type&) const;

    iterator upper_bound(const key_type&);
    const_iterator upper_bound(const key_type&) const;

    std::pair<iterator, iterator> equal_range(const key_type&);
    std::pair<const_iterator, const_iterator>
    equal_range(const key_type&) const;

    template <typename F>
    F traverseLmr(F) const;
    template <typename F>
    F traverseRml(F) const;
    template <typename F>
    F traverseBreadth(F) const;

  private:
    using Node = detail::Node<value_type>;
    class EndNodeGuard;

    Node* root_ = nullptr;
    size_type size_ = 0;
    Comparator comparator_;

    void deallocate() noexcept;
    Node* get_end_node() const noexcept;

    template <typename... Args>
    Node* emplace_to_node(Node*, Args&&...);
    template <typename... Args>
    iterator emplace_to_empty(Args&&...);
    Node* erase_from_node(Node*, const_pointer);
    void swap_vals(Node* lhs, pointer lhsPtr, Node* rhs, pointer rhsPtr);

    Node* split(Node*);
    std::tuple<Node*, Node*> split_in_two(const Node*);
    void
    split_children(const Node* src, Node* left, Node* right) const noexcept;

    pointer find_key(const Node*, const key_type&) const;
    Node* find_target(Node*, const key_type&) const;
    Node* validate_hint(Node*, const key_type&) const;

    Node* fix_underflow(Node*);
    Node* fix_root_underflow(Node*);

    bool is_mergeable(const Node*) const;
    Node* merge(Node*);

    Node* redistribute(Node*);
    Node* give_to_sibling(Node*);
    Node* borrow_from_sibling(Node*);
    Node* borrow_from_left(Node* target, Node* sibling);
    Node* borrow_from_right(Node* target, Node* sibling);
  };

  template <typename K, typename T, typename C>
  map<K, T, C>::~map()
  {
    if (!empty()) {
      deallocate();
    }
  }

  template <typename K, typename T, typename C>
  bool operator==(const map<K, T, C>&, const map<K, T, C>&);

  template <typename K, typename T, typename C>
  bool operator!=(const map<K, T, C>&, const map<K, T, C>&);

  template <typename K, typename T, typename C>
  bool operator<(const map<K, T, C>&, const map<K, T, C>&);

  template <typename K, typename T, typename C>
  bool operator>(const map<K, T, C>&, const map<K, T, C>&);

  template <typename K, typename T, typename C>
  bool operator<=(const map<K, T, C>&, const map<K, T, C>&);

  template <typename K, typename T, typename C>
  bool operator>=(const map<K, T, C>&, const map<K, T, C>&);

  template <typename K, typename T, typename C>
  void swap(map<K, T, C>& l, map<K, T, C>& r) noexcept(noexcept(l.swap(r)));
}

template <typename K, typename T, typename C>
template <bool IsConst>
class ftl::map<K, T, C>::Iterator
{
private:
  template <typename T1, typename T2>
  using conditional_t = std::conditional_t<IsConst, T1, T2>;

public:
  using value_type = map::value_type;
  using difference_type = std::ptrdiff_t;
  using pointer = conditional_t<map::const_pointer, map::pointer>;
  using reference = conditional_t<map::const_reference, map::reference>;
  using iterator_category = std::bidirectional_iterator_tag;

  Iterator() noexcept = default;
  template <bool RhsConst, std::enable_if_t<IsConst && !RhsConst, int> = 0>
  Iterator(const Iterator<RhsConst>&) noexcept;

  pointer operator->() const noexcept;
  reference operator*() const noexcept;

  Iterator& operator++();
  Iterator operator++(int);
  Iterator& operator--();
  Iterator operator--(int);

  friend bool operator==(const Iterator& lhs, const Iterator& rhs) noexcept
  {
    return rhs.node_ == lhs.node_ && rhs.valuePtr_ == lhs.valuePtr_;
  }

  friend bool operator!=(const Iterator& lhs, const Iterator& rhs) noexcept
  {
    return !(rhs == lhs);
  }

private:
  friend class map;

  Node* node_ = nullptr;
  pointer valuePtr_ = nullptr;

  Iterator(Node*) noexcept;
  Iterator(Node*, pointer) noexcept;
};

template <typename K, typename T, typename C>
template <bool IsConst>
ftl::map<K, T, C>::Iterator<IsConst>::Iterator(Node* node) noexcept :
  node_(node),
  valuePtr_(node ? node->begin : nullptr)
{
}

template <typename K, typename T, typename C>
template <bool IsConst>
ftl::map<K, T, C>::Iterator<IsConst>::Iterator(Node* node,
    pointer valuePtr) noexcept :
  node_(node),
  valuePtr_(valuePtr)
{
}

template <typename K, typename T, typename C>
template <bool IsConst>
template <bool IsRhsConst, std::enable_if_t<IsConst && !IsRhsConst, int>>
ftl::map<K, T, C>::Iterator<IsConst>::Iterator(
    const Iterator<IsRhsConst>& rhs) noexcept :
  node_(rhs.node_),
  valuePtr_(rhs.valuePtr_)
{
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::Iterator<IsConst>::operator->() const noexcept
    -> pointer
{
  assert(node_ && valuePtr_ && "Dereferencing empty iterator");
  return std::addressof(**this);
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::Iterator<IsConst>::operator*() const noexcept
    -> reference
{
  assert(node_ && valuePtr_ && "Dereferencing empty iterator");
  return *valuePtr_;
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::Iterator<IsConst>::operator++() -> Iterator&
{
  assert(node_ && valuePtr_ && "Incrementing empty iterator");
  std::tie(node_, valuePtr_) = detail::next_iter(node_, valuePtr_);
  return *this;
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::Iterator<IsConst>::operator++(int) -> Iterator
{
  Iterator result(*this);
  ++(*this);
  return result;
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::Iterator<IsConst>::operator--() -> Iterator&
{
  assert(node_ && valuePtr_ && "Decrementing empty iterator");
  std::tie(node_, valuePtr_) = detail::prev_iter(node_, valuePtr_);
  return *this;
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::Iterator<IsConst>::operator--(int) -> Iterator
{
  Iterator result(*this);
  --(*this);
  return result;
}

template <typename K, typename T, typename C>
template <bool IsConst>
class ftl::map<K, T, C>::HeavyIterator
{
private:
  template <typename T1, typename T2>
  using conditional_t = std::conditional_t<IsConst, T1, T2>;

public:
  using value_type = map::value_type;
  using difference_type = std::ptrdiff_t;
  using pointer = conditional_t<map::const_pointer, map::pointer>;
  using reference = conditional_t<map::const_reference, map::reference>;
  using iterator_category = std::forward_iterator_tag;

  pointer operator->() const noexcept { return std::addressof(**this); }
  reference operator*() const noexcept
  {
    assert(valuePtr_ && "Dereferencing empty HeavyIterator");
    return *valuePtr_;
  }
  operator map::Iterator<IsConst>() { return { node_, valuePtr_ }; }

  friend bool
  operator==(const HeavyIterator& lhs, const HeavyIterator& rhs) noexcept
  {
    return lhs.node_ == rhs.node_ && lhs.valuePtr_ == rhs.valuePtr_;
  }

  friend bool
  operator!=(const HeavyIterator& lhs, const HeavyIterator& rhs) noexcept
  {
    return !(lhs == rhs);
  }

protected:
  Node* node_ = nullptr;
  pointer valuePtr_ = nullptr;
};

template <typename K, typename T, typename C>
template <bool IsConst>
class ftl::map<K, T, C>::LmrIterator : public HeavyIterator<IsConst>
{
public:
  using pointer = typename HeavyIterator<IsConst>::pointer;
  using reference = typename HeavyIterator<IsConst>::reference;

  LmrIterator() noexcept = default;
  template <bool RhsConst, std::enable_if_t<IsConst && !RhsConst, int> = 0>
  LmrIterator(const LmrIterator<RhsConst>& rhs) :
    HeavyIterator<IsConst>(rhs),
    values_(rhs.values_)
  {
  }
  template <bool RhsConst, std::enable_if_t<!IsConst || RhsConst, int> = 0>
  LmrIterator(Iterator<RhsConst> rhs) : LmrIterator(rhs.node_)
  {
  }

  LmrIterator& operator++();
  LmrIterator operator++(int)
  {
    LmrIterator result(*this);
    ++(*this);
    return result;
  }

private:
  friend class map;

  std::stack<std::pair<Node*, pointer>> values_{};
  using HeavyIterator<IsConst>::valuePtr_;
  using HeavyIterator<IsConst>::node_;

  LmrIterator(Node*);
};

template <typename K, typename T, typename C>
template <bool IsConst>
ftl::map<K, T, C>::LmrIterator<IsConst>::LmrIterator(Node* root) :
  HeavyIterator<IsConst>()
{
  while (root && root->parent) {
    root = root->parent;
  }
  while (root && !detail::is_empty(root)) {
    values_.emplace(root, root->begin);
    root = root->children[0];
  }
  ++(*this);
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::LmrIterator<IsConst>::operator++() -> LmrIterator&
{
  if (values_.empty()) {
    valuePtr_ = nullptr;
    node_ = nullptr;
    return *this;
  }
  node_ = values_.top().first;
  valuePtr_ = values_.top().second;
  values_.pop();
  Node* next = nullptr;
  if (valuePtr_ == node_->begin && detail::is_three(node_)) {
    values_.emplace(node_, valuePtr_ + 1);
    next = node_->children[1];
  } else {
    next = node_->children[detail::size(node_)];
  }
  while (next && !detail::is_empty(next)) {
    values_.emplace(next, next->begin);
    next = next->children[0];
  }
  return *this;
}

template <typename K, typename T, typename C>
template <bool IsConst>
class ftl::map<K, T, C>::RmlIterator : public HeavyIterator<IsConst>
{
public:
  using pointer = typename HeavyIterator<IsConst>::pointer;
  using reference = typename HeavyIterator<IsConst>::reference;

  RmlIterator() noexcept = default;
  template <bool RhsConst, std::enable_if_t<IsConst && !RhsConst, int> = 0>
  RmlIterator(const RmlIterator<RhsConst>& rhs) :
    HeavyIterator<IsConst>(rhs),
    values_(rhs.values_)
  {
  }
  template <bool RhsConst, std::enable_if_t<!IsConst || RhsConst, int> = 0>
  RmlIterator(Iterator<RhsConst> rhs) : RmlIterator(rhs.node_)
  {
  }

  RmlIterator& operator++();
  RmlIterator operator++(int)
  {
    RmlIterator result(*this);
    ++(*this);
    return result;
  }

private:
  friend class map;

  std::stack<std::pair<Node*, pointer>> values_{};
  using HeavyIterator<IsConst>::valuePtr_;
  using HeavyIterator<IsConst>::node_;

  RmlIterator(Node*);
};

template <typename K, typename T, typename C>
template <bool IsConst>
ftl::map<K, T, C>::RmlIterator<IsConst>::RmlIterator(Node* root) :
  HeavyIterator<IsConst>()
{
  while (root && root->parent) {
    root = root->parent;
  }
  while (root && !detail::is_empty(root)) {
    values_.emplace(root, root->end - 1);
    root = root->children[detail::size(root)];
  }
  ++(*this);
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::RmlIterator<IsConst>::operator++() -> RmlIterator&
{
  if (values_.empty()) {
    valuePtr_ = nullptr;
    node_ = nullptr;
    return *this;
  }
  node_ = values_.top().first;
  valuePtr_ = values_.top().second;
  values_.pop();
  Node* next = nullptr;
  if (valuePtr_ != node_->begin) {
    values_.emplace(node_, valuePtr_ + 1);
    next = node_->children[valuePtr_ == node_->begin + 1];
  } else {
    next = node_->children[0];
  }
  while (next && !detail::is_empty(next)) {
    values_.emplace(next, next->begin);
    next = next->children[detail::size(next)];
  }
  return *this;
}

template <typename K, typename T, typename C>
template <bool IsConst>
class ftl::map<K, T, C>::BfsIterator : public HeavyIterator<IsConst>
{
public:
  using pointer = typename HeavyIterator<IsConst>::pointer;
  using reference = typename HeavyIterator<IsConst>::reference;

  BfsIterator() noexcept = default;
  template <bool RhsConst, std::enable_if_t<IsConst && !RhsConst, int> = 0>
  BfsIterator(const BfsIterator<RhsConst>& rhs) :
    HeavyIterator<IsConst>(rhs),
    nodes_(rhs.nodes_)
  {
  }
  template <bool RhsConst, std::enable_if_t<!IsConst || RhsConst, int> = 0>
  BfsIterator(Iterator<RhsConst> rhs) : BfsIterator(rhs.node_)
  {
  }

  BfsIterator& operator++();
  BfsIterator operator++(int)
  {
    BfsIterator result(*this);
    ++(*this);
    return result;
  }

private:
  friend class map;

  std::queue<Node*> nodes_{};
  using HeavyIterator<IsConst>::valuePtr_;
  using HeavyIterator<IsConst>::node_;

  BfsIterator(Node*);
};

template <typename K, typename T, typename C>
template <bool IsConst>
ftl::map<K, T, C>::BfsIterator<IsConst>::BfsIterator(Node* root) :
  HeavyIterator<IsConst>()
{
  while (root && root->parent) {
    root = root->parent;
  }
  if (root) {
    nodes_.push(root);
    ++(*this);
  }
}

template <typename K, typename T, typename C>
template <bool IsConst>
auto ftl::map<K, T, C>::BfsIterator<IsConst>::operator++() -> BfsIterator&
{
  if (valuePtr_ && valuePtr_ + 1 != node_->end) {
    ++valuePtr_;
    return *this;
  }
  if (nodes_.empty()) {
    valuePtr_ = nullptr;
    node_ = nullptr;
    return *this;
  }
  node_ = nodes_.front();
  valuePtr_ = node_->begin;
  nodes_.pop();
  for (Node* child : node_->children) {
    if (child && !detail::is_empty(child)) {
      nodes_.push(child);
    }
  }
  return *this;
}

template <typename K, typename T, typename C>
class ftl::map<K, T, C>::value_compare
{
public:
  bool operator()(const_reference lhs, const_reference rhs) const
  {
    return comp_(lhs.first, rhs.first);
  }

private:
  friend class map;
  key_compare comp_;

  value_compare(key_compare comp) : comp_(comp) {}
};

template <typename K, typename T, typename C>
ftl::map<K, T, C>::map(const map& rhs) :
  map(rhs.begin(), rhs.end(), rhs.comparator_)
{
}

template <typename K, typename T, typename C>
ftl::map<K, T, C>::map(map&& rhs) noexcept(is_nothrow_move_constructible) :
  root_(std::exchange(rhs.root_, nullptr)),
  size_(std::exchange(rhs.size_, 0)),
  comparator_(std::move(rhs.comparator_))
{
}

template <typename K, typename T, typename C>
ftl::map<K, T, C>::map(const key_compare& comparator) noexcept(
    is_nothrow_copy_constructible) :
  comparator_(comparator)
{
}

template <typename K, typename T, typename C>
template <typename InputIt>
ftl::map<K, T, C>::map(const InputIt first, const InputIt last,
    const key_compare& comparator) :
  map(comparator)
{
  insert(first, last);
}

template <typename K, typename T, typename C>
ftl::map<K, T, C>::map(std::initializer_list<value_type> init,
    const key_compare& comparator) :
  map(init.begin(), init.end(), comparator)
{
}

template <typename K, typename T, typename C>
ftl::map<K, T, C>& ftl::map<K, T, C>::operator=(const map& rhs)
{
  map(rhs).swap(*this);
  return *this;
}

template <typename K, typename T, typename C>
ftl::map<K, T, C>&
ftl::map<K, T, C>::operator=(map&& rhs) noexcept(is_nothrow_move_assignable)
{
  map(std::move(rhs)).swap(*this);
  return *this;
}

template <typename K, typename T, typename C>
ftl::map<K, T, C>&
ftl::map<K, T, C>::operator=(std::initializer_list<value_type> init)
{
  map(init, comparator_).swap(*this);
  return *this;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator ftl::map<K, T, C>::begin() noexcept
{
  if (empty()) {
    return end();
  }
  return iterator{ detail::tree_min(root_) };
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator ftl::map<K, T, C>::end() noexcept
{
  return iterator{ get_end_node() };
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_iterator
ftl::map<K, T, C>::begin() const noexcept
{
  if (empty()) {
    return end();
  }
  return const_iterator{ detail::tree_min(root_) };
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_iterator
ftl::map<K, T, C>::end() const noexcept
{
  return const_iterator{ get_end_node() };
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::lmr_iterator ftl::map<K, T, C>::lmr_begin()
{
  return lmr_iterator(root_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::lmr_iterator ftl::map<K, T, C>::lmr_end()
{
  return lmr_iterator(nullptr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_lmr_iterator
ftl::map<K, T, C>::lmr_begin() const
{
  return const_lmr_iterator(root_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_lmr_iterator
ftl::map<K, T, C>::lmr_end() const
{
  return const_lmr_iterator(nullptr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::rml_iterator ftl::map<K, T, C>::rml_begin()
{
  return rml_iterator(root_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::rml_iterator ftl::map<K, T, C>::rml_end()
{
  return rml_iterator(nullptr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_rml_iterator
ftl::map<K, T, C>::rml_begin() const
{
  return const_rml_iterator(root_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_rml_iterator
ftl::map<K, T, C>::rml_end() const
{
  return const_rml_iterator(nullptr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::bfs_iterator ftl::map<K, T, C>::bfs_begin()
{
  return bfs_iterator(root_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::bfs_iterator ftl::map<K, T, C>::bfs_end()
{
  return bfs_iterator(nullptr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_bfs_iterator
ftl::map<K, T, C>::bfs_begin() const
{
  return const_bfs_iterator(root_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_bfs_iterator
ftl::map<K, T, C>::bfs_end() const
{
  return const_bfs_iterator(nullptr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::size_type ftl::map<K, T, C>::size() const noexcept
{
  return size_;
}

template <typename K, typename T, typename C>
bool ftl::map<K, T, C>::empty() const noexcept
{
  return root_ == nullptr;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::mapped_type&
ftl::map<K, T, C>::operator[](const key_type& key)
{
  const auto inserted = emplace(key, mapped_type());
  return inserted.first->second;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::mapped_type&
ftl::map<K, T, C>::operator[](key_type&& key)
{
  const auto inserted = emplace(std::move(key), mapped_type());
  return inserted.first->second;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::mapped_type&
ftl::map<K, T, C>::at(const key_type& key)
{
  const map* constThis = this;
  return const_cast<mapped_type&>(constThis->at(key));
}

template <typename K, typename T, typename C>
const typename ftl::map<K, T, C>::mapped_type&
ftl::map<K, T, C>::at(const key_type& key) const
{
  auto it = find(key);
  if (it == end()) {
    throw std::out_of_range("Key not found");
  }
  return it->second;
}

template <typename K, typename T, typename C>
std::pair<typename ftl::map<K, T, C>::iterator, bool>
ftl::map<K, T, C>::insert(const_reference value)
{
  return emplace(value);
}

template <typename K, typename T, typename C>
std::pair<typename ftl::map<K, T, C>::iterator, bool>
ftl::map<K, T, C>::insert(value_type&& value)
{
  return emplace(std::move(value));
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::insert(const_iterator hint, const_reference value)
{
  return emplace_hint(hint, value);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::insert(const_iterator hint, value_type&& value)
{
  return emplace_hint(hint, std::move(value));
}

template <typename K, typename T, typename C>
template <typename InputIt>
void ftl::map<K, T, C>::insert(InputIt first, const InputIt last)
{
  for (; first != last; ++first) {
    insert(*first);
  }
}

template <typename K, typename T, typename C>
void ftl::map<K, T, C>::insert(std::initializer_list<value_type> list)
{
  insert(list.begin(), list.end());
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::erase(const_iterator position)
{
  assert(position != end() && "Position for erasing must not be end()");
  const key_type erasingKey = position->first;
  Node* node = position.node_;
  pointer nodePtr = const_cast<pointer>(position.valuePtr_);
  EndNodeGuard guard(this);
  using std::exchange;
  if (!detail::is_leaf(node) && nodePtr == node->begin) {
    Node* left = detail::tree_max(node->children[0]);
    pointer leftPtr = left->end - 1;
    swap_vals(left, leftPtr, exchange(node, left), exchange(nodePtr, leftPtr));
  } else if (!detail::is_leaf(node)) {
    Node* right = detail::tree_min(node->children[detail::size(node)]);
    pointer rightPtr = right->begin;
    swap_vals(right, rightPtr, exchange(node, right),
        exchange(nodePtr, rightPtr));
  }
  node = erase_from_node(node, nodePtr);
  --size_;
  while (node && detail::is_empty(node)) {
    node = fix_underflow(node);
  }
  guard.join();
  return upper_bound(erasingKey);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::size_type
ftl::map<K, T, C>::erase(const key_type& key)
{
  const_iterator position = find(key);
  if (position == end()) {
    return 0;
  }
  erase(position);
  return 1;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::erase(const_iterator first, const const_iterator last)
{
  if (first == begin() && last == end()) {
    clear();
    return end();
  }
  iterator firstI(first.node_, const_cast<pointer>(first.valuePtr_));
  for (; firstI != last; ++first) {
    firstI = erase(firstI);
  }
  return firstI;
}

template <typename K, typename T, typename C>
void ftl::map<K, T, C>::clear() noexcept
{
  if (!empty()) {
    deallocate();
    root_ = nullptr;
    size_ = 0;
  }
}

template <typename K, typename T, typename C>
void ftl::map<K, T, C>::swap(map& rhs) noexcept(is_nothrow_swappable)
{
  using std::swap;
  swap(root_, rhs.root_);
  swap(size_, rhs.size_);
  swap(comparator_, rhs.comparator_);
}

template <typename K, typename T, typename C>
template <typename... Args>
std::pair<typename ftl::map<K, T, C>::iterator, bool>
ftl::map<K, T, C>::emplace(Args&&... args)
{
  const_iterator rootPos{ root_ };
  value_type value(std::forward<Args>(args)...);
  const bool result = count(value.first);
  return std::make_pair(emplace_hint(rootPos, std::move(value)), !result);
}

template <typename K, typename T, typename C>
template <typename... Args>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::emplace_hint(const_iterator hint, Args&&... args)
{
  if (empty()) {
    return emplace_to_empty(std::forward<Args>(args)...);
  }
  EndNodeGuard guard(this);
  value_type value{ std::forward<Args>(args)... };
  Node* target = find_target(hint.node_, value.first);
  pointer valuePtr = find_key(target, value.first);
  if (valuePtr != target->end) {
    return iterator(target, valuePtr);
  }
  const key_type key = value.first;
  target = emplace_to_node(target, std::move(value));
  ++size_;
  while (detail::size(target) > 2) {
    target = split(target);
  }
  guard.join();
  return find(key);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::key_compare ftl::map<K, T, C>::key_comp() const
{
  return comparator_;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::value_compare ftl::map<K, T, C>::value_comp() const
{
  return value_compare(comparator_);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::find(const key_type& key)
{
  const map* constThis = this;
  const_iterator constRes = constThis->find(key);
  return iterator(constRes.node_, const_cast<pointer>(constRes.valuePtr_));
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_iterator
ftl::map<K, T, C>::find(const key_type& key) const
{
  if (empty()) {
    return end();
  }
  Node* target = find_target(root_, key);
  pointer valuePtr = find_key(target, key);
  return valuePtr == target->end ? end() : const_iterator(target, valuePtr);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::size_type
ftl::map<K, T, C>::count(const key_type& key) const
{
  return find(key) == end() ? 0 : 1;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::lower_bound(const key_type& key)
{
  const map* constThis = this;
  const auto res = constThis->lower_bound(key);
  return iterator{ res.node_, const_cast<pointer>(res.valuePtr_) };
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_iterator
ftl::map<K, T, C>::lower_bound(const key_type& key) const
{
  auto res = begin();
  while (res != end() && key_comp()(res->first, key)) {
    ++res;
  };
  return res;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::upper_bound(const key_type& key)
{
  const map* constThis = this;
  const auto res = constThis->upper_bound(key);
  return iterator{ res.node_, const_cast<pointer>(res.valuePtr_) };
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::const_iterator
ftl::map<K, T, C>::upper_bound(const key_type& key) const
{
  auto res = begin();
  while (res != end() && !key_comp()(key, res->first)) {
    ++res;
  };
  return res;
}

template <typename K, typename T, typename C>
auto ftl::map<K, T, C>::equal_range(const key_type& key)
    -> std::pair<iterator, iterator>
{
  return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename K, typename T, typename C>
auto ftl::map<K, T, C>::equal_range(const key_type& key) const
    -> std::pair<const_iterator, const_iterator>
{
  return std::make_pair(lower_bound(key), upper_bound(key));
}

template <typename K, typename T, typename C>
template <typename F>
F ftl::map<K, T, C>::traverseLmr(F func) const
{
  return std::for_each(lmr_begin(), lmr_end(), func);
}

template <typename K, typename T, typename C>
template <typename F>
F ftl::map<K, T, C>::traverseRml(F func) const
{
  return std::for_each(rml_begin(), rml_end(), func);
}

template <typename K, typename T, typename C>
template <typename F>
F ftl::map<K, T, C>::traverseBreadth(F func) const
{
  return std::for_each(bfs_begin(), bfs_end(), func);
}

template <typename K, typename T, typename C>
class ftl::map<K, T, C>::EndNodeGuard
{
public:
  EndNodeGuard(const EndNodeGuard&) = delete;
  EndNodeGuard(map* owner) noexcept :
    owner_(owner),
    endNode_(owner->get_end_node())
  {
    endNode_->parent->children.fill(nullptr);
  }
  EndNodeGuard& operator=(const EndNodeGuard&) = delete;

  ~EndNodeGuard()
  {
    if (!isJoined_) {
      join();
    }
  }

  void join() noexcept
  {
    assert(!isJoined_ && "EndNode has already joined");
    isJoined_ = true;
    if (owner_->empty()) {
      delete endNode_;
      return;
    }
    Node* max = detail::tree_max(owner_->root_);
    endNode_->parent = max;
    max->children.fill(endNode_);
  }

private:
  map* owner_ = nullptr;
  Node* endNode_ = nullptr;
  bool isJoined_ = false;
};

template <typename K, typename T, typename C>
void ftl::map<K, T, C>::deallocate() noexcept
{
  assert(!empty() && "Attempt to deallocate empty tree");
  Node* endNode = get_end_node();
  endNode->parent->children.fill(nullptr);
  delete endNode;
  Node* left = root_;
  while (root_->children[0]) {
    left = detail::tree_min(left);
    const auto& rtChildren = root_->children;
    std::copy(rtChildren.begin() + 1, rtChildren.end(), left->children.begin());
    delete std::exchange(root_, root_->children[0]);
  }
  delete root_;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::get_end_node() const noexcept
{
  if (empty()) {
    return nullptr;
  }
  Node* max = detail::tree_max(root_);
  return detail::is_empty(max) ? max : max->children[0];
}

template <typename K, typename T, typename C>
template <typename... Args>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::emplace_to_node(Node* node, Args&&... args)
{
  assert(node && "emplaceToNode: nullptr given");
  if (detail::is_empty(node)) {
    detail::emplace_back(node, std::forward<Args>(args)...);
    return node;
  }
  std::unique_ptr<Node> result = std::make_unique<Node>();
  detail::emplace(node, result.get(), value_comp(),
      std::forward<Args>(args)...);
  detail::relink(node, result.get());
  if (node == root_) {
    root_ = result.get();
  }
  delete node;
  return result.release();
}

template <typename K, typename T, typename C>
template <typename... Args>
typename ftl::map<K, T, C>::iterator
ftl::map<K, T, C>::emplace_to_empty(Args&&... args)
{
  assert(empty() && "emplaceToEmpty called on non empty Map");
  root_ = new Node;
  detail::emplace_back(root_, std::forward<Args>(args)...);
  Node* endNode = new Node;
  endNode->parent = root_;
  root_->children.fill(endNode);
  ++size_;
  return begin();
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::erase_from_node(Node* node, const_pointer valPtr)
{
  assert(node && valPtr && "eraseFromNode: nullptr given");
  assert(valPtr >= node->begin && valPtr < node->end &&
         "eraseFromNode: invalid valPtr");
  if (valPtr == node->end - 1) {
    detail::pop_back(node);
    return node;
  }
  std::unique_ptr<Node> result = std::make_unique<Node>();
  detail::pop(node, result.get(), valPtr);
  detail::relink(node, result.get());
  if (node == root_) {
    root_ = result.get();
  }
  delete node;
  return result.release();
}

template <typename K, typename T, typename C>
void ftl::map<K, T, C>::swap_vals(Node* lhs, pointer lhsPtr, Node* rhs,
    pointer rhsPtr)
{
  assert(lhs && lhsPtr && rhs && rhsPtr && "SwapVals: nullptr node given");
  value_type temp1(*lhsPtr);
  value_type temp2(*rhsPtr);
  lhsPtr->~value_type();
  rhsPtr->~value_type();
  new (lhsPtr) value_type(std::move(temp2));
  new (rhsPtr) value_type(std::move(temp1));
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node* ftl::map<K, T, C>::split(Node* node)
{
  assert(node && "Attempt to split nullptr node");
  assert(detail::size(node) > 2 && "Cannot split node with size less than 3");
  assert((!node->parent || detail::size(node->parent) < 3) &&
         "Split: filled parent");

  Node* left = nullptr;
  Node* right = nullptr;
  std::tie(left, right) = split_in_two(node);
  Node* parent = node->parent;
  if (!parent) {
    parent = new Node;
    parent->children[0] = node;
    root_ = parent;
  }
  parent = emplace_to_node(parent, *(node->begin + 1));
  auto& children = parent->children;
  *std::remove(children.begin(), children.end(), node) = nullptr;
  delete node;
  auto it = std::find(children.begin(), children.end(), nullptr);
  *(it++) = detail::update_parent(left);
  *(it++) = detail::update_parent(right);
  struct NodeComparator
  {
    bool operator()(const Node* lhs, const Node* rhs) const
    {
      return comp(*(lhs->begin), *(rhs->begin));
    }
    value_compare comp;
  };
  std::sort(children.begin(), it, NodeComparator{ value_comp() });
  return detail::update_parent(parent);
}

template <typename K, typename T, typename C>
std::tuple<typename ftl::map<K, T, C>::Node*, typename ftl::map<K, T, C>::Node*>
ftl::map<K, T, C>::split_in_two(const Node* node)
{
  assert(node && "splitInTwo: nullptr node given");
  assert(detail::size(node) == 3 && "splitInTwo: node must be filled");
  std::unique_ptr<Node> left = std::make_unique<Node>();
  std::unique_ptr<Node> right = std::make_unique<Node>();
  emplace_to_node(left.get(), *node->begin);
  emplace_to_node(right.get(), *(node->end - 1));
  split_children(node, left.get(), right.get());
  return std::make_tuple(left.release(), right.release());
}

template <typename K, typename T, typename C>
void ftl::map<K, T, C>::split_children(const Node* src, Node* left,
    Node* right) const noexcept
{
  assert(src && left && right && "SplitChildren: nullptr node given");
  auto mid = src->children.begin() + (src->children.size() / 2);
  std::copy(src->children.begin(), mid, left->children.begin());
  std::copy(mid, src->children.end(), right->children.begin());
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::pointer
ftl::map<K, T, C>::find_key(const Node* node, const key_type& key) const
{
  assert(node && "FindKey: nullptr node given");
  struct KeyEqual
  {
    bool operator()(const_reference val) const
    {
      return !comp(val.first, key) && !comp(key, val.first);
    }
    C comp;
    const key_type& key;
  };
  return std::find_if(node->begin, node->end, KeyEqual{ key_comp(), key });
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::find_target(Node* hint, const key_type& key) const
{
  assert(!empty() && "Attempt to find target node in empty tree");
  Node* current = validate_hint(hint, key);
  while (!detail::is_leaf(current) && find_key(current, key) == current->end) {
    const size_t currSize = detail::size(current);
    if (comparator_(key, current->begin->first)) {
      current = current->children[0];
    } else if (currSize == 1 || comparator_(key, (current->begin + 1)->first)) {
      current = current->children[1];
    } else {
      current = current->children[2];
    }
  }
  return current;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::validate_hint(Node* hint, const key_type& key) const
{
  if (!hint || detail::is_empty(hint) || hint == root_) {
    return root_;
  }
  bool isValid = true;
  bool isMid = false;
  const Node* current = hint;
  while (current->parent && isValid && !isMid) {
    const key_type& parFirst = current->parent->begin->first;
    const key_type& parLast = (current->parent->end - 1)->first;
    isMid = detail::is_middle(current);
    isValid = isMid && !(key_comp()(key, parFirst) || key_comp()(parLast, key));
    isValid =
        isValid || (detail::is_left(current) && !key_comp()(parFirst, key));
    isValid =
        isValid || (detail::is_right(current) && !key_comp()(key, parLast));
    current = current->parent;
  }
  return isValid ? hint : root_;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node* ftl::map<K, T, C>::fix_underflow(Node* node)
{
  assert(node && "fixUnderflow called with nullptr");
  assert(detail::is_empty(node) && "fixUnderflow called on non-empty node");
  if (node == root_) {
    return fix_root_underflow(node);
  }
  if (is_mergeable(node)) {
    return merge(node)->parent;
  }
  return redistribute(node)->parent;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::fix_root_underflow(Node* root)
{
  assert(root && "fixRootUnderflow: nullptr node given");
  assert(root == root_ && "fixRootUnderflow: non-root node given");
  if (detail::is_leaf(root)) {
    delete std::exchange(root_, nullptr);
    return root_;
  }
  Node* newRoot = root_->children[0];
  newRoot->parent = nullptr;
  delete std::exchange(root_, newRoot);
  return root_;
}

template <typename K, typename T, typename C>
bool ftl::map<K, T, C>::is_mergeable(const Node* node) const
{
  assert(node && "IsMergeable: nullptr node given");
  assert(node->parent && "IsMergeable: root node given");
  if (detail::is_three(node->parent)) {
    return false;
  }
  const auto getRight = detail::get_right_sibling<const Node*>;
  const auto getLeft = detail::get_left_sibling<const Node*>;
  return !detail::is_three(
      detail::is_left(node) ? getRight(node) : getLeft(node));
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node* ftl::map<K, T, C>::merge(Node* node)
{
  assert(node && "Merge: nullptr node given");
  assert(node->parent && detail::is_empty(node) &&
         "Merge: empty or root node given");
  assert(!detail::is_three(node->parent) && "Merge: invalid node given");
  Node* parent = node->parent;
  using detail::get_left_sibling;
  using detail::get_right_sibling;
  Node* sibling =
      detail::is_left(node) ? get_right_sibling(node) : get_left_sibling(node);
  sibling = emplace_to_node(sibling, std::move_if_noexcept(*parent->begin));
  if (detail::is_right(node)) {
    sibling->children[2] = node->children[0];
  } else {
    sibling->children[2] =
        std::exchange(sibling->children[1], sibling->children[0]);
    sibling->children[0] = node->children[0];
  }
  *std::remove(parent->children.begin(), parent->children.end(), node) =
      nullptr;
  delete node;
  detail::clear(parent);
  return detail::update_parent(sibling);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node* ftl::map<K, T, C>::redistribute(Node* node)
{
  assert(node && "Redistribute: nullptr node given");
  assert(!is_mergeable(node) && "Redistribute: merge must be called");
  assert(detail::is_empty(node) && "Redistribute: non-empty node given");
  assert(node->parent && "Redistribute: root node given");
  const auto& children = node->parent->children;
  const auto end = std::find(children.begin(), children.end(), nullptr);
  if (std::any_of(children.begin(), end, detail::is_three<const Node*>)) {
    return borrow_from_sibling(node);
  }
  return give_to_sibling(node);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node* ftl::map<K, T, C>::give_to_sibling(Node* node)
{
  assert(node && "GiveToSibling: nullptr node given");
  assert(node->parent && "GiveToSibling: root node given");
  assert(detail::is_three(node->parent) && "GiveToSibling: invalid node given");
  const auto getRight = detail::get_right_sibling<Node*>;
  const auto getLeft = detail::get_left_sibling<Node*>;
  Node* parent = node->parent;
  Node* sibling = detail::is_left(node) ? detail::get_right_sibling(node)
                                        : detail::get_left_sibling(node);
  pointer parentPtr = detail::is_right(node) ? parent->end - 1 : parent->begin;
  sibling = emplace_to_node(sibling, *parentPtr);
  parent = erase_from_node(parent, parentPtr);
  if (detail::is_left(node)) {
    sibling->children[2] = sibling->children[1];
    sibling->children[1] = sibling->children[0];
    sibling->children[0] = node->children[0];
  } else {
    sibling->children[2] = node->children[0];
  }
  *std::remove(parent->children.begin(), parent->children.end(), node) =
      nullptr;
  delete node;
  return detail::update_parent(sibling);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::borrow_from_sibling(Node* node)
{
  assert(node && "BorrowFromSibling: nullptr node given");
  assert(node->parent && "BorrowFromSibling: root node given");
  Node* sibling = nullptr;
  const bool isMid = detail::is_middle(node);
  const bool fromLeftMid =
      isMid && detail::is_three(detail::get_right_sibling(node));
  const bool fromLeft = detail::is_left(node) || fromLeftMid;
  if (fromLeft) {
    sibling = detail::get_right_sibling(node);
    sibling =
        detail::is_three(sibling) ? sibling : borrow_from_sibling(sibling);
    node = borrow_from_right(node, sibling);
    sibling = detail::get_right_sibling(node);
    node->children[detail::size(node)] = sibling->children[0];
    sibling->children[0] = sibling->children[1];
    sibling->children[1] = sibling->children[2];
  } else {
    sibling = detail::get_left_sibling(node);
    sibling =
        detail::is_three(sibling) ? sibling : borrow_from_sibling(sibling);
    node = borrow_from_left(node, sibling);
    sibling = detail::get_left_sibling(node);
    node->children[2] = node->children[1];
    node->children[1] = node->children[0];
    node->children[0] = sibling->children[2];
  }
  sibling->children[2] = nullptr;
  return detail::update_parent(node);
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::borrow_from_left(Node* node, Node* sibling)
{
  assert(node && sibling && "BorrowFromLeft: nullptr given");
  assert(
      node->parent == sibling->parent && "BorrowFromLeft: siblings expected");
  assert(node != sibling && is_three(sibling) &&
         "BorrowFromLeft: invalid sibling");
  assert(!detail::is_left(node) && "BorrowFromLeft: left node given");
  Node* parent = node->parent;
  pointer const parentPtr =
      detail::is_right(node) ? parent->end - 1 : parent->begin;
  node = emplace_to_node(node, *parentPtr);
  parent = erase_from_node(parent, parentPtr);
  parent = emplace_to_node(parent, *(sibling->end - 1));
  sibling = erase_from_node(sibling, sibling->end - 1);
  return node;
}

template <typename K, typename T, typename C>
typename ftl::map<K, T, C>::Node*
ftl::map<K, T, C>::borrow_from_right(Node* node, Node* sibling)
{
  assert(node && sibling && "BorrowFromRight: nullptr given");
  assert(
      node->parent == sibling->parent && "BorrowFromRight: siblings expected");
  assert(node != sibling && is_three(sibling) &&
         "BorrowFromRight: invalid sibling");
  assert(!detail::is_right(node) && "BorrowFromRight: left node given");
  Node* parent = node->parent;
  pointer const parentPtr =
      detail::is_left(node) ? parent->begin : parent->end - 1;
  node = emplace_to_node(node, *parentPtr);
  parent = erase_from_node(parent, parentPtr);
  parent = emplace_to_node(parent, *sibling->begin);
  sibling = erase_from_node(sibling, sibling->begin);
  return node;
}

template <typename K, typename T, typename C>
bool ftl::operator==(const map<K, T, C>& lhs, const map<K, T, C>& rhs)
{
  const bool sameSize = lhs.size() == rhs.size();
  return sameSize && compare(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename K, typename T, typename C>
bool ftl::operator!=(const map<K, T, C>& lhs, const map<K, T, C>& rhs)
{
  return !(lhs == rhs);
}

template <typename K, typename T, typename C>
bool ftl::operator<(const map<K, T, C>& lhs, const map<K, T, C>& rhs)
{
  return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
      rhs.end());
}

template <typename K, typename T, typename C>
bool ftl::operator>(const map<K, T, C>& lhs, const map<K, T, C>& rhs)
{
  return rhs < lhs;
}

template <typename K, typename T, typename C>
bool ftl::operator<=(const map<K, T, C>& lhs, const map<K, T, C>& rhs)
{
  return !(rhs < lhs);
}

template <typename K, typename T, typename C>
bool ftl::operator>=(const map<K, T, C>& lhs, const map<K, T, C>& rhs)
{
  return !(lhs < rhs);
}

template <typename K, typename T, typename C>
void ftl::swap(map<K, T, C>& lhs, map<K, T, C>& rhs) noexcept(
    noexcept(lhs.swap(rhs)))
{
  lhs.swap(rhs);
}

#endif
