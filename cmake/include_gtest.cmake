if (FTL_DOWNLOAD_GTEST)
  include(FetchContent)
  FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.17.0
  )
  FetchContent_MakeAvailable(googletest)
else()
  find_package(GTest REQUIRED)
endif()

include(GoogleTest)

