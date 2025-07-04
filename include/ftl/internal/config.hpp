// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_CONFIG_HPP
#define FTL_CONFIG_HPP

#ifdef __cplusplus
#  if __cplusplus >= 202002L
#    define FTL_CPP20_FEATURES
#  endif

#  if __cplusplus >= 201703L
#    define FTL_CPP17_FEATURES
#  endif

#  if __cplusplus >= 201402L
#    define FTL_CPP14_FEATURES
#  endif
#endif

#if defined(FTL_CPP14_FEATURES)
#  define FTL_CONSTEXPR_SINCE_CXX14 constexpr
#else
#  define FTL_CONSTEXPR_SINCE_CXX14
#endif

#ifdef _MSC_VER
#  define FTL_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#  define FTL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#if defined(FTL_CPP17_FEATURES)
#  define FTL_NODISCARD [[nodiscard]]
#else
#  define FTL_NODISCARD
#endif

#endif
