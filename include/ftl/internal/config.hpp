// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_CONFIG_HPP
#define FTL_CONFIG_HPP

#ifdef _MSC_VER
#  define FTL_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#  define FTL_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#define FTL_NODISCARD [[nodiscard]]

#if defined(FTL_DISABLE_EXCEPTIONS)
#  define FTL_THROW(expr)                        \
    do {                                         \
      /* TODO: Handle exceptions in FTL_THROW */ \
    } while (0)
#else
#  define FTL_THROW(expr) throw(expr)
#endif

#define FTL_LIFETIMEBOUND [[clang::lifetimebound]]

#endif
