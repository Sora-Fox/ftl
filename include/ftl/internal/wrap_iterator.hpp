// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_INTERNAL_WRAP_ITERATOR_HPP
#define FTL_INTERNAL_WRAP_ITERATOR_HPP

#include <iterator>
#include "config.hpp"

namespace ftl {
  template <typename T, typename Allocator>
  class vector;
}

namespace ftl {
  namespace detail {

    template <typename It>
    class wrap_iterator final
    {
    private:
      using traits = std::iterator_traits<It>;

    public:
      using iterator_type = It;
      using value_type = typename traits::value_type;
      using difference_type = typename traits::difference_type;
      using pointer = typename traits::pointer;
      using reference = typename traits::reference;
      using iterator_category = typename traits::iterator_category;
#if defined(FTL_CPP20_FEATURES)
      using iterator_concept = typename traits::iterator_concept;
#endif

    private:
      iterator_type i_;

    public:
      wrap_iterator() {}

      template <typename OtherIt,
          typename = typename std::enable_if<
              std::is_convertible<OtherIt, iterator_type>::value>::type>
      wrap_iterator(const wrap_iterator<OtherIt>& other) : i_(other.base()){};

      iterator_type base() const { return i_; }
      reference operator*() const { return *i_; }

      pointer operator->() const { return std::addressof(*i_); }

      reference operator[](difference_type n) const { return i_[n]; }

      wrap_iterator& operator++()
      {
        ++i_;
        return *this;
      }

      wrap_iterator operator++(int)
      {
        wrap_iterator temp = *this;
        ++(*this);
        return temp;
      }

      wrap_iterator& operator--()
      {
        --i_;
        return *this;
      }

      wrap_iterator operator--(int)
      {
        wrap_iterator temp = *this;
        --(*this);
        return temp;
      }

      wrap_iterator& operator+=(difference_type n)
      {
        i_ += n;
        return *this;
      }

      wrap_iterator& operator-=(difference_type n)
      {
        i_ -= n;
        return *this;
      }

    private:
      explicit wrap_iterator(iterator_type i) : i_(i) {}

      template <typename T>
      friend class wrap_iterator;

      template <typename T, typename Allocator>
      friend class ftl::vector;
    };

    template <typename It1, typename It2>
    bool
    operator==(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
    {
      return lhs.base() == rhs.base();
    }

    template <typename It1, typename It2>
    bool
    operator!=(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
    {
      return !(lhs == rhs);
    }

    template <typename It1, typename It2>
    bool operator<(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
    {
      return lhs.base() < rhs.base();
    }

    template <typename It1, typename It2>
    bool
    operator<=(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
    {
      return !(rhs < lhs);
    }

    template <typename It1, typename It2>
    bool operator>(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
    {
      return rhs < lhs;
    }

    template <typename It1, typename It2>
    bool
    operator>=(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
    {
      return !(lhs < rhs);
    }

    template <typename It>
    wrap_iterator<It> operator+(const wrap_iterator<It>& it,
        typename wrap_iterator<It>::difference_type n)
    {
      wrap_iterator<It> result = it;
      result += n;
      return result;
    }

    template <typename It>
    wrap_iterator<It> operator+(typename wrap_iterator<It>::difference_type n,
        const wrap_iterator<It>& it)
    {
      return it + n;
    }

    template <typename It1, typename It2>
    auto operator-(const wrap_iterator<It1>& lhs, const wrap_iterator<It2>& rhs)
        -> decltype(lhs.base() - rhs.base())
    {
      return lhs.base() - rhs.base();
    }

    template <typename It>
    wrap_iterator<It> operator-(const wrap_iterator<It>& it,
        typename wrap_iterator<It>::difference_type n)
    {
      wrap_iterator<It> result = it;
      result -= n;
      return result;
    }
  }
}

#endif
