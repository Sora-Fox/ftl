// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_CONTAINERS_VECTOR_HPP
#define FTL_CONTAINERS_VECTOR_HPP

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "../internal/compressed_pair.hpp"
#include "../internal/exception_guard.hpp"
#include "../internal/wrap_iterator.hpp"

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
    using reference = value_type&;
    using const_reference = const value_type&;
    using allocator_type = Allocator;

  private:
    using AllocTraits = std::allocator_traits<allocator_type>;

  public:
    using pointer = typename AllocTraits::pointer;
    using const_pointer = typename AllocTraits::const_pointer;
    using size_type = typename AllocTraits::size_type;
    using difference_type = typename AllocTraits::difference_type;
    using iterator = detail::wrap_iterator<pointer>;
    using const_iterator = detail::wrap_iterator<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    vector() : vector(allocator_type()) {}
    vector(const vector&);
    vector(vector&&) noexcept;
    vector(const allocator_type& alloc);
    vector(size_type, const allocator_type& = allocator_type());
    vector(size_type, const_reference,
        const allocator_type& = allocator_type());
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    vector(InputIt, InputIt, const allocator_type& = allocator_type());
    vector(std::initializer_list<value_type>,
        const allocator_type& = allocator_type());
    ~vector();

    vector& operator=(const vector&) &;
    vector& operator=(vector&&) & noexcept;
    reference operator[](size_type i) noexcept;
    const_reference operator[](size_type i) const noexcept;

    void reserve(size_type);
    void resize(size_type, const_reference = value_type());
    void shrink_to_fit();
    void clear() noexcept;
    void swap(vector&) noexcept;

    void push_back(const_reference);
    void push_back(value_type&&); // TODO: Add definition
    void pop_back();

    reference at(size_type);
    const_reference at(size_type) const;

    void assign(size_type, const_reference);
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    void assign(InputIt, InputIt);
    void assign(std::initializer_list<value_type>);

    iterator insert(const_iterator, const_reference);
    iterator insert(const_iterator, value_type&&); // TODO: Add definition
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

    reference front() noexcept { return *begin_; }
    reference back() noexcept { return *(end_ - 1); }
    pointer data() noexcept { return begin_; }
    const_reference front() const noexcept { return *begin_; }
    const_reference back() const noexcept { return *(end_ - 1); }
    const_pointer data() const noexcept { return begin_; }

    iterator begin() noexcept { return iterator(begin_); }
    iterator end() noexcept { return iterator(end_); }
    const_iterator cbegin() const noexcept { return const_iterator(begin_); }
    const_iterator cend() const noexcept { return const_iterator(end_); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    bool empty() const noexcept { return begin_ == end_; }
    size_type size() const noexcept { return end_ - begin_; }
    size_type capacity() const noexcept { return end_cap_() - begin_; }
    size_type max_size() const noexcept;
    allocator_type get_allocator() const noexcept { return alloc_(); }

  private:
    class Deleter;

    pointer begin_;
    pointer end_;
    detail::compressed_pair<pointer, allocator_type> end_cap_alloc_;

    void allocate(size_type);
    void deallocate() noexcept;

    template <typename... Args>
    void construct_at_end(Args&&...);
    void destroy_at_end() noexcept;

    void reallocate_storage(size_type);
    void move_range(pointer, pointer, pointer);
    size_type growth_capacity(size_type) const;

    void throw_out_of_range() const;
    void throw_length_error() const;

    pointer& end_cap_() noexcept;
    allocator_type& alloc_() noexcept;
    const pointer& end_cap_() const noexcept;
    const allocator_type& alloc_() const noexcept;
  };

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(const vector& rhs) : vector(rhs.alloc_())
  {
    allocate(rhs.size());
    detail::exception_guard<Deleter> guard(Deleter(*this));
    for (auto i = rhs.begin_, end = rhs.end_; i != end; ++i) {
      construct_at_end(*i);
    }
    guard.complete();
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(vector&& rhs) noexcept :
    begin_(std::exchange(rhs.begin_, nullptr)),
    end_(std::exchange(rhs.end_, nullptr)),
    end_cap_alloc_(std::move(rhs.end_cap_alloc_))
  {
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(const allocator_type& alloc) :
    begin_(nullptr),
    end_(nullptr),
    end_cap_alloc_(nullptr, alloc)
  {
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(size_type size, const allocator_type& alloc) :
    vector(size, value_type(), alloc)
  {
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(size_type size, const_reference value,
      const allocator_type& alloc) :
    vector(alloc)
  {
    allocate(size);
    detail::exception_guard<Deleter> guard(Deleter(*this));
    for (; end_ != begin_ + size;) {
      construct_at_end(value);
    }
    guard.complete();
  }

  template <typename T, typename Allocator>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  vector<T, Allocator>::vector(InputIt first, InputIt last,
      const allocator_type& alloc) :
    vector(alloc)
  {
    detail::exception_guard<Deleter> guard(Deleter(*this));
    for (; first != last; ++first) {
      emplace_back(*first);
    }
    guard.complete();
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(std::initializer_list<value_type> list,
      const allocator_type& alloc) :
    vector(alloc)
  {
    allocate(list.size());
    detail::exception_guard<Deleter> guard(Deleter(*this));
    for (auto i = list.begin(), end = list.end(); i != end; ++i) {
      construct_at_end(*i);
    }
    guard.complete();
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::~vector()
  {
    deallocate();
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>& vector<T, Allocator>::operator=(const vector& rhs) &
  {
    vector copy = rhs;
    swap(copy);
    return *this;
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>& vector<T, Allocator>::operator=(vector&& rhs) & noexcept
  {
    deallocate();
    swap(rhs);
    return *this;
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::reference
  vector<T, Allocator>::operator[](size_type index) noexcept
  {
    return *(begin_ + index);
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::const_reference
  vector<T, Allocator>::operator[](size_type index) const noexcept
  {
    return *(begin_ + index);
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::reserve(size_type new_capacity)
  {
    if (new_capacity <= capacity()) {
      return;
    }
    if (new_capacity > max_size()) {
      throw_length_error();
    }
    reallocate_storage(new_capacity);
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::resize(size_type new_size, const_reference value)
  {
    if (size() >= new_size) {
      for (pointer new_end = begin_ + new_size; end_ != new_end;) {
        destroy_at_end();
      }
      return;
    }
    if (capacity() < new_size) {
      reallocate_storage(growth_capacity(new_size));
    }
    for (pointer new_end = begin_ + new_size; end_ != new_end;) {
      construct_at_end(value);
    }
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::shrink_to_fit()
  {
    if (end_ == end_cap_()) {
      return;
    }
    reallocate_storage(size());
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::clear() noexcept
  {
    for (; end_ != begin_;) {
      destroy_at_end();
    }
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::swap(vector& rhs) noexcept
  {
    using std::swap;
    swap(begin_, rhs.begin_);
    swap(end_, rhs.end_);
    swap(end_cap_alloc_, rhs.end_cap_alloc_);
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::push_back(const_reference value)
  {
    emplace_back(value);
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::pop_back()
  {
    destroy_at_end();
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::reference
  vector<T, Allocator>::at(size_type index)
  {
    if (index >= size()) {
      throw_out_of_range();
    }
    return *(begin_ + index);
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::const_reference
  vector<T, Allocator>::at(size_type index) const
  {
    if (index >= size()) {
      throw_out_of_range();
    }
    return *(begin_ + index);
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::assign(size_type size, const_reference value)
  {
    vector tmp(size, value);
    swap(tmp);
  }

  template <typename T, typename Allocator>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  void vector<T, Allocator>::assign(InputIt first, InputIt last)
  {
    vector tmp(first, last);
    swap(tmp);
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::assign(std::initializer_list<value_type> list)
  {
    vector tmp(list);
    swap(tmp);
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator position, const_reference value)
  {
    return emplace(position, value);
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator position, size_type size,
      const_reference value)
  {
    /* TODO: Implement this method */
#if 0
    size_type shift = position - begin();
    if (end_ == end_cap_()) {
      reallocate_storage(growth_capacity(capacity() + size));
    }
    pointer pos = begin_ + shift;
    if (pos == end_) {
      for (size_type i = 0; i != size; ++i) {
        construct_at_end(value);
      }
    } else {
      move_range(pos, end_, pos + size); // move_range should update end_ to avoid leaks
    }
    return iterator(pos);
#endif
    return begin(); // for correct test running
  }

  template <typename T, typename Allocator>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator, InputIt, InputIt)
  {
    /* TODO: Implement this method */
    return begin(); // for correct test running
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator position,
      std::initializer_list<value_type> list)
  {
    // TODO: should we use another algorithm?
    return insert(position, list.begin(), list.end());
  }

  template <typename T, typename Allocator>
  template <typename... Args>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::emplace(const_iterator position, Args&&... args)
  {
    /* TODO: Implement this method */
#if 0
    size_type shift = position - begin();
    if (end_ == end_cap_()) {
      reallocate_storage(growth_capacity(capacity() + 1));
    }
    pointer pos = begin_ + shift;
    if (pos == end_) {
      construct_at_end(std::forward<Args>(args)...);
    } else {
      move_range(pos, end_, pos + 1);
      ++end_;
      AllocTraits::construct(alloc_(), pos, std::forward<Args>(args)...);
    }
    return iterator(pos);
#endif
    return begin(); // for correct test running
  }

  template <typename T, typename Allocator>
  template <typename... Args>
  void vector<T, Allocator>::emplace_back(Args&&... args)
  {
    if (end_ == end_cap_()) {
      reallocate_storage(growth_capacity(capacity() + 1));
    }
    construct_at_end(std::forward<Args>(args)...);
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::erase(const_iterator position)
  {
    return erase(position, ++position);
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::erase(const_iterator first, const_iterator last)
  {
    /* TODO: Implement this method */
    return begin(); // for correct test running
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::const_reverse_iterator
  vector<T, Allocator>::crbegin() const noexcept
  {
    return reverse_iterator(end());
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::const_reverse_iterator
  vector<T, Allocator>::crend() const noexcept
  {
    return reverse_iterator(begin());
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::size_type
  vector<T, Allocator>::max_size() const noexcept
  {
    return AllocTraits::max_size(alloc_());
  }

  template <typename T, typename Allocator>
  class vector<T, Allocator>::Deleter
  {
  public:
    Deleter(vector& v) : v_(v) {}
    void operator()() { v_.deallocate(); }

  private:
    vector& v_;
  };

  template <typename T, typename Allocator>
  void vector<T, Allocator>::allocate(size_type size)
  {
    if (size > max_size()) {
      throw_length_error();
    }
    begin_ = AllocTraits::allocate(alloc_(), size);
    end_ = begin_;
    end_cap_() = begin_ + size;
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::deallocate() noexcept
  {
    if (begin_ != nullptr) {
      clear();
      AllocTraits::deallocate(alloc_(), begin_, capacity());
      begin_ = end_ = end_cap_() = nullptr;
    }
  }

  template <typename T, typename Allocator>
  template <typename... Args>
  void vector<T, Allocator>::construct_at_end(Args&&... args)
  {
    AllocTraits::construct(alloc_(), end_, args...);
    ++end_;
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::destroy_at_end() noexcept
  {
    AllocTraits::destroy(alloc_(), end_ - 1);
    --end_;
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::reallocate_storage(size_type new_capacity)
  {
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

  template <typename T, typename Allocator>
  void
  vector<T, Allocator>::move_range(pointer first, pointer last, pointer out)
  {
    /* TODO: Implement this method */
#if 0
    if (first == out) {
      return;
    }
    if (first < out && out < last) {
      pointer src = last;
      pointer dest = out + (last - first);
      while (src != first) {
        --src;
        --dest;
        if (dest >= end_) {
          AllocTraits::construct(alloc_(), dest, std::move(*src));
        } else {
          *dest = std::move(*src);
        }
      }
      return;
    }
    for (; first != last; ++first, ++out) {
      if (out >= end_) {
        AllocTraits::construct(alloc_(), out, std::move(*first));
      } else {
        *out = std::move(*first);
      }
    }
#endif
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::size_type
  vector<T, Allocator>::growth_capacity(size_type new_capacity) const
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

  template <typename T, typename Allocator>
  void vector<T, Allocator>::throw_out_of_range() const
  {
    throw std::out_of_range("ftl::vector out_of_range");
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::throw_length_error() const
  {
    throw std::length_error("ftl::vector length_error");
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::pointer&
  vector<T, Allocator>::end_cap_() noexcept
  {
    return end_cap_alloc_.first();
  }

  template <typename T, typename Allocator>
  const typename vector<T, Allocator>::pointer&
  vector<T, Allocator>::end_cap_() const noexcept
  {
    return end_cap_alloc_.first();
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::allocator_type&
  vector<T, Allocator>::alloc_() noexcept
  {
    return end_cap_alloc_.second();
  }

  template <typename T, typename Allocator>
  const typename vector<T, Allocator>::allocator_type&
  vector<T, Allocator>::alloc_() const noexcept
  {
    return end_cap_alloc_.second();
  }

  template <typename T, typename Allocator>
  void swap(vector<T, Allocator>& lhs, vector<T, Allocator>& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  template <typename T, typename Allocator>
  bool
  operator==(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
  {
    const bool is_same_size = lhs.size() == rhs.size();
    return is_same_size && std::equal(lhs.begin(), lhs.end(), rhs.begin());
  }

#if !defined(FTL_CPP20_FEATURES)

  template <typename T, typename Allocator>
  bool
  operator!=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
  {
    return !(lhs == rhs);
  }

  template <typename T, typename Allocator>
  bool operator<(const vector<T, Allocator>& l, const vector<T, Allocator>& r)
  {
    return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
  }

  template <typename T, typename Allocator>
  bool
  operator>(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
  {
    return rhs < lhs;
  }

  template <typename T, typename Allocator>
  bool
  operator<=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
  {
    return !(lhs > rhs);
  }

  template <typename T, typename Allocator>
  bool
  operator>=(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
  {
    return !(lhs < rhs);
  }

#else

  // TODO: ensure that it's correct
  template <typename T, typename Allocator>
  auto
  operator<=>(const vector<T, Allocator>& lhs, const vector<T, Allocator>& rhs)
  {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
        rhs.begin(), rhs.end());
  }

#endif
}

#endif
