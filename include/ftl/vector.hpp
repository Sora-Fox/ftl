// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_VECTOR_HPP
#define FTL_VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include "internal/type_traits.hpp"
#include "internal/wrap_iterator.hpp"

namespace ftl {
  template <typename T, typename Allocator = std::allocator<T>>
  class vector final
  {
  public:
    using value_type = T;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;

  private:
    using AllocTraits = std::allocator_traits<allocator_type>;

  public:
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    using iterator = detail::wrap_iterator<pointer>;
    using const_iterator = detail::wrap_iterator<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = typename AllocTraits::size_type;
    using difference_type = typename AllocTraits::difference_type;

    static_assert(std::is_same_v<typename AllocTraits::value_type, value_type>,
        "Allocator::value_type for ftl::vector must be same as T");
    static_assert(std::is_nothrow_default_constructible_v<allocator_type>,
        "Allocator for ftl::vector must be nothrow default constructible");
    static_assert(std::is_nothrow_copy_constructible_v<allocator_type>,
        "Allocator for ftl::vector must be nothrow copy constructible");
    static_assert(std::is_nothrow_move_constructible_v<allocator_type>,
        "Allocator for ftl::vector must be nothrow move constructible");
    static_assert(std::is_nothrow_swappable_v<allocator_type>,
        "Allocator for ftl::vector must be nothrow swappable");

    vector() noexcept = default;
    vector(const vector&);
    vector(vector&&) noexcept;
    vector(const vector&, const allocator_type&);
    vector(vector&&, const allocator_type&) noexcept;
    explicit vector(const allocator_type&) noexcept;
    explicit vector(size_type, const allocator_type& = {});
    vector(size_type, const_reference, const allocator_type& = {});
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    vector(InputIt, InputIt, const allocator_type& = {});
    vector(std::initializer_list<value_type>, const allocator_type& = {});
    ~vector();

    vector& operator=(const vector&);
    vector& operator=(vector&&) noexcept;
    vector& operator=(std::initializer_list<value_type>);

    FTL_NODISCARD iterator begin() noexcept;
    FTL_NODISCARD iterator end() noexcept;
    FTL_NODISCARD const_iterator begin() const noexcept;
    FTL_NODISCARD const_iterator end() const noexcept;
    FTL_NODISCARD reverse_iterator rbegin() noexcept;
    FTL_NODISCARD reverse_iterator rend() noexcept;
    FTL_NODISCARD const_iterator cbegin() const noexcept;
    FTL_NODISCARD const_iterator cend() const noexcept;
    FTL_NODISCARD const_reverse_iterator crbegin() const noexcept;
    FTL_NODISCARD const_reverse_iterator crend() const noexcept;

    FTL_NODISCARD size_type size() const noexcept;
    FTL_NODISCARD size_type max_size() const noexcept;
    FTL_NODISCARD size_type capacity() const noexcept;
    FTL_NODISCARD bool empty() const noexcept;
    void resize(size_type, const_reference = {});
    void reserve(size_type);
    void shrink_to_fit();

    FTL_NODISCARD reference operator[](size_type) noexcept;
    FTL_NODISCARD const_reference operator[](size_type) const noexcept;
    FTL_NODISCARD reference at(size_type);
    FTL_NODISCARD const_reference at(size_type) const;
    FTL_NODISCARD reference front() noexcept;
    FTL_NODISCARD const_reference front() const noexcept;
    FTL_NODISCARD reference back() noexcept;
    FTL_NODISCARD const_reference back() const noexcept;
    FTL_NODISCARD pointer data() noexcept;
    FTL_NODISCARD const_pointer data() const noexcept;

    void assign(size_type, const_reference);
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    void assign(InputIt, InputIt);
    void assign(std::initializer_list<value_type>);
    void push_back(const_reference);
    void push_back(value_type&&);
    void pop_back();

    iterator insert(const_iterator, const_reference);
    iterator insert(const_iterator, value_type&&);
    iterator insert(const_iterator, size_type, const_reference);
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    iterator insert(const_iterator, InputIt, InputIt);
    iterator insert(const_iterator, std::initializer_list<value_type>);
    iterator erase(const_iterator);
    iterator erase(const_iterator, const_iterator);

    void swap(vector&) noexcept;
    void clear() noexcept;

    template <typename... Args>
    iterator emplace(const_iterator, Args&&...);
    template <typename... Args>
    void emplace_back(Args&&...);

