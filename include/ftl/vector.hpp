// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_VECTOR_HPP
#define FTL_VECTOR_HPP

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include "internal/compressed_pair.hpp"
#include "internal/exception_guard.hpp"
#include "internal/wrap_iterator.hpp"

namespace ftl {
  namespace detail {

    template <typename...>
    using void_t = void;

    template <typename T, typename = void>
    struct is_input_iterator : std::false_type
    {
    };

    template <typename T>
    struct is_input_iterator<T,
        void_t<typename std::iterator_traits<T>::iterator_category>> :
      std::is_base_of<std::input_iterator_tag,
          typename std::iterator_traits<T>::iterator_category>
    {
    };

    template <typename Iterator>
    using enable_if_input_iterator =
        typename std::enable_if<is_input_iterator<Iterator>::value, int>::type;
  }
}

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

    vector();
    vector(const vector&);
    vector(vector&&) noexcept;
    vector(const vector&, const allocator_type&);
    vector(vector&&, const allocator_type&);
    explicit vector(const allocator_type&);
    explicit vector(size_type, const allocator_type& = allocator_type());
    vector(size_type, const_reference,
        const allocator_type& = allocator_type());
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    vector(InputIt, InputIt, const allocator_type& = allocator_type());
    vector(std::initializer_list<value_type>,
        const allocator_type& = allocator_type());
    ~vector();

    vector& operator=(const vector&);
    vector& operator=(vector&&) noexcept;
    reference operator[](size_type i) noexcept;
    const_reference operator[](size_type i) const noexcept;

    void reserve(size_type);
    void resize(size_type, const_reference = value_type());
    void shrink_to_fit();
    void clear() noexcept;
    void swap(vector&) noexcept;

    void push_back(const_reference);
    void push_back(value_type&&);
    void pop_back();

    reference at(size_type);
    const_reference at(size_type) const;

    void assign(size_type, const_reference);
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    void assign(InputIt, InputIt);
    void assign(std::initializer_list<value_type>);

    iterator insert(const_iterator, const_reference);
    iterator insert(const_iterator, value_type&&);
    iterator insert(const_iterator, size_type, const_reference);
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    iterator insert(const_iterator, InputIt, InputIt);
    iterator insert(const_iterator, std::initializer_list<value_type>);

    template <typename... Args>
    iterator emplace(const_iterator, Args&&...);
    template <typename... Args>
    void emplace_back(Args&&...);

    iterator erase(const_iterator);
    iterator erase(const_iterator, const_iterator);

    reference front() noexcept;
    reference back() noexcept;
    pointer data() noexcept;
    const_reference front() const noexcept;
    const_reference back() const noexcept;
    const_pointer data() const noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;
    size_type capacity() const noexcept;
    size_type max_size() const noexcept;
    allocator_type get_allocator() const noexcept;

  private:
    pointer begin_;
    pointer end_;
    detail::compressed_pair<pointer, allocator_type> end_cap_alloc_;

    void allocate(size_type);
    void deallocate() noexcept;

    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    void construct_at_end(InputIt, InputIt);
    template <typename... Args>
    void construct_at_end(size_type, Args&&...);
    void destroy_at_end(pointer) noexcept;

    template <typename... Args>
    pointer emplace_unsafe(pointer, Args&&...);

    void reallocate_storage(size_type);
    void move_right_uninitialized(pointer);
    void move_right(pointer, pointer);
    size_type growth_capacity(size_type) const;

    void throw_out_of_range() const;
    void throw_length_error() const;

    pointer& end_cap_() noexcept;
    allocator_type& alloc_() noexcept;
    const pointer& end_cap_() const noexcept;
    const allocator_type& alloc_() const noexcept;
  };

  template <typename T, typename A>
  vector<T, A>::vector(const vector& rhs) : vector(rhs.alloc_())
  {
    allocate(rhs.size());
    construct_at_end(rhs.begin_, rhs.end_);
  }

  template <typename T, typename A>
  vector<T, A>::vector(vector&& rhs) noexcept :
    begin_(std::exchange(rhs.begin_, nullptr)),
    end_(std::exchange(rhs.end_, nullptr)),
    end_cap_alloc_(std::move(rhs.end_cap_alloc_))
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(const allocator_type& alloc) :
    begin_(nullptr),
    end_(nullptr),
    end_cap_alloc_(nullptr, alloc)
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(size_type size, const allocator_type& alloc) :
    vector(size, value_type(), alloc)
  {
  }

  template <typename T, typename A>
  vector<T, A>::vector(size_type size, const_reference value,
      const allocator_type& alloc) :
    vector(alloc)
  {
    allocate(size);
    construct_at_end(size, value);
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  vector<T, A>::vector(InputIt first, InputIt last,
      const allocator_type& alloc) :
    vector(alloc)
  {
    for (; first != last; ++first) {
      emplace_back(*first);
    }
  }

  template <typename T, typename A>
  vector<T, A>::vector(std::initializer_list<value_type> list,
      const allocator_type& alloc) :
    vector(alloc)
  {
    allocate(list.size());
    construct_at_end(list.begin(), list.end());
  }

  template <typename T, typename A>
  vector<T, A>::~vector()
  {
    deallocate();
  }

  template <typename T, typename A>
  vector<T, A>& vector<T, A>::operator=(const vector& rhs)
  {
    vector copy = rhs;
    swap(copy);
    return *this;
  }

  template <typename T, typename A>
  vector<T, A>& vector<T, A>::operator=(vector&& rhs) noexcept
  {
    deallocate();
    swap(rhs);
    return *this;
  }

  template <typename T, typename A>
  typename vector<T, A>::reference
  vector<T, A>::operator[](size_type index) noexcept
  {
    assert(begin_ + index < end_ &&
           "ftl::vector::operator[] called with invalid index");
    return *(begin_ + index);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference
  vector<T, A>::operator[](size_type index) const noexcept
  {
    assert(begin_ + index < end_ &&
           "ftl::vector::operator[] called with invalid index");
    return *(begin_ + index);
  }

  template <typename T, typename A>
  void vector<T, A>::reserve(size_type new_capacity)
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
  void vector<T, A>::resize(size_type new_size, const_reference value)
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
  void vector<T, A>::shrink_to_fit()
  {
    if (end_ == end_cap_()) {
      return;
    }
    reallocate_storage(size());
  }

  template <typename T, typename A>
  void vector<T, A>::clear() noexcept
  {
    destroy_at_end(begin_);
  }

  template <typename T, typename A>
  void vector<T, A>::swap(vector& rhs) noexcept
  {
    using std::swap;
    swap(begin_, rhs.begin_);
    swap(end_, rhs.end_);
    swap(end_cap_alloc_, rhs.end_cap_alloc_);
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
    destroy_at_end(end_ - 1);
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
  typename vector<T, A>::const_reference vector<T, A>::at(size_type index) const
  {
    if (index >= size()) {
      throw_out_of_range();
    }
    return *(begin_ + index);
  }

  template <typename T, typename A>
  void vector<T, A>::assign(size_type size, const_reference value)
  {
    if (capacity() < size) {
      vector tmp(size, value);
      swap(tmp);
      return;
    }
    clear();
    construct_at_end(size, value);
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  void vector<T, A>::assign(InputIt first, InputIt last)
  {
    clear();
    for (; first != last; ++first) {
      emplace_back(*first);
    }
  }

  template <typename T, typename A>
  void vector<T, A>::assign(std::initializer_list<value_type> list)
  {
    if (capacity() < list.size()) {
      vector tmp(list);
      swap(tmp);
      return;
    }
    clear();
    construct_at_end(list.begin(), list.end());
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
    size_type shift = position - begin();
    if (end_ == end_cap_()) {
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
  typename vector<T, A>::iterator
  vector<T, A>::insert(const_iterator position, InputIt first, InputIt last)
  {
    size_type shift = position - cbegin();
    for (; first != last; ++first) {
      position = emplace(position, *first);
      ++position;
    }
    return iterator(begin_ + shift);
  }

  template <typename T, typename A>
  typename vector<T, A>::iterator vector<T, A>::insert(const_iterator position,
      std::initializer_list<value_type> list)
  {
    size_type shift = position - begin();
    if (size() + list.size() > capacity()) {
      reallocate_storage(growth_capacity(size() + list.size()));
    }
    pointer pos = begin_ + shift;
    if (pos == end_) {
      construct_at_end(list.begin(), list.end());
    } else {
      for (auto i = list.begin(), end = list.end(); i != end; ++i) {
        pos = emplace_unsafe(pos, *i) + 1;
      }
    }
    return iterator(begin_ + shift);
  }

  template <typename T, typename A>
  template <typename... Args>
  typename vector<T, A>::iterator
  vector<T, A>::emplace(const_iterator position, Args&&... args)
  {
    if (position == cend()) {
      emplace_back(std::forward<Args>(args)...);
      return iterator(end_ - 1);
    }
    size_type shift = position - cbegin();
    if (end_ == end_cap_()) {
      reallocate_storage(growth_capacity(capacity() + 1));
    }
    pointer pos = begin_ + shift;
    return iterator(emplace_unsafe(pos, std::forward<Args>(args)...));
  }

  template <typename T, typename A>
  template <typename... Args>
  void vector<T, A>::emplace_back(Args&&... args)
  {
    if (end_ == end_cap_()) {
      reallocate_storage(growth_capacity(capacity() + 1));
    }
    construct_at_end(1, std::forward<Args>(args)...);
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
    pointer first_ptr = begin_ + (first - cbegin());
    pointer last_ptr = begin_ + (last - cbegin());
    const size_type count = last_ptr - first_ptr;
    if (count == 0) {
      return iterator(first_ptr);
    }
    std::move(last_ptr, end_, first_ptr);
    destroy_at_end(end_ - count);
    return iterator(first_ptr);
  }

  template <typename T, typename A>
  typename vector<T, A>::reference vector<T, A>::front() noexcept
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
  typename vector<T, A>::pointer vector<T, A>::data() noexcept
  {
    return begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference vector<T, A>::front() const noexcept
  {
    assert(!empty() && "ftl::vector::front() called on empty vector");
    return *begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::const_reference vector<T, A>::back() const noexcept
  {
    assert(!empty() && "ftl::vector::back() called on empty vector");
    return *(end_ - 1);
  }

  template <typename T, typename A>
  typename vector<T, A>::const_pointer vector<T, A>::data() const noexcept
  {
    return begin_;
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
  bool vector<T, A>::empty() const noexcept
  {
    return begin_ == end_;
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type vector<T, A>::size() const noexcept
  {
    return end_ - begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type vector<T, A>::capacity() const noexcept
  {
    return end_cap_() - begin_;
  }

  template <typename T, typename A>
  typename vector<T, A>::size_type vector<T, A>::max_size() const noexcept
  {
    using size_limits = std::numeric_limits<size_type>;
    using diff_limits = std::numeric_limits<difference_type>;
    const size_type alloc_max = AllocTraits::max_size(alloc_());
    constexpr size_type diff_max = static_cast<size_type>(diff_limits::max());
    constexpr size_type bytes_max = size_limits::max() / sizeof(T);
    return std::min({ alloc_max, diff_max, bytes_max });
  }

  template <typename T, typename A>
  typename vector<T, A>::allocator_type
  get_vector<T, A>::allocator() const noexcept
  {
    return alloc_();
  }

  template <typename T, typename A>
  void vector<T, A>::allocate(size_type size)
  {
    assert(empty() && "ftl::vector::allocate called on non-empty vector");
    if (size > max_size()) {
      throw_length_error();
    }
    begin_ = AllocTraits::allocate(alloc_(), size);
    end_ = begin_;
    end_cap_() = begin_ + size;
  }

  template <typename T, typename A>
  void vector<T, A>::deallocate() noexcept
  {
    if (begin_ != nullptr) {
      clear();
      AllocTraits::deallocate(alloc_(), begin_, capacity());
      begin_ = nullptr;
      end_ = nullptr;
      end_cap_() = nullptr;
    }
  }

  template <typename T, typename A>
  template <typename... Args>
  void vector<T, A>::construct_at_end(size_type size, Args&&... args)
  {
    for (size_type i = 0; i != size; ++i, ++end_) {
      AllocTraits::construct(alloc_(), end_, args...);
    }
  }

  template <typename T, typename A>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  void vector<T, A>::construct_at_end(InputIt first, InputIt last)
  {
    for (; first != last; ++first, ++end_) {
      AllocTraits::construct(alloc_(), end_, *first);
    }
  }

  template <typename T, typename A>
  void vector<T, A>::destroy_at_end(pointer new_end) noexcept
  {
    for (; end_ != new_end; --end_) {
      AllocTraits::destroy(alloc_(), end_ - 1);
    }
  }

  template <typename T, typename A>
  void vector<T, A>::reallocate_storage(size_type new_capacity)
  {
    // TODO: too much responsibility: should be shrink storage and expand?
    pointer new_begin = AllocTraits::allocate(alloc_(), new_capacity);
    pointer new_end = new_begin;
    pointer new_end_cap = new_begin + new_capacity;
    auto deleter = [&]() {
      for (; new_end != new_begin; --new_end) {
        AllocTraits::destroy(alloc_(), new_end);
      }
      AllocTraits::deallocate(alloc_(), new_begin, new_capacity);
    };

    detail::exception_guard<decltype(deleter)> guard(deleter);
    for (pointer i = begin_, end = new_begin + std::min(new_capacity, size());
        new_end != end; ++new_end, ++i) {
      AllocTraits::construct(alloc_(), new_end, std::move_if_noexcept(*i));
    }
    guard.complete();
    deallocate();

    begin_ = new_begin;
    end_ = new_end;
    end_cap_() = new_end_cap;
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
  typename vector<T, A>::pointer& vector<T, A>::end_cap_() noexcept
  {
    return end_cap_alloc_.first();
  }

  template <typename T, typename A>
  const typename vector<T, A>::pointer& vector<T, A>::end_cap_() const noexcept
  {
    return end_cap_alloc_.first();
  }

  template <typename T, typename A>
  typename vector<T, A>::allocator_type& vector<T, A>::alloc_() noexcept
  {
    return end_cap_alloc_.second();
  }

  template <typename T, typename A>
  const typename vector<T, A>::allocator_type&
  vector<T, A>::alloc_() const noexcept
  {
    return end_cap_alloc_.second();
  }

  template <typename T, typename A>
  void
  swap(vector<T, A>& lhs, vector<T, A>& rhs) noexcept(noexcept(lhs.swap(rhs)))
  {
    lhs.swap(rhs);
  }

  template <typename T, typename A>
  bool operator==(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    const bool is_same_size = lhs.size() == rhs.size();
    return is_same_size && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
  }

#if !defined(FTL_CPP20_FEATURES)

  template <typename T, typename A>
  bool operator!=(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return !(lhs == rhs);
  }

  template <typename T, typename A>
  bool operator<(const vector<T, A>& l, const vector<T, A>& r)
  {
    return std::lexicographical_compare(l.cbegin(), l.cend(), r.cbegin(),
        r.cend());
  }

  template <typename T, typename A>
  bool operator>(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return rhs < lhs;
  }

  template <typename T, typename A>
  bool operator<=(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return !(lhs > rhs);
  }

  template <typename T, typename A>
  bool operator>=(const vector<T, A>& lhs, const vector<T, A>& rhs)
  {
    return !(lhs < rhs);
  }

#else

  template <typename T, typename A>
  auto operator<=>(const vector<T, A>& lhs, const vector<T, A>& rhs)
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
