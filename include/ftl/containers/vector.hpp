// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_CONTAINERS_VECTOR_HPP
#define FTL_CONTAINERS_VECTOR_HPP

#include <initializer_list>
#include <iterator>
#include <memory>
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
    using AllocTraits_ = std::allocator_traits<allocator_type>;

  public:
    using pointer = typename AllocTraits_::pointer;
    using const_pointer = typename AllocTraits_::const_pointer;
    using size_type = typename AllocTraits_::size_type;
    using difference_type = typename AllocTraits_::difference_type;
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
    void pop_back();

    reference at(size_type);
    const_reference at(size_type) const;

    void assign(size_type, const_reference);
    template <typename InputIt, detail::enable_if_input_iterator<InputIt> = 0>
    void assign(InputIt, InputIt);
    void assign(std::initializer_list<value_type>);

    iterator insert(const_iterator, const_reference);
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
    pointer begin_;
    pointer end_;
    detail::compressed_pair<pointer, allocator_type> end_cap_alloc_;

    class Deleter;

    pointer& end_cap_() noexcept;
    allocator_type& alloc_() noexcept;
    const pointer& end_cap_() const noexcept;
    const allocator_type& alloc_() const noexcept;
  };

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(const vector& rhs) : vector(rhs.alloc_())
  {
    /* TODO: Should we allocate degree of 2 here instead of given size */
    /* TODO: Check max_size here */
    begin_ = alloc_().allocate(rhs.size());
    end_ = begin_;
    end_cap_() = begin_ + rhs.size();
    detail::exception_guard<Deleter> guard(Deleter(*this));
    /* TODO: Refactor */
    for (auto i = rhs.begin_; end_ != begin_ + rhs.size(); ++end_, ++i) {
      AllocTraits_::construct(alloc_(), end_, *i);
    }
    guard.complete();
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(vector&& rhs) noexcept :
    begin_(std::exchange(rhs.begin_, nullptr)),
    end_(std::exchange(rhs.end_, nullptr)),
    end_cap_alloc_(std::move(rhs.end_cap_alloc_))
  /* TODO: ensure compressed_pair has correct move constructor */
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
    /* TODO: Should we allocate degree of 2 here instead of given size */
    /* TODO: Check max_size here */
    begin_ = alloc_().allocate(size);
    end_ = begin_;
    end_cap_() = begin_ + size;
    detail::exception_guard<Deleter> guard(Deleter(*this));
    /* TODO: Refactor */
    for (; end_ != begin_ + size; ++end_) {
      AllocTraits_::construct(alloc_(), end_, value);
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
      push_back(*first);
    }
    guard.complete();
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::vector(std::initializer_list<value_type> list,
      const allocator_type& alloc) :
    vector(list.begin(), list.end(), alloc)
  {
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>::~vector()
  {
    clear();
    AllocTraits_::deallocate(alloc_(), begin_, capacity());
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>& vector<T, Allocator>::operator=(const vector&) &
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  vector<T, Allocator>& vector<T, Allocator>::operator=(vector&&) & noexcept
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::reference
  vector<T, Allocator>::operator[](size_type) noexcept
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::const_reference
  vector<T, Allocator>::operator[](size_type) const noexcept
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::reserve(size_type)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::resize(size_type, const_reference)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::shrink_to_fit()
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::clear() noexcept
  {
    for (; end_ != begin_; --end_) {
      AllocTraits_::destroy(alloc_(), end_ - 1);
    }
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::swap(vector&) noexcept
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::push_back(const_reference)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::pop_back()
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::reference vector<T, Allocator>::at(size_type)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::const_reference
  vector<T, Allocator>::at(size_type) const
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::assign(size_type, const_reference)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  void vector<T, Allocator>::assign(InputIt, InputIt)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  void vector<T, Allocator>::assign(std::initializer_list<value_type>)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator, const_reference)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator, size_type, const_reference)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  template <typename InputIt, detail::enable_if_input_iterator<InputIt>>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator, InputIt, InputIt)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::insert(const_iterator,
      std::initializer_list<value_type>)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  template <typename... Args>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::emplace(const_iterator, Args&&...)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  template <typename... Args>
  void vector<T, Allocator>::emplace_back(Args&&...)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::erase(const_iterator)
  {
    /* TODO: Implement this method */
  }

  template <typename T, typename Allocator>
  typename vector<T, Allocator>::iterator
  vector<T, Allocator>::erase(const_iterator, const_iterator)
  {
    /* TODO: Implement this method */
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
    /* TODO: Implement this method */
    // return alloc_().max_size();
  }

  template <typename T, typename Allocator>
  class vector<T, Allocator>::Deleter
  {
  public:
    Deleter(vector& v) : v_(v) {}
    void operator()()
    {
      v_.clear();
      AllocTraits_::deallocate(v_.alloc_(), v_.begin_, v_.capacity());
    }

  private:
    vector& v_;
  };

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
  void swap(vector<T, Allocator>&, vector<T, Allocator>&) noexcept
  {
    /* TODO: Implement this function */
  }

  template <typename T, typename Allocator>
  bool operator==(const vector<T, Allocator>&, const vector<T, Allocator>&)
  {
    /* TODO: Implement this function */
  }

#if !defined(FTL_CPP20_FEATURES)
  template <typename T, typename Allocator>
  bool operator!=(const vector<T, Allocator>&, const vector<T, Allocator>&)
  {
    /* TODO: Implement this function */
  }
#endif
}

#endif