    FTL_NODISCARD allocator_type get_allocator() const noexcept;

  private:
    pointer begin_ = nullptr;
    pointer end_ = nullptr;
    pointer end_cap_ = nullptr;
    FTL_NO_UNIQUE_ADDRESS allocator_type alloc_{};

    void allocate(size_type);
    void deallocate() noexcept;

    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    void construct_at_end(InputIt, InputIt);
    template <typename... Args>
    void construct_at_end(size_type, Args&&...);
    void destroy_at_end(pointer) noexcept;

    template <typename... Args>
    void emplace_back_impl(Args&&...);
    template <typename... Args>
    pointer emplace_unsafe(pointer, Args&&...);

    void reallocate_storage(size_type);
    void move_right_uninitialized(pointer);
    void move_right(pointer, pointer);
    FTL_NODISCARD size_type growth_capacity(size_type) const;

    void throw_out_of_range() const;
    void throw_length_error() const;
  };

  template <typename T, typename A>
  vector<T, A>::vector(const vector& rhs) : vector(rhs, rhs.alloc_)
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(vector&& rhs) noexcept :
    begin_(std::exchange(rhs.begin_, nullptr)),
    end_(std::exchange(rhs.end_, nullptr)),
    end_cap_(std::exchange(rhs.end_cap_, nullptr)),
    alloc_(std::move(rhs.alloc_))
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(const vector& rhs, const allocator_type& alloc) :
    vector(alloc)
  {
    if (!rhs.empty()) {
      allocate(rhs.size());
      construct_at_end(rhs.begin_, rhs.end_);
    }
  }

