#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <ftl/vector.hpp>
#include <gtest/gtest.h>

using VectorT = ftl::vector<int>;
using namespace std::placeholders;

void test_invariants(const VectorT& vector)
{
  EXPECT_EQ(vector.empty(), vector.size() == 0);
  EXPECT_EQ(vector.empty(), vector.begin() == vector.end());
  EXPECT_EQ(vector.data() == nullptr, vector.capacity() == 0);
  EXPECT_LE(vector.size(), vector.capacity());
  EXPECT_LE(vector.begin(), vector.end());
  EXPECT_EQ(vector.size(), std::distance(vector.begin(), vector.end()));
}

TEST(VectorConstructor, Default)
{
  const VectorT vector;
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorConstructor, CopyEmpty)
{
  const VectorT src;
  const VectorT vector(src);
  EXPECT_EQ(vector, src);
  test_invariants(vector);
}

TEST(VectorConstructor, Copy)
{
  const VectorT src{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT vector(src);
  EXPECT_EQ(vector, src);
  test_invariants(vector);
}

TEST(VectorConstructor, MoveEmpty)
{
  VectorT src;
  const VectorT vector(std::move(src));
  EXPECT_EQ(vector, VectorT{});
  test_invariants(vector);
}

TEST(VectorConstructor, Move)
{
  VectorT src{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT expected(src);
  const VectorT vector(std::move(src));
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorConstructor, CopyWithAllocator)
{
  const std::allocator<VectorT::value_type> alloc;
  const VectorT src{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT vector(src, alloc);
  EXPECT_EQ(vector, src);
  test_invariants(vector);
}

TEST(VectorConstructor, MoveWithAllocator)
{
  const std::allocator<VectorT::value_type> alloc;
  VectorT src{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT expected(src);
  const VectorT vector(std::move(src), alloc);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorConstructor, Allocator)
{
  const std::allocator<VectorT::value_type> alloc;
  const VectorT vector(alloc);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorConstructor, ZeroSize)
{
  const VectorT vector(0);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorConstructor, Size)
{
  constexpr static VectorT::size_type size = 100;
  const static auto eq_to_zero = std::bind(std::equal_to<>{}, _1, 0);
  const VectorT vector(size);
  EXPECT_EQ(vector.size(), size);
  EXPECT_TRUE(std::all_of(vector.begin(), vector.end(), eq_to_zero));
  test_invariants(vector);
}

TEST(VectorConstructor, ZeroSizeAndValue)
{
  const VectorT vector(0, 7);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorConstructor, SizeAndValue)
{
  constexpr static VectorT::size_type size = 100;
  constexpr static VectorT::value_type value = 7;
  const static auto eq_to_value = std::bind(std::equal_to<>{}, _1, value);
  const VectorT vector(size, value);
  EXPECT_EQ(vector.size(), size);
  EXPECT_TRUE(std::all_of(vector.begin(), vector.end(), eq_to_value));
  test_invariants(vector);
}

TEST(VectorConstructor, EmptyIterators)
{
  const std::initializer_list<VectorT::value_type> values;
  const VectorT vector(values.begin(), values.end());
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorConstructor, Iterators)
{
  const auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT vector(values.begin(), values.end());
  EXPECT_EQ(vector.size(), values.size());
  EXPECT_TRUE(std::equal(vector.begin(), vector.end(), values.begin()));
  test_invariants(vector);
}

TEST(VectorConstructor, EmptyInitializerList)
{
  const std::initializer_list<VectorT::value_type> values;
  const VectorT vector(values);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorConstructor, InitializerList)
{
  const auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT vector(values);
  EXPECT_EQ(vector.size(), values.size());
  EXPECT_TRUE(std::equal(vector.begin(), vector.end(), values.begin()));
  test_invariants(vector);
}

TEST(VectorAssignmentOperator, CopyEmpty)
{
  const VectorT src;
  VectorT vector;
  vector = src;
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorAssignmentOperator, Copy)
{
  const VectorT src{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  VectorT vector;
  vector = src;
  EXPECT_EQ(vector, src);
  test_invariants(vector);
}

TEST(VectorAssignmentOperator, MoveEmpty)
{
  VectorT vector;
  vector = VectorT{};
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorAssignmentOperator, Move)
{
  VectorT src{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT expected(src);
  VectorT vector;
  vector = std::move(src);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorAssignmentOperator, EmptyInitializerList)
{
  const std::initializer_list<int> values;
  VectorT vector;
  vector = values;
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorAssignmentOperator, InitializerList)
{
  const auto values = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  const VectorT expected(values);
  VectorT vector;
  vector = values;
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

// TODO: Refactor below

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
