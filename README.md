# FTL

**FTL** (*Fox Template Library*) is a C++ library composed of templates focusing on **data structures**, **algorithms**, and other related topics. Parts of it contain implementations inspired by components of the C++ Standard Library.

The primary purpose of this project is to study and learn C++ by implementing concepts and exploring development tools. If you're at a similar stage of learning C++, you may find this code useful as a reference. Contributions are welcome, whether by adding your own implementations, improving existing ones, or sharing ideas to help the library grow.

---

## Usage

FTL is a library designed for learning purposes, not as a fully polished or feature complete solution, so use it carefully and thoughtfully.

### Integrating FTL with CMake

1. Use CMake FetchContent as follows:

```cmake
include(FetchContent)

FetchContent_Declare(
    ftl
    GIT_REPOSITORY https://github.com/Sora-Fox/ftl.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(ftl)
```

2. Link FTL to your target:

```cmake
add_executable(
    main
    main.cpp
)

target_link_libraries(
    main
    ftl
)
```

3. In your C++ code, include the core header to access the library's functionality:

```cpp
#include <ftl/core.hpp>
```

---

## License

This project is licensed under the **GPL v3** License. See the [LICENSE](./LICENSE) file for more details.

