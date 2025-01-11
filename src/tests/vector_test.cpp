#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ftl/core.hpp>
#include <gtest/gtest.h>

namespace test {
  using VectorT = ftl::vector<double>;

  void AssertInvariants(const VectorT& vector, VectorT::size_type size)
  {
    auto beg = vector.cbegin();
    auto end = vector.cend();
    ASSERT_EQ(vector.empty(), size == 0);
    ASSERT_EQ(beg == end, size == 0);
    ASSERT_EQ(vector.data() == nullptr, vector.capacity() == 0);
    ASSERT_EQ(vector.size(), size);
    ASSERT_GE(vector.capacity(), size);
    ASSERT_EQ(std::distance(beg, end), size);
    ASSERT_LE(beg, end);
  }

  class VectorTest : public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      filled = VectorT(100);
      empty = VectorT();
      std::iota(filled.begin(), filled.end(), 0);
      copy = filled;
    }
    VectorT filled;
    VectorT empty;
    VectorT copy;
  };

  TEST(VectorConstructor, Default)
  {
    VectorT empty;
    AssertInvariants(empty, 0);
  }

  TEST(VectorConstructor, Allocator)
  {
    std::allocator<VectorT::value_type> alloc;
    VectorT vector(alloc);
    AssertInvariants(vector, 0);
  }

  TEST(VectorConstructor, Size)
  {
    size_t size = 100;
    VectorT::value_type val{};
    VectorT vector(size);
    AssertInvariants(vector, size);
    EXPECT_TRUE(std::all_of(vector.begin(), vector.end(),
        [&val](const auto& x) { return x == val; }));
  }

  TEST(VectorConstructor, SizeAndValue)
  {
    size_t size = 100;
    VectorT::value_type val(7);
    VectorT vector(size, val);
    AssertInvariants(vector, size);
    EXPECT_TRUE(std::all_of(vector.begin(), vector.end(),
        [&val](const auto& x) { return x == val; }));
  }

  TEST(VectorConstructor, Iterators)
  {
    auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    VectorT vector(values.begin(), values.end());
    AssertInvariants(vector, values.size());
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), values.begin()));
  }

  TEST(VectorConstructor, InitializerList)
  {
    std::initializer_list<VectorT::value_type> values = { 1, 2, 3, 4, 5, 6, 7 };
    VectorT vector(values);
    AssertInvariants(vector, values.size());
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), values.begin()));
  }

  TEST(VectorConstructor, Copy)
  {
    VectorT vector(100, 7);
    VectorT copy(vector);
    AssertInvariants(copy, vector.size());
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), copy.begin()));
  }

  TEST(VectorConstructor, Move)
  {
    VectorT vector(100, 7);
    VectorT copy(vector);
    VectorT moved(std::move(vector));
    AssertInvariants(moved, copy.size());
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), moved.begin()));
  }

  TEST(VectorAssignmentOperator, Move)
  {
    VectorT vector(100, 7);
    VectorT copy(vector);
    VectorT moved;
    moved = std::move(vector);
    AssertInvariants(moved, copy.size());
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), moved.begin()));
  }

  TEST(VectorAssignmentOperator, Copy)
  {
    VectorT vector(100, 7);
    VectorT copy;
    copy = vector;
    AssertInvariants(copy, vector.size());
    EXPECT_TRUE(std::equal(vector.begin(), vector.end(), copy.begin()));
  }

  TEST_F(VectorTest, PushBack)
  {
    auto val = VectorT::value_type(77);
    filled.push_back(val);
    AssertInvariants(filled, copy.size() + 1);
    EXPECT_EQ(filled.back(), val);
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
  }

  TEST_F(VectorTest, PushBackEmpty)
  {
    auto val = VectorT::value_type(77);
    empty.push_back(val);
    AssertInvariants(empty, 1);
    EXPECT_EQ(empty.back(), val);
    EXPECT_EQ(empty.front(), val);
  }

  TEST_F(VectorTest, PopBack)
  {
    filled.pop_back();
    AssertInvariants(filled, copy.size() - 1);
  }

  TEST_F(VectorTest, Reserve)
  {
    auto newCapacity = filled.capacity() * 2;
    filled.reserve(newCapacity);
    AssertInvariants(filled, copy.size());
    EXPECT_GE(filled.capacity(), newCapacity);
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
  }

  TEST_F(VectorTest, ResizeWithoutReallocation)
  {
    filled.reserve(filled.capacity() * 2);
    auto newSize = filled.capacity() - 1;
    filled.resize(newSize);
    AssertInvariants(filled, newSize);
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
  }

  TEST_F(VectorTest, ResizeWithReallocation)
  {
    auto newSize = filled.capacity() * 2;
    filled.resize(newSize);
    AssertInvariants(filled, newSize);
    EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
  }

  TEST_F(VectorTest, ShrinkToFit)
  {
    auto capacity = filled.capacity();
    auto newCapacity = capacity * 2;
    filled.reserve(newCapacity);
    filled.shrink_to_fit();
    AssertInvariants(filled, copy.size());
    EXPECT_EQ(filled.capacity(), copy.size());
  }

  TEST_F(VectorTest, SwapDifferent)
  {
    filled.swap(empty);
    AssertInvariants(filled, 0);
    AssertInvariants(empty, copy.size());
    EXPECT_TRUE(std::equal(empty.begin(), empty.end(), copy.begin()));
  }

  TEST_F(VectorTest, SwapSame)
  {
    filled.swap(filled);
    AssertInvariants(filled, copy.size());
    EXPECT_TRUE(std::equal(filled.begin(), filled.end(), copy.begin()));
  }

  TEST_F(VectorTest, ClearFilled)
  {
    filled.clear();
    AssertInvariants(filled, 0);
  }

  TEST_F(VectorTest, ClearEmpty)
  {
    empty.clear();
    AssertInvariants(empty, 0);
  }

  TEST_F(VectorTest, InsertValueWithoutReallocation)
  {
    filled.reserve(filled.capacity() + 1);
    auto val = VectorT::value_type(18);
    auto pos = filled.begin() + filled.size() / 2;
    auto it = filled.insert(pos, val);
    EXPECT_EQ(*it, val);
    AssertInvariants(filled, copy.size() + 1);
  }

  TEST_F(VectorTest, InsertValueWithReallocation)
  {
    filled.shrink_to_fit();
    auto val = VectorT::value_type(18);
    auto pos = filled.begin() + filled.size() / 2;
    auto it = filled.insert(pos, val);
    EXPECT_EQ(*it, val);
    AssertInvariants(filled, copy.size() + 1);
  }

  TEST_F(VectorTest, InsertSeveralValues)
  {
    /* TODO: Implement this test: InsertSeveralValues */
  }

  TEST_F(VectorTest, InsertIterator)
  {
    /* TODO: Implement this test: InsertIterator */
  }

  TEST_F(VectorTest, InsertInitializerList)
  {
    /* TODO: Implement this test: InsertInitializerList */
  }

  TEST_F(VectorTest, AssignValues)
  {
    auto val = VectorT::value_type(18);
    auto size = filled.size() * 2;
    filled.assign(size, val);
    AssertInvariants(filled, size);
    EXPECT_TRUE(std::all_of(filled.begin(), filled.end(),
        [&val](const auto& x) { return x == val; }));
  }

  TEST_F(VectorTest, AssignIterator)
  {
    /* TODO: Implement this test: AssignIterator */
  }

  TEST_F(VectorTest, AssignInitializerList)
  {
    /* TODO: Implement this test: AssignInitializerList */
  }

  TEST_F(VectorTest, Emplace)
  {
    /* TODO: Implement this test: Emplace */
  }

  TEST_F(VectorTest, EmplaceBack)
  {
    auto val = VectorT::value_type(1212);
    filled.emplace_back(val);
    AssertInvariants(filled, copy.size() + 1);
    EXPECT_EQ(filled.back(), val);
  }

  TEST_F(VectorTest, EraseOne)
  {
    auto pos = filled.begin() + filled.size() / 2;
    auto it = filled.erase(pos);
    AssertInvariants(filled, copy.size() - 1);
    EXPECT_EQ(it, filled.begin() + copy.size() / 2);
  }

  TEST_F(VectorTest, EraseRange)
  {
    /* TODO: Implement this test: EraseRange */
  }

  // TODO: Add Tests for: at, operator[]
  // TODO: Add Tests for: free swap function, free comparison operators
  // TODO: Think: should we add tests for: size(), capacity(), max_size(),
  // empty(), [c|r]begin(), [c|r]end(), front(), back(), data() ?
}