  template <typename T, typename A>
  vector<T, A>::vector(vector&& rhs, const allocator_type& alloc) noexcept :
    begin_(std::exchange(rhs.begin_, nullptr)),
    end_(std::exchange(rhs.end_, nullptr)),
    end_cap_(std::exchange(rhs.end_cap_, nullptr)),
    alloc_(alloc)
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(const allocator_type& alloc) noexcept :
    begin_(nullptr),
    end_(nullptr),
    end_cap_(nullptr),
    alloc_(alloc)
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(const size_type size, const allocator_type& alloc) :
    vector(size, value_type(), alloc)
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(const size_type size, const_reference value,
      const allocator_type& alloc) :
    vector(alloc)
  {
    if (size != 0) {
      allocate(size);
      construct_at_end(size, value);
    }
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  vector<T, A>::vector(InputIt first, const InputIt last,
      const allocator_type& alloc) :
    vector(alloc)
  {
    for (; first != last; ++first) {
      emplace_back(*first);
    }
  }

  template <typename T, typename A>
  vector<T, A>::vector(const std::initializer_list<value_type> list,
      const allocator_type& alloc) :
    vector(alloc)
  {
    if (list.size() != 0) {
      allocate(list.size());
      construct_at_end(list.begin(), list.end());
    }
  }

  template <typename T, typename A>
  vector<T, A>::~vector()
  {
    deallocate();
  }

  template <typename T, typename A>
  vector<T, A>& vector<T, A>::operator=(const vector& rhs)
  {
    clear();
    reserve(rhs.size());
    construct_at_end(rhs.begin(), rhs.end());
    return *this;
  }

  template <typename T, typename A>
  vector<T, A>& vector<T, A>::operator=(vector&& rhs) noexcept
  {
    vector(std::move(rhs)).swap(*this);
    return *this;
  }

  template <typename T, typename A>
  vector<T, A>&
  vector<T, A>::operator=(const std::initializer_list<value_type> list)
  {
    clear();
    reserve(list.size());
    construct_at_end(list.begin(), list.end());
    return *this;
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator vector<T, A>::begin() noexcept
  {
    return iterator(begin_);
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator vector<T, A>::end() noexcept
  {
    return iterator(end_);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_iterator vector<T, A>::begin() const noexcept
  {
    return const_iterator(begin_);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_iterator vector<T, A>::end() const noexcept
  {
    return const_iterator(end_);
  }

  template <typename T, typename A>
  typename vector<T, A>::reverse_iterator vector<T, A>::rbegin() noexcept
  {
    return reverse_iterator(end());
  }

  template <typename T, typename A>
  typename vector<T, A>::reverse_iterator vector<T, A>::rend() noexcept
  {
    return reverse_iterator(begin());
  }

  template <typename T, typename A>
  typename vector<T, A>::const_iterator vector<T, A>::cbegin() const noexcept
  {
    return const_iterator(begin_);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_iterator vector<T, A>::cend() const noexcept
  {
    return const_iterator(end_);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reverse_iterator
  vector<T, A>::crbegin() const noexcept
  {
    return const_reverse_iterator(end());
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reverse_iterator
  vector<T, A>::crend() const noexcept
  {
    return const_reverse_iterator(begin());
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type vector<T, A>::size() const noexcept
  {
    return end_ - begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type vector<T, A>::max_size() const noexcept
  {
    using size_limits = std::numeric_limits<size_type>;
    using diff_limits = std::numeric_limits<difference_type>;
    const size_type alloc_max = AllocTraits::max_size(alloc_);
    constexpr size_type diff_max = static_cast<size_type>(diff_limits::max());
    constexpr size_type vals_max = size_limits::max() / sizeof(T);
    return std::min({ alloc_max, diff_max, vals_max });
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type vector<T, A>::capacity() const noexcept
  {
    return end_cap_ - begin_;
  }

  template <typename T, typename A>
  bool vector<T, A>::empty() const noexcept
  {
    return begin_ == end_;
  }

  template <typename T, typename A>
  void vector<T, A>::resize(const size_type new_size, const_reference value)
  {
    if (size() >= new_size) {
      destroy_at_end(begin_ + new_size);
      return;
    }
    if (capacity() < new_size) {
      reallocate_storage(growth_capacity(new_size));
    }
    construct_at_end(new_size - size(), value);
  }

  template <typename T, typename A>
  void vector<T, A>::reserve(const size_type new_capacity)
  {
    if (new_capacity <= capacity()) {
      return;
    }
    if (new_capacity > max_size()) {
      throw_length_error();
    }
    reallocate_storage(new_capacity);
  }

  template <typename T, typename A>
  void vector<T, A>::shrink_to_fit()
  {
    if (end_ == end_cap_) {
      return;
    }
    if (empty()) {
      deallocate();
      return;
    }
    reallocate_storage(size());
  }

  template <typename T, typename A>
  typename vector<T, A>::reference
  vector<T, A>::operator[](const size_type index) noexcept
  {
    assert(begin_ + index < end_ && "ftl::vector::operator[] out of range");
    return *(begin_ + index);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference
  vector<T, A>::operator[](const size_type index) const noexcept
  {
    assert(begin_ + index < end_ && "ftl::vector::operator[] out of range");
    return *(begin_ + index);
  }

  template <typename T, typename A>
  typename vector<T, A>::reference vector<T, A>::at(size_type index)
  {
    if (index >= size()) {
      throw_out_of_range();
    }
    return *(begin_ + index);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference
  vector<T, A>::at(const size_type index) const
  {
    if (index >= size()) {
      throw_out_of_range();
    }
    return *(begin_ + index);
  }

  template <typename T, typename A>
  typename vector<T, A>::reference vector<T, A>::front() noexcept
  {
    assert(!empty() && "ftl::vector::front() called on empty vector");
    return *begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference vector<T, A>::front() const noexcept
  {
    assert(!empty() && "ftl::vector::front() called on empty vector");
    return *begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::reference vector<T, A>::back() noexcept
  {
    assert(!empty() && "ftl::vector::back() called on empty vector");
    return *(end_ - 1);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference vector<T, A>::back() const noexcept
  {
    assert(!empty() && "ftl::vector::back() called on empty vector");
    return *(end_ - 1);
  }

  template <typename T, typename A>
  typename vector<T, A>::pointer vector<T, A>::data() noexcept
  {
    return begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::const_pointer vector<T, A>::data() const noexcept
  {
    return begin_;
  }

  template <typename T, typename A>
  void vector<T, A>::assign(const size_type size, const_reference value)
  {
    if (capacity() < size) {
      vector(size, value).swap(*this);
      return;
    }
    clear();
    construct_at_end(size, value);
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  void vector<T, A>::assign(const InputIt first, const InputIt last)
  {
    vector(first, last).swap(*this);
  }

  template <typename T, typename A>
  void vector<T, A>::assign(const std::initializer_list<value_type> list)
  {
    if (capacity() < list.size()) {
      vector(list).swap(*this);
      return;
    }
    clear();
    construct_at_end(list.begin(), list.end());
  }

  template <typename T, typename A>
  void vector<T, A>::push_back(const_reference value)
  {
    emplace_back(value);
  }

  template <typename T, typename A>
  void vector<T, A>::push_back(value_type&& value)
  {
    emplace_back(std::move(value));
  }

  template <typename T, typename A>
  void vector<T, A>::pop_back()
  {
    assert(!empty() && "ftl::vector::pop_back() called on empty vector");
    destroy_at_end(end_ - 1);
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator
  vector<T, A>::insert(const_iterator position, const_reference value)
  {
    return emplace(position, value);
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator
  vector<T, A>::insert(const_iterator position, value_type&& value)
  {
    return emplace(position, std::move(value));
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator vector<T, A>::insert(const_iterator position,
      size_type size, const_reference value)
  {
    // TODO: Refactor ftl::vector::insert
    size_type shift = position - begin();
    if (end_ + size > end_cap_) {
      reallocate_storage(growth_capacity(capacity() + size));
    }
    pointer pos = begin_ + shift;
    if (pos == end_) {
      construct_at_end(size, value);
    } else {
      for (; size != 0; --size) {
        pos = emplace_unsafe(pos, value) + 1;
      }
    }
    return iterator(begin_ + shift);
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  typename vector<T, A>::iterator vector<T, A>::insert(const_iterator position,
      InputIt first, const InputIt last)
  {
    const size_type shift = position - begin();
    for (; first != last; ++first, ++position) {
      position = emplace(position, *first);
    }
    return iterator(begin_ + shift);
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator vector<T, A>::insert(const_iterator position,
      const std::initializer_list<value_type> list)
  {
    size_type shift = position - begin();
    if (size() + list.size() > capacity()) {
      reallocate_storage(growth_capacity(size() + list.size()));
    }
    return insert(const_iterator{ begin() + shift }, list.begin(), list.end());
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator vector<T, A>::erase(const_iterator position)
  {
    return erase(position, position + 1);
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator
  vector<T, A>::erase(const_iterator first, const_iterator last)
  {
    pointer first_ptr = begin_ + (first - begin());
    pointer last_ptr = begin_ + (last - begin());
    const size_type count = last_ptr - first_ptr;
    if (count == 0) {
      return iterator(first_ptr);
    }
    std::move(last_ptr, end_, first_ptr);
    destroy_at_end(end_ - count);
    return iterator(first_ptr);
  }

  template <typename T, typename A>
  void vector<T, A>::swap(vector& rhs) noexcept
  {
    using std::swap;
    swap(begin_, rhs.begin_);
    swap(end_, rhs.end_);
    swap(end_cap_, rhs.end_cap_);
    swap(alloc_, rhs.alloc_);
  }

  template <typename T, typename A>
  void vector<T, A>::clear() noexcept
  {
    destroy_at_end(begin_);
  }

  template <typename T, typename A>
  template <typename... Args>
  typename vector<T, A>::iterator
  vector<T, A>::emplace(const_iterator position, Args&&... args)
  {
    if (position == end()) {
      emplace_back(std::forward<Args>(args)...);
      return iterator(end_ - 1);
    }
    size_type shift = position - begin();
    if (end_ == end_cap_) {
      reallocate_storage(growth_capacity(capacity() + 1));
    }
    pointer pos = begin_ + shift;
    return iterator(emplace_unsafe(pos, std::forward<Args>(args)...));
  }

  template <typename T, typename A>
  template <typename... Args>
  void vector<T, A>::emplace_back(Args&&... args)
  {
    if (end_ == end_cap_) {
      reallocate_storage(growth_capacity(capacity() + 1));
    }
    emplace_back_impl(std::forward<Args>(args)...);
  }

  template <typename T, typename A>
  typename vector<T, A>::allocator_type
  vector<T, A>::get_allocator() const noexcept
  {
    return alloc_;
  }

  template <typename T, typename A>
  void vector<T, A>::allocate(const size_type size)
  {
    assert(empty() && "ftl::vector::allocate called on non-empty vector");
    if (size > max_size()) {
      throw_length_error();
    }
    begin_ = AllocTraits::allocate(alloc_, size);
    end_ = begin_;
    end_cap_ = begin_ + size;
  }

  template <typename T, typename A>
  void vector<T, A>::deallocate() noexcept
  {
    if (begin_ != nullptr) {
      clear();
      AllocTraits::deallocate(alloc_, begin_, capacity());
      begin_ = nullptr;
      end_ = nullptr;
      end_cap_ = nullptr;
    }
  }

  template <typename T, typename A>
  template <typename... Args>
  void vector<T, A>::construct_at_end(const size_type size, Args&&... args)
  {
    for (size_type i = 0; i != size; ++i, ++end_) {
      AllocTraits::construct(alloc_, end_, args...);
    }
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  void vector<T, A>::construct_at_end(InputIt first, InputIt last)
  {
    for (; first != last; ++first, ++end_) {
      AllocTraits::construct(alloc_, end_, *first);
    }
  }

  template <typename T, typename A>
  void vector<T, A>::destroy_at_end(pointer new_end) noexcept
  {
    for (; end_ != new_end; --end_) {
      AllocTraits::destroy(alloc_, end_ - 1);
    }
  }

  template <typename T, typename A>
  void vector<T, A>::reallocate_storage(const size_type new_cap)
  {
    assert(new_cap >= size() &&
           "ftl::vector::reallocate_storage cannot reduce size");
    vector buffer(alloc_);
    buffer.allocate(new_cap);
    for (auto i = begin(), e = end(); i != e; ++i) {
      buffer.emplace_back_impl(std::move_if_noexcept(*i));
    }
    swap(buffer);
  }

  template <typename T, typename A>
  template <typename... Args>
  void vector<T, A>::emplace_back_impl(Args&&... args)
  {
    assert(end_ != end_cap_ && "ftl::vector::emplace_back_impl filled storage");
    AllocTraits::construct(alloc_, end_, std::forward<Args>(args)...);
    ++end_;
  }

  template <typename T, typename A>
  template <typename... Args>
  typename vector<T, A>::pointer
  vector<T, A>::emplace_unsafe(pointer position, Args&&... args)
  {
    move_right(position, position + 1);
    *position = value_type(std::forward<Args>(args)...);
    return position;
  }

  template <typename T, typename A>
  void vector<T, A>::move_right_uninitialized(pointer begin)
  {
    construct_at_end(end_ - begin, std::move(*begin));
  }

  template <typename T, typename A>
  void vector<T, A>::move_right(pointer first, pointer out)
  {
    size_type shift = out - first;
    pointer new_last = end_ - shift;
    move_right_uninitialized(new_last);
    std::move_backward(first, new_last, new_last + shift);
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type
  vector<T, A>::growth_capacity(size_type new_capacity) const
  {
    size_type max_sz = max_size();
    if (new_capacity > max_sz) {
      throw_length_error();
    }
    size_type cap = capacity();
    if (cap >= max_sz / 2) {
      return max_sz;
    }
    return std::max(cap * 2, new_capacity);
  }

  template <typename T, typename A>
  void vector<T, A>::throw_out_of_range() const
  {
    throw std::out_of_range("ftl::vector out_of_range");
  }

  template <typename T, typename A>
  void vector<T, A>::throw_length_error() const
  {
    throw std::length_error("ftl::vector length_error");
  }

  template <typename T, typename A>
  void
  swap(vector<T, A>& lhs, vector<T, A>& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }

  template <typename T, typename A>
  FTL_NODISCARD bool
  operator==(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    const bool is_same_size = lhs.size() == rhs.size();
    return is_same_size && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
  }

#if !defined(FTL_CPP20_FEATURES)

  template <typename T, typename A>
  FTL_NODISCARD bool
  operator!=(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return !(lhs == rhs);
  }

  template <typename T, typename A>
  FTL_NODISCARD bool operator<(const vector<T, A>& l, const vector<T, A>& r)
  {
    return std::lexicographical_compare(l.cbegin(), l.cend(), r.cbegin(),
        r.cend());
  }

  template <typename T, typename A>
  FTL_NODISCARD bool operator>(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return rhs < lhs;
  }

  template <typename T, typename A>
  FTL_NODISCARD bool
  operator<=(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return !(lhs > rhs);
  }

  template <typename T, typename A>
  FTL_NODISCARD bool
  operator>=(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return !(lhs < rhs);
  }

#else

  template <typename T, typename A>
  FTL_NODISCARD auto
  operator<=>(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(),
        rhs.cbegin(), rhs.cend());
  }

#endif
}

namespace std {
  template <typename T, typename A>
  struct hash<ftl::vector<T, A>>
  {
    size_t operator()(const ftl::vector<T, A>& vector) const
    {
      size_t seed = vector.size();
      for (const auto& elem : vector) {
        seed ^= hash<T>{}(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      return seed;
    }
  };
}

#endif
