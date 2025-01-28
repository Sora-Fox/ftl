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
    if (size > 0) {
      EXPECT_EQ(vector.front(), *vector.cbegin());
      EXPECT_EQ(vector.back(), *(vector.cend() - 1));
    }
  }

  void AssertAllElementsEqual(const VectorT& vec,
      const VectorT::value_type& expected)
  {
    EXPECT_TRUE(std::all_of(vec.cbegin(), vec.cend(),
        [&expected](const auto& x) { return x == expected; }));
  }

  void AssertVectorsEqual(const VectorT& vec1, const VectorT& vec2)
  {
    EXPECT_EQ(vec1.size(), vec2.size());
    EXPECT_TRUE(std::equal(vec1.cbegin(), vec1.cend(), vec2.cbegin()));
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
    AssertAllElementsEqual(vector, val);
  }

  TEST(VectorConstructor, SizeAndValue)
  {
    size_t size = 100;
    VectorT::value_type val(7);
    VectorT vector(size, val);
    AssertInvariants(vector, size);
    AssertAllElementsEqual(vector, val);
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
    AssertVectorsEqual(vector, copy);
  }

  TEST(VectorConstructor, Move)
  {
    VectorT vector(100, 7);
    VectorT copy(vector);
    VectorT moved(std::move(vector));
    AssertInvariants(moved, copy.size());
    AssertVectorsEqual(copy, moved);
  }

  TEST(VectorAssignmentOperator, Move)
  {
    VectorT vector(100, 7);
    VectorT copy(vector);
    VectorT moved;
    moved = std::move(vector);
    AssertInvariants(moved, copy.size());
    AssertVectorsEqual(copy, moved);
  }

  TEST(VectorAssignmentOperator, Copy)
  {
    VectorT vector(100, 7);
    VectorT copy;
    copy = vector;
    AssertInvariants(copy, vector.size());
    AssertVectorsEqual(copy, copy);
  }

  TEST_F(VectorTest, ReverseIterators)
  {
    auto rbeg = filled.rbegin();
    auto rend = filled.rend();
    EXPECT_EQ(*rbeg, filled.back());
    EXPECT_EQ(*(rend - 1), filled.front());
  }

  TEST_F(VectorTest, IteratorInvalidationOnReallocation)
  {
    auto it = filled.begin();
    filled.reserve(filled.capacity() * 2);
    EXPECT_NE(it, filled.begin());
  }

  TEST_F(VectorTest, DataPointer)
  {
    EXPECT_EQ(filled.data(), std::addressof(filled.front()));
    EXPECT_EQ(empty.data(), nullptr);
  }

  TEST_F(VectorTest, OperatorBracketAccess)
  {
    auto value = 111;
    auto index = filled.size() - 1;
    filled[filled.size() - 1] = value;
    EXPECT_EQ(filled[index], value);
    const VectorT& const_filled = filled;
    EXPECT_EQ(const_filled[index], value);
  }

  TEST_F(VectorTest, AtValidIndex)
  {
    auto value = 111;
    auto index = filled.size() - 1;
    filled.at(index) = value;
    EXPECT_EQ(filled.at(index), value);
    const VectorT& const_filled = filled;
    EXPECT_EQ(const_filled.at(index), value);
  }

  TEST_F(VectorTest, AtOutOfRange)
  {
    EXPECT_THROW(filled.at(filled.size()), std::out_of_range);
    EXPECT_THROW(empty.at(0), std::out_of_range);
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
    AssertVectorsEqual(filled, copy);
  }

  TEST_F(VectorTest, ResizeToZero)
  {
    filled.resize(0);
    AssertInvariants(filled, 0);
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
    AssertVectorsEqual(filled, copy);
    EXPECT_EQ(filled.capacity(), copy.size());
  }

  TEST_F(VectorTest, SwapDifferent)
  {
    filled.swap(empty);
    AssertInvariants(filled, 0);
    AssertInvariants(empty, copy.size());
    AssertVectorsEqual(empty, copy);
  }

  TEST_F(VectorTest, SwapSame)
  {
    filled.swap(filled);
    AssertInvariants(filled, copy.size());
    AssertVectorsEqual(filled, copy);
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
    auto it = filled.insert(pos, std::move(val));
    EXPECT_EQ(*it, val);
    AssertInvariants(filled, copy.size() + 1);
  }

  TEST_F(VectorTest, InsertSeveralValues)
  {
    size_t new_vals_count = 3;
    auto val = VectorT::value_type(18);
    auto it = filled.insert(filled.cbegin(), new_vals_count, val);
    ASSERT_EQ(filled.cbegin(), it);
    ASSERT_TRUE(std::all_of(filled.cbegin(), filled.cbegin() + new_vals_count,
        [&](auto e) { return e == val; }));
    AssertInvariants(filled, copy.size() + new_vals_count);
  }

  TEST_F(VectorTest, InsertIterator)
  {
    auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    size_t shift = 2;
    auto pos = filled.cbegin() + shift;
    auto it = filled.insert(pos, values.begin(), values.end());
    ASSERT_EQ(std::distance(filled.begin(), it), shift);
    pos = filled.cbegin() + shift;
    ASSERT_TRUE(std::equal(pos, pos + values.size(), values.begin()));
    AssertInvariants(filled, copy.size() + values.size());
  }

  TEST_F(VectorTest, InsertInitializerList)
  {
    std::initializer_list<VectorT::value_type> values = { 1, 2, 3, 4, 5, 6 };
    size_t shift = 2;
    auto pos = filled.cbegin() + shift;
    auto it = filled.insert(pos, values);
    ASSERT_EQ(std::distance(filled.begin(), it), shift);
    pos = filled.cbegin() + shift;
    ASSERT_TRUE(std::equal(pos, pos + values.size(), values.begin()));
    AssertInvariants(filled, copy.size() + values.size());
  }

  TEST_F(VectorTest, AssignValues)
  {
    auto val = VectorT::value_type(18);
    auto size = filled.size() * 2;
    filled.assign(size, val);
    AssertInvariants(filled, size);
    AssertAllElementsEqual(filled, val);
  }

  TEST_F(VectorTest, AssignIterator)
  {
    auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    filled.assign(values.begin(), values.end());
    AssertInvariants(filled, values.size());
    EXPECT_TRUE(std::equal(filled.begin(), filled.end(), values.begin()));
  }

  TEST_F(VectorTest, AssignInitializerList)
  {
    std::initializer_list<VectorT::value_type> values = { 1, 2, 3, 4, 5, 6, 7 };
    filled.assign(values);
    AssertInvariants(filled, values.size());
    EXPECT_TRUE(std::equal(filled.begin(), filled.end(), values.begin()));
  }

  TEST_F(VectorTest, EmplaceWithoutReallocation)
  {
    filled.reserve(filled.capacity() + 1);
    auto val = VectorT::value_type(18);
    size_t shift = filled.size() / 2;
    auto pos = filled.begin() + shift;
    auto it = filled.emplace(pos, val);
    EXPECT_EQ(*it, val);
    AssertInvariants(filled, copy.size() + 1);
    auto filled_end = filled.begin() + shift;
    auto copy_end = copy.begin() + shift;
    EXPECT_TRUE(std::equal(filled.begin(), filled_end, copy.begin(), copy_end));
    EXPECT_TRUE(std::equal(filled_end + 1, filled.end(), copy_end, copy.end()));
  }

  TEST_F(VectorTest, EmplaceWithReallocation)
  {
    filled.shrink_to_fit();
    auto val = VectorT::value_type(18);
    size_t shift = filled.size() / 2;
    auto pos = filled.begin() + shift;
    auto it = filled.emplace(pos, val);
    EXPECT_EQ(*it, val);
    AssertInvariants(filled, copy.size() + 1);
    auto filled_end = filled.begin() + shift;
    auto copy_end = copy.begin() + shift;
    EXPECT_TRUE(std::equal(filled.begin(), filled_end, copy.begin(), copy_end));
    EXPECT_TRUE(std::equal(filled_end + 1, filled.end(), copy_end, copy.end()));
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
    auto first = filled.begin() + filled.size() / 2;
    auto last = first + 4;
    auto count = last - first;
    auto it = filled.erase(first, last);
    AssertInvariants(filled, copy.size() - count);
    EXPECT_EQ(it, filled.begin() + copy.size() / 2);
    EXPECT_TRUE(std::equal(filled.begin(), it, copy.begin()));
    EXPECT_TRUE(std::equal(it, filled.end(),
        copy.begin() + (it - filled.begin()) + count));
  }

  TEST(VectorComparison, Equality)
  {
    VectorT vec1{ 1, 2, 3 };
    VectorT vec2{ 1, 2, 3 };
    VectorT vec3{ 1, 2, 3, 4 };
    EXPECT_TRUE(vec1 == vec2);
    EXPECT_TRUE(vec1 == vec1);
    EXPECT_FALSE(vec1 == vec3);
  }

  TEST(VectorComparison, LexicographicalOrder)
  {
    VectorT vec1{ 1, 2, 3 };
    VectorT vec2{ 1, 2, 4 };
    EXPECT_TRUE(vec1 < vec2);
    EXPECT_FALSE(vec2 < vec1);
  }
}
