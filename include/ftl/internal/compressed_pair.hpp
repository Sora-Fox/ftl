// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_INTERNAL_COMPRESSED_PAIR_HPP
#define FTL_INTERNAL_COMPRESSED_PAIR_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace ftl {
  namespace detail {

    template <typename T, size_t,
        bool isEmpty = std::is_empty<T>::value && !std::is_final<T>::value>
    class compressed_pair_element;

    template <typename, typename>
    class compressed_pair;

    template <typename T1, typename T2>
    void swap(compressed_pair<T1, T2>&, compressed_pair<T1, T2>&) noexcept;
  }
}

template <typename T, size_t Index>
class ftl::detail::compressed_pair_element<T, Index, false>
{
private:
  T value_;

public:
  compressed_pair_element() = default;

  template <typename U>
  explicit compressed_pair_element(U&& val) : value_(std::forward<U>(val))
  {
  }

  T& get() noexcept { return value_; }
  const T& get() const noexcept { return value_; }
};

template <typename T, size_t Index>
class ftl::detail::compressed_pair_element<T, Index, true> : private T
{
public:
  compressed_pair_element() = default;

  template <typename U>
  explicit compressed_pair_element(U&& val) : T(std::forward<U>(val))
  {
  }

  T& get() noexcept { return *this; }
  const T& get() const noexcept { return *this; }
};

template <typename T1, typename T2>
class ftl::detail::compressed_pair final :
  private compressed_pair_element<T1, 0>,
  private compressed_pair_element<T2, 1>
{
private:
  using Base1 = compressed_pair_element<T1, 0>;
  using Base2 = compressed_pair_element<T2, 1>;

public:
  compressed_pair() = default;

  template <typename U1, typename U2>
  explicit compressed_pair(U1&& first, U2&& second) :
    Base1(std::forward<U1>(first)),
    Base2(std::forward<U2>(second))
  {
  }

  compressed_pair(const compressed_pair& rhs) :
    Base1(rhs.first()),
    Base2(rhs.second())
  {
  }

  compressed_pair(compressed_pair&& rhs) noexcept :
    Base1(std::move(rhs.first())),
    Base2(std::move(rhs.second()))
  {
  }

  compressed_pair& operator=(const compressed_pair& rhs)
  {
    if (this != &rhs) {
      first() = rhs.first();
      second() = rhs.second();
    }
    return *this;
  }

  compressed_pair& operator=(compressed_pair&& rhs) noexcept
  {
    if (this != &rhs) {
      first() = std::move(rhs.first());
      second() = std::move(rhs.second());
    }
    return *this;
  }

  T1& first() noexcept { return static_cast<Base1&>(*this).get(); }
  const T1& first() const noexcept
  {
    return static_cast<const Base1&>(*this).get();
  }

  T2& second() noexcept { return static_cast<Base2&>(*this).get(); }
  const T2& second() const noexcept
  {
    return static_cast<const Base2&>(*this).get();
  }

  void swap(compressed_pair& rhs) noexcept
  {
    using std::swap;
    swap(first(), rhs.first());
    swap(second(), rhs.second());
  }
};

template <typename T1, typename T2>
void ftl::detail::swap(compressed_pair<T1, T2>& lhs,
    compressed_pair<T1, T2>& rhs) noexcept
{
  lhs.swap(rhs);
}

#endif
