#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ftl/vector.hpp>
#include <gtest/gtest.h>

using VectorT = ftl::vector<int>;

void test_invariants(const VectorT& vector)
{
  EXPECT_EQ(vector.empty(), vector.size() == 0);
  EXPECT_EQ(vector.empty(), vector.begin() == vector.end());
  EXPECT_EQ(vector.data() == nullptr, vector.capacity() == 0);
  EXPECT_LE(vector.size(), vector.capacity());
  EXPECT_LE(vector.begin(), vector.end());
  EXPECT_EQ(vector.size(), std::distance(vector.begin(), vector.end()));
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
  test_invariants(empty);
}

TEST(VectorConstructor, Allocator)
{
  std::allocator<VectorT::value_type> alloc;
  VectorT vector(alloc);
  test_invariants(vector);
}

TEST(VectorConstructor, Size)
{
  size_t size = 100;
  VectorT::value_type val{};
  VectorT vector(size);
  test_invariants(vector);
  EXPECT_TRUE(std::all_of(vector.cbegin(), vector.cend(),
      [&val](const auto& x) { return x == val; }));
}

TEST(VectorConstructor, SizeAndValue)
{
  size_t size = 100;
  VectorT::value_type val(7);
  VectorT vector(size, val);
  test_invariants(vector);
  EXPECT_TRUE(std::all_of(vector.cbegin(), vector.cend(),
      [&val](const auto& x) { return x == val; }));
}

TEST(VectorConstructor, Iterators)
{
  auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  VectorT vector(values.begin(), values.end());
  test_invariants(vector);
  EXPECT_TRUE(std::equal(vector.begin(), vector.end(), values.begin()));
}

TEST(VectorConstructor, InitializerList)
{
  std::initializer_list<VectorT::value_type> values = { 1, 2, 3, 4, 5, 6, 7 };
  VectorT vector(values);
  test_invariants(vector);
  EXPECT_TRUE(std::equal(vector.begin(), vector.end(), values.begin()));
}

TEST(VectorConstructor, Copy)
{
  VectorT vector(100, 7);
  VectorT copy(vector);
  test_invariants(copy);
  EXPECT_EQ(vector, copy);
}

TEST(VectorConstructor, Move)
{
  VectorT vector(100, 7);
  VectorT copy(vector);
  VectorT moved(std::move(vector));
  test_invariants(moved);
  EXPECT_EQ(copy, moved);
}

TEST(VectorAssignmentOperator, Move)
{
  VectorT vector(100, 7);
  VectorT copy(vector);
  VectorT moved;
  moved = std::move(vector);
  test_invariants(moved);
  EXPECT_EQ(copy, moved);
}

TEST(VectorAssignmentOperator, Copy)
{
  VectorT vector(100, 7);
  VectorT copy;
  copy = vector;
  test_invariants(copy);
  EXPECT_EQ(copy, copy);
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
  test_invariants(filled);
  EXPECT_EQ(filled.back(), val);
  EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
}

TEST_F(VectorTest, PushBackEmpty)
{
  auto val = VectorT::value_type(77);
  empty.push_back(val);
  test_invariants(empty);
  EXPECT_EQ(empty.back(), val);
  EXPECT_EQ(empty.front(), val);
}

TEST_F(VectorTest, PopBack)
{
  filled.pop_back();
  test_invariants(filled);
}

TEST_F(VectorTest, Reserve)
{
  auto newCapacity = filled.capacity() * 2;
  filled.reserve(newCapacity);
  test_invariants(filled);
  EXPECT_GE(filled.capacity(), newCapacity);
  EXPECT_EQ(filled, copy);
}

TEST_F(VectorTest, ResizeToZero)
{
  filled.resize(0);
  test_invariants(filled);
}

TEST_F(VectorTest, ResizeWithoutReallocation)
{
  filled.reserve(filled.capacity() * 2);
  auto newSize = filled.capacity() - 1;
  filled.resize(newSize);
  test_invariants(filled);
  EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
}

TEST_F(VectorTest, ResizeWithReallocation)
{
  auto newSize = filled.capacity() * 2;
  filled.resize(newSize);
  test_invariants(filled);
  EXPECT_TRUE(std::equal(copy.begin(), copy.end(), filled.begin()));
}

