// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_INTERNAL_COMPRESSED_PAIR_HPP
#define FTL_INTERNAL_COMPRESSED_PAIR_HPP

#include <cstddef>
#include <type_traits>

namespace ftl {
  namespace detail {

    template <typename, size_t, bool>
    class compressed_pair_element;

    template <typename, typename>
    class compressed_pair;

    template <typename T1, typename T2>
    void swap(compressed_pair<T1, T2>&, compressed_pair<T1, T2>&) noexcept;
  }
}

template <typename T, size_t,
    bool isEmpty = std::is_empty<T>::value && !std::is_final<T>::value>
class ftl::detail::compressed_pair_element
{
public:
  compressed_pair_element() = default;
  compressed_pair_element(const T& val) : value_(val) {}

  T& get() noexcept { return value_; }
  const T& get() const noexcept { return value_; }

private:
  T value_;
};

template <typename T, size_t Index>
class ftl::detail::compressed_pair_element<T, Index, true> : private T
{
public:
  compressed_pair_element() : T() {}
  compressed_pair_element(const T& val) : T(val) {}

  T& get() noexcept { return static_cast<T>(*this); }
  const T& get() const noexcept { return static_cast<T>(*this); }
};

template <typename T1, typename T2>
class ftl::detail::compressed_pair final :
  private compressed_pair_element<T1, 0>,
  private compressed_pair_element<T2, 1>
{
public:
  using Base1 = compressed_pair_element<T1, 0>;
  using Base2 = compressed_pair_element<T2, 1>;

  compressed_pair() : Base1(), Base2() {}
  compressed_pair(const T1& t1, const T2& t2) : Base1(t1), Base2(t2) {}

  T1& first() noexcept { return static_cast<Base1>(*this).get(); }
  const T1& first() const noexcept { return static_cast<Base1>(*this).get(); }

  T2& second() noexcept { return static_cast<Base2>(*this).get(); }
  const T2& second() const noexcept { return static_cast<Base2>(*this).get(); }

  void swap(const compressed_pair& rhs) noexcept
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
