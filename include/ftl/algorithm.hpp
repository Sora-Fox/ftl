// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_ALGORITHMS_HPP
#define FTL_ALGORITHMS_HPP

#include <functional>
#include <iterator>
#include <utility>

namespace ftl {
  template <typename InputIt, typename Comp = std::less<>>
  bool lexicographical_compare(InputIt first1, InputIt last1, InputIt first2,
      InputIt last2, Comp comp = Comp{})
  {
    for (; (first1 != last1) && (first2 != last2); ++first1, ++first2) {
      if (comp(*first1, *first2)) {
        return true;
      }
      if (comp(*first2, *first1)) {
        return false;
      }
    }
    return (first1 == last1) && (first2 != last2);
  }

  template <typename InputIt1, typename InputIt2,
      typename Comp = std::equal_to<>>
  bool compare(InputIt1 first1, const InputIt1 last1, InputIt2 first2,
      Comp comp = Comp{})
  {
    for (; first1 != last1; ++first1, ++first2) {
      if (!(comp(*first1, *first2))) {
        return false;
      }
    }
    return true;
  }

  template <typename InputIt, typename ResultIt>
  ResultIt uninitialized_copy(InputIt first, const InputIt last, ResultIt out)
  {
    using value_type = typename std::iterator_traits<ResultIt>::value_type;
    ResultIt current = out;
    try {
      for (; first != last; ++first, ++current) {
        new (std::addressof(*current)) value_type(*first);
      }
      return current;
    } catch (...) {
      for (; out != current; ++out) {
        out->~value_type();
      }
      throw;
    }
  }

  template <typename FwdIt, typename T>
  FwdIt uninitialized_fill(FwdIt first, const FwdIt last, const T& value)
  {
    using value_type = typename std::iterator_traits<FwdIt>::value_type;
    FwdIt current = first;
    try {
      for (; current != last; ++current) {
        new (std::addressof(*current)) value_type(value);
      }
      return current;
    } catch (...) {
      for (; first != current; ++first) {
        first->~value_type();
      }
      throw;
    }
  }

  template <typename FwdIt>
  FwdIt destroy(FwdIt first, const FwdIt last) noexcept
  {
    using value_type = typename std::iterator_traits<FwdIt>::value_type;
    for (; first != last; ++first) {
      std::addressof(*first)->~value_type();
    }
    return first;
  }

  template <typename InputIt, typename OutputIt>
  OutputIt move_range(InputIt first, const InputIt last, OutputIt out)
  {
    for (; first != last; ++out, ++first) {
      *out = std::move(*first);
    }
    return out;
  }

  template <typename BidirIt, typename ResultIt>
  ResultIt move_backward(const BidirIt first, BidirIt last, ResultIt out)
  {
    for (; first != last;) {
      *(--out) = std::move(*(--last));
    }
    return out;
  }
}

#endif