TEST_F(VectorTest, ShrinkToFit)
{
  auto capacity = filled.capacity();
  auto newCapacity = capacity * 2;
  filled.reserve(newCapacity);
  filled.shrink_to_fit();
  test_invariants(filled);
  EXPECT_EQ(filled, copy);
  EXPECT_EQ(filled.capacity(), copy.size());
}

TEST_F(VectorTest, SwapDifferent)
{
  filled.swap(empty);
  test_invariants(filled);
  test_invariants(empty);
  EXPECT_EQ(empty, copy);
}

TEST_F(VectorTest, SwapSame)
{
  filled.swap(filled);
  test_invariants(filled);
  EXPECT_EQ(filled, copy);
}

TEST_F(VectorTest, ClearFilled)
{
  filled.clear();
  test_invariants(filled);
}

TEST_F(VectorTest, ClearEmpty)
{
  empty.clear();
  test_invariants(empty);
}

TEST_F(VectorTest, InsertValueWithoutReallocation)
{
  filled.reserve(filled.capacity() + 1);
  auto val = VectorT::value_type(18);
  auto pos = filled.begin() + filled.size() / 2;
  auto it = filled.insert(pos, val);
  EXPECT_EQ(*it, val);
  test_invariants(filled);
}

TEST_F(VectorTest, InsertValueWithReallocation)
{
  filled.shrink_to_fit();
  auto val = VectorT::value_type(18);
  auto pos = filled.begin() + filled.size() / 2;
  auto it = filled.insert(pos, std::move(val));
  EXPECT_EQ(*it, val);
  test_invariants(filled);
}

TEST_F(VectorTest, InsertSeveralValues)
{
  size_t new_vals_count = 3;
  auto val = VectorT::value_type(18);
  auto it = filled.insert(filled.cbegin(), new_vals_count, val);
  ASSERT_EQ(filled.cbegin(), it);
  ASSERT_TRUE(std::all_of(filled.cbegin(), filled.cbegin() + new_vals_count,
      [&](auto e) { return e == val; }));
  test_invariants(filled);
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
  test_invariants(filled);
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
  test_invariants(filled);
}

TEST_F(VectorTest, AssignValues)
{
  auto val = VectorT::value_type(18);
  auto size = filled.size() * 2;
  filled.assign(size, val);
  test_invariants(filled);
  EXPECT_TRUE(std::all_of(filled.cbegin(), filled.cend(),
      [&val](const auto& x) { return x == val; }));
}

TEST_F(VectorTest, AssignIterator)
{
  auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  filled.assign(values.begin(), values.end());
  test_invariants(filled);
  EXPECT_TRUE(std::equal(filled.begin(), filled.end(), values.begin()));
}

TEST_F(VectorTest, AssignInitializerList)
{
  std::initializer_list<VectorT::value_type> values = { 1, 2, 3, 4, 5, 6, 7 };
  filled.assign(values);
  test_invariants(filled);
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
  test_invariants(filled);
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
  test_invariants(filled);
  auto filled_end = filled.begin() + shift;
  auto copy_end = copy.begin() + shift;
  EXPECT_TRUE(std::equal(filled.begin(), filled_end, copy.begin(), copy_end));
  EXPECT_TRUE(std::equal(filled_end + 1, filled.end(), copy_end, copy.end()));
}

TEST_F(VectorTest, EmplaceBack)
{
  auto val = VectorT::value_type(1212);
  filled.emplace_back(val);
  test_invariants(filled);
  EXPECT_EQ(filled.back(), val);
}

TEST_F(VectorTest, EraseOne)
{
  auto pos = filled.begin() + filled.size() / 2;
  auto it = filled.erase(pos);
  test_invariants(filled);
  EXPECT_EQ(it, filled.begin() + copy.size() / 2);
}

TEST_F(VectorTest, EraseRange)
{
  auto first = filled.begin() + filled.size() / 2;
  auto last = first + 4;
  auto count = last - first;
  auto it = filled.erase(first, last);
  test_invariants(filled);
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
