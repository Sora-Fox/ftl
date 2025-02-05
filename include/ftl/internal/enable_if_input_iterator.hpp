// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_INTERNAL_ENABLE_IF_INPUT_ITERATOR
#define FTL_INTERNAL_ENABLE_IF_INPUT_ITERATOR

#include <iterator>
#include <type_traits>

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

#endif
