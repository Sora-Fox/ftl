// This file is part of the FTL Project, under the GNU General Public License
// v3.0. See https://www.gnu.org/licenses/gpl-3.0.txt for license information.
// SPDX-License-Identifier: GPL-3.0

#ifndef FTL_INTERNAL_EXCEPTION_GUARD
#define FTL_INTERNAL_EXCEPTION_GUARD

namespace ftl {
  namespace detail {

    template <typename Destructor>
    class exception_guard final
    {
    public:
      exception_guard(const exception_guard&) = delete;
      exception_guard(exception_guard&&) = delete;
      exception_guard& operator=(const exception_guard&) = delete;
      exception_guard& operator=(exception_guard&&) = delete;

      exception_guard(const Destructor& destructor) : destructor_(destructor) {}
      ~exception_guard()
      {
        if (!isCompleted_) {
          destructor_();
        }
      }
      void complete() noexcept { isCompleted_ = true; }

    private:
      bool isCompleted_ = false;
      Destructor destructor_;
    };
  }
}

#endif
