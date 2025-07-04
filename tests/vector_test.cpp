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

TEST(VectorResize, ZeroSize)
{
  VectorT vector;
  vector.resize(0);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorResize, ReduceSize)
{
  const VectorT expected{ 1, 2, 3 };
  VectorT vector{ 1, 2, 3, 4, 5 };
  vector.resize(3);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorResize, IncreaseSizeWithDefaultValue)
{
  const VectorT expected{ 1, 2, 3, 0, 0 };
  VectorT vector{ 1, 2, 3 };
  vector.resize(5);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorResize, IncreaseSizeWithCustomValue)
{
  const VectorT expected{ 1, 2, 3, 7, 7 };
  VectorT vector{ 1, 2, 3 };
  vector.resize(5, 7);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorResize, SameSize)
{
  VectorT vector{ 1, 2, 3 };
  const VectorT expected(vector);
  vector.resize(3);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorReserve, IncreaseCapacityEmpty)
{
  VectorT vector;
  vector.reserve(100);
  EXPECT_GE(vector.capacity(), 100);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorReserve, IncreaseCapacity)
{
  const VectorT expected{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  VectorT vector(expected);
  vector.reserve(100);
  EXPECT_GE(vector.capacity(), 100);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorReserve, DecreaseCapacity)
{
  VectorT vector;
  vector.reserve(100);
  const auto capacity = vector.capacity();
  vector.reserve(5);
  EXPECT_EQ(vector.capacity(), capacity);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorReserve, ZeroCapacity)
{
  VectorT vector;
  vector.reserve(0);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorShrinkToFit, Empty)
{
  VectorT vector;
  vector.shrink_to_fit();
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorShrinkToFit, EmptyReserved)
{
  VectorT vector;
  vector.reserve(100);
  vector.shrink_to_fit();
  EXPECT_TRUE(vector.empty());
  EXPECT_EQ(vector.capacity(), 0);
  test_invariants(vector);
}

TEST(VectorShrinkToFit, AlreadyFitted)
{
  const VectorT expected{ 1, 2, 3 };
  VectorT vector(expected);
  VectorT::const_pointer const old_data = vector.data();
  vector.shrink_to_fit();
  EXPECT_EQ(vector.data(), old_data);
  EXPECT_EQ(vector.capacity(), vector.size());
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorShrinkToFit, Unfitted)
{
  const VectorT expected{ 1, 2, 3, 4, 5 };
  VectorT vector(expected.begin(), expected.end());
  vector.shrink_to_fit();
  EXPECT_EQ(vector.capacity(), vector.size());
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorElementAccess, ConstSquareBrackets)
{
  const VectorT vector{ 10, 20, 30 };
  EXPECT_EQ(vector[0], 10);
  EXPECT_EQ(vector[1], 20);
  EXPECT_EQ(vector[2], 30);
  test_invariants(vector);
}

TEST(VectorElementAccess, NonConstSquareBrackets)
{
  const VectorT expected{ 7, 20, 30 };
  VectorT vector{ 10, 20, 30 };
  vector[0] = 7;
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorElementAccess, ConstAtValid)
{
  const VectorT vector{ 10, 20, 30 };
  EXPECT_EQ(vector.at(0), 10);
  EXPECT_EQ(vector.at(1), 20);
  EXPECT_EQ(vector.at(2), 30);
  test_invariants(vector);
}

TEST(VectorElementAccess, ConstAtOutOfRange)
{
  const VectorT vector{ 10, 20, 30 };
  EXPECT_THROW(std::ignore = vector.at(3), std::out_of_range);
  test_invariants(vector);
}

TEST(VectorElementAccess, ConstAtEmpty)
{
  const VectorT vector;
  EXPECT_THROW(std::ignore = vector.at(0), std::out_of_range);
  test_invariants(vector);
}

TEST(VectorElementAccess, NonConstAtValid)
{
  const VectorT expected{ 7, 20, 30 };
  VectorT vector{ 10, 20, 30 };
  vector.at(0) = 7;
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorElementAccess, NonConstAtOutOfRange)
{
  VectorT vector{ 10, 20, 30 };
  EXPECT_THROW(std::ignore = vector.at(3), std::out_of_range);
  test_invariants(vector);
}

TEST(VectorElementAccess, NonConstFront)
{
  const VectorT expected{ 10, 20, 30 };
  VectorT vector{ 42, 20, 30 };
  vector.front() = 10;
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorElementAccess, ConstFront)
{
  const VectorT vector{ 42, 20, 30 };
  EXPECT_EQ(vector.front(), 42);
  EXPECT_EQ(&vector.front(), &*vector.begin());
  test_invariants(vector);
}

TEST(VectorElementAccess, NonConstBack)
{
  const VectorT expected{ 10, 20, 30 };
  VectorT vector{ 10, 20, 1 };
  vector.back() = 30;
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorElementAccess, ConstBack)
{
  const VectorT vector{ 10, 20, 1 };
  EXPECT_EQ(vector.back(), 1);
  EXPECT_EQ(&vector.back(), &*(vector.end() - 1));
  test_invariants(vector);
}

TEST(VectorElementAccess, EmptyData)
{
  const VectorT vector;
  EXPECT_EQ(vector.data(), nullptr);
  test_invariants(vector);
}

TEST(VectorElementAccess, EmptyReservedData)
{
  VectorT vector;
  vector.reserve(10);
  EXPECT_NE(vector.data(), nullptr);
  test_invariants(vector);
}

TEST(VectorElementAccess, NonConstData)
{
  const VectorT expected{ 10, 20, 30 };
  VectorT vector{ 42, 20, 30 };
  vector.data()[0] = 10;
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(vector.data(), &*vector.begin());
  test_invariants(vector);
}

TEST(VectorAssign, ZeroSizeAndValue)
{
  VectorT vector{ 1, 2, 3 };
  vector.assign(0, 42);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorAssign, SizeAndValue)
{
  const VectorT expected(5, 10);
  VectorT vector;
  vector.assign(5, 10);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorAssign, IteratorsEmptyRange)
{
  const std::initializer_list<VectorT::value_type> values;
  VectorT vector{ 1, 2, 3 };
  vector.assign(values.begin(), values.end());
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorAssign, IteratorsNonEmptyRange)
{
  const VectorT expected{ 4, 5, 6, 7, 8, 9 };
  VectorT vector{ 1, 2, 3 };
  vector.assign(expected.begin(), expected.end());
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorAssign, InitializerListEmpty)
{
  const std::initializer_list<VectorT::value_type> values;
  VectorT vector{ 1, 2, 3 };
  vector.assign(values);
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorAssign, InitializerListNonEmpty)
{
  const auto values = { 4, 5, 6, 7, 8, 9 };
  const VectorT expected(values);
  VectorT vector{ 1, 2, 3 };
  vector.assign(values);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorPushBack, CopyEmpty)
{
  const VectorT expected{ 10 };
  VectorT vector;
  vector.push_back(expected.front());
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorPushBack, CopyNonEmpty)
{
  const VectorT expected{ 10, 20, 30 };
  VectorT vector{ 10, 20 };
  vector.push_back(expected.back());
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorPushBack, MoveEmpty)
{
  const VectorT expected{ 10 };
  VectorT vector;
  vector.push_back(10);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorPushBack, MoveNonEmpty)
{
  const VectorT expected{ 10, 20, 30 };
  VectorT vector{ 10, 20 };
  vector.push_back(30);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorPopBack, SingleElement)
{
  VectorT vector{ 7 };
  vector.pop_back();
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorPopBack, MultipleElements)
{
  const VectorT expected{ 10, 20, 30 };
  VectorT vector{ 10, 20, 30, 7 };
  vector.pop_back();
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorInsert, SingleToEmpty)
{
  const VectorT expected{ 1 };
  VectorT vector;
  const auto it = vector.insert(vector.end(), 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorInsert, SingleCopyAtBegin)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector{ 2, 3, 4 };
  const auto it = vector.insert(vector.begin(), 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorInsert, SingleCopyAtMiddle)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector{ 1, 3, 4 };
  const auto it = vector.insert(vector.begin() + 1, 2);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorInsert, SingleCopyAtEnd)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector{ 1, 2, 3 };
  auto it = vector.insert(vector.end(), 4);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.end() - 1);
  test_invariants(vector);
}

TEST(VectorInsert, SingleMove)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector{ 1, 3, 4 };
  const auto it = vector.insert(vector.begin() + 1, 2);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorInsert, MultipleCopiesToEmpty)
{
  const VectorT expected{ 1, 1 };
  VectorT vector;
  const auto it = vector.insert(vector.begin(), 2, 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorInsert, MultipleCopiesAtBegin)
{
  const VectorT expected{ 2, 2, 3, 4 };
  VectorT vector{ 3, 4 };
  const auto it = vector.insert(vector.begin(), 2, 2);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorInsert, MultipleCopiesAtMiddle)
{
  const VectorT expected{ 1, 2, 2, 3 };
  VectorT vector{ 1, 3 };
  const auto it = vector.insert(vector.begin() + 1, 2, 2);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorInsert, MultipleCopiesAtEnd)
{
  const VectorT expected{ 1, 2, 3, 3 };
  VectorT vector{ 1, 2 };
  const auto it = vector.insert(vector.end(), 2, 3);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.end() - 2);
  test_invariants(vector);
}

TEST(VectorInsert, IteratorRangeToEmpty)
{
  const VectorT expected{ 1, 2 };
  const std::initializer_list<VectorT::value_type> values = { 1, 2 };
  VectorT vector;
  const auto it = vector.insert(vector.end(), values.begin(), values.end());
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorInsert, IteratorRangeAtBegin)
{
  const VectorT expected{ 1, 2, 3, 4 };
  const std::initializer_list<VectorT::value_type> values = { 1, 2 };
  VectorT vector{ 3, 4 };
  const auto it = vector.insert(vector.begin(), values.begin(), values.end());
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorInsert, IteratorRangeAtMiddle)
{
  const VectorT expected{ 1, 2, 3, 4 };
  const std::initializer_list<VectorT::value_type> vals = { 2, 3 };
  VectorT vector{ 1, 4 };
  const auto it = vector.insert(vector.begin() + 1, vals.begin(), vals.end());
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorInsert, IteratorRangeAtEnd)
{
  const VectorT expected{ 1, 2, 3, 4 };
  const std::initializer_list<VectorT::value_type> vals = { 3, 4 };
  VectorT vector{ 1, 2 };
  auto it = vector.insert(vector.end(), vals.begin(), vals.end());
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.end() - 2);
  test_invariants(vector);
}

TEST(VectorInsert, InitializerList)
{
  const VectorT expected{ 1, 2, 3, 4 };
  const std::initializer_list<VectorT::value_type> vals = { 2, 3 };
  VectorT vector{ 1, 4 };
  const auto it = vector.insert(vector.begin() + 1, vals);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorErase, SingleAtBegin)
{
  const VectorT expected{ 2, 3 };
  VectorT vector{ 1, 2, 3 };
  const auto it = vector.erase(vector.begin());
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorErase, SingleAtMiddle)
{
  const VectorT expected{ 1, 3 };
  VectorT vector{ 1, 2, 3 };
  const auto it = vector.erase(vector.begin() + 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorErase, SingleAtEnd)
{
  const VectorT expected{ 1, 2 };
  VectorT vector{ 1, 2, 3 };
  const auto it = vector.erase(vector.end() - 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.end());
  test_invariants(vector);
}

TEST(VectorErase, EmptyRangeFromEmpty)
{
  VectorT vector;
  const auto it = vector.erase(vector.begin(), vector.begin());
  EXPECT_TRUE(vector.empty());
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorErase, EmptyRange)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector(expected);
  const auto it = vector.erase(vector.begin() + 1, vector.begin() + 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorErase, RangeFromBegin)
{
  const VectorT expected{ 3, 4 };
  VectorT vector{ 1, 2, 3, 4 };
  const auto it = vector.erase(vector.begin(), vector.end() - 2);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorErase, RangeInMiddle)
{
  const VectorT expected{ 1, 4 };
  VectorT vector{ 1, 2, 3, 4 };
  const auto it = vector.erase(vector.begin() + 1, vector.end() - 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorErase, RangeToEnd)
{
  const VectorT expected{ 1, 2 };
  VectorT vector{ 1, 2, 3, 4 };
  const auto it = vector.erase(vector.begin() + 2, vector.end());
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.end());
  test_invariants(vector);
}

TEST(VectorErase, FullRange)
{
  VectorT vector{ 1, 2, 3, 4 };
  const auto it = vector.erase(vector.begin(), vector.end());
  EXPECT_TRUE(vector.empty());
  EXPECT_EQ(it, vector.end());
  test_invariants(vector);
}

TEST(VectorSwap, NonEmptyWithNonEmpty)
{
  VectorT lhs{ 1, 2, 3 };
  VectorT rhs{ 4, 5 };
  const VectorT rhs_copy = rhs;
  const VectorT lhs_copy = lhs;
  lhs.swap(rhs);
  EXPECT_EQ(lhs, rhs_copy);
  EXPECT_EQ(rhs, lhs_copy);
  test_invariants(lhs);
  test_invariants(rhs);
}

TEST(VectorSwap, EmptyWithNonEmpty)
{
  VectorT lhs;
  VectorT rhs{ 4, 5 };
  const VectorT rhs_copy = rhs;
  lhs.swap(rhs);
  EXPECT_EQ(lhs, rhs_copy);
  EXPECT_TRUE(rhs.empty());
  test_invariants(lhs);
  test_invariants(rhs);
}

TEST(VectorSwap, EmptyWithEmpty)
{
  VectorT lhs;
  VectorT rhs;
  lhs.swap(rhs);
  EXPECT_TRUE(lhs.empty() && rhs.empty());
  test_invariants(lhs);
  test_invariants(rhs);
}

TEST(VectorClear, NonEmpty)
{
  VectorT vector{ 1, 2, 3, 4 };
  vector.clear();
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorClear, Empty)
{
  VectorT vector;
  vector.clear();
  EXPECT_TRUE(vector.empty());
  test_invariants(vector);
}

TEST(VectorEmplace, Empty)
{
  const VectorT expected{ 1 };
  VectorT vector;
  const auto it = vector.emplace(vector.end(), 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorEmplace, AtBegin)
{
  const VectorT expected{ 1, 2, 3 };
  VectorT vector{ 2, 3 };
  const auto it = vector.emplace(vector.begin(), 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorEmplace, AtMiddle)
{
  const VectorT expected{ 1, 2, 3 };
  VectorT vector{ 1, 3 };
  const auto it = vector.emplace(vector.begin() + 1, 2);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 1);
  test_invariants(vector);
}

TEST(VectorEmplace, AtEnd)
{
  const VectorT expected{ 1, 2, 3 };
  VectorT vector{ 1, 2 };
  const auto it = vector.emplace(vector.end(), 3);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.end() - 1);
  test_invariants(vector);
}

TEST(VectorEmplace, EmptyReserved)
{
  const VectorT expected{ 1 };
  VectorT vector;
  vector.reserve(10);
  const auto it = vector.emplace(vector.end(), 1);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin());
  test_invariants(vector);
}

TEST(VectorEmplace, Reserved)
{
  const VectorT expected{ 1, 2, 3, 4, 5 };
  VectorT vector{ 1, 2, 4, 5 };
  vector.reserve(10);
  const auto it = vector.emplace(vector.begin() + 2, 3);
  EXPECT_EQ(vector, expected);
  EXPECT_EQ(it, vector.begin() + 2);
  test_invariants(vector);
}

TEST(VectorEmplaceBack, Empty)
{
  const VectorT expected{ 17 };
  VectorT vector;
  vector.emplace_back(17);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorEmplaceBack, NonReserved)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector{ 1, 2, 3 };
  vector.shrink_to_fit();
  vector.emplace_back(4);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorEmplaceBack, Reserved)
{
  const VectorT expected{ 1, 2, 3, 4 };
  VectorT vector{ 1, 2, 3 };
  vector.reserve(10);
  vector.emplace_back(4);
  EXPECT_EQ(vector, expected);
  test_invariants(vector);
}

TEST(VectorGetAllocator, Default)
{
  VectorT vector;
  std::allocator<int> default_alloc;
  EXPECT_EQ(vector.get_allocator(), default_alloc);
  test_invariants(vector);
}

void test_comparison(const VectorT& lhs, const VectorT& rhs)
{
  EXPECT_TRUE(lhs == lhs && rhs == rhs);
  EXPECT_TRUE(lhs <= lhs && rhs <= rhs);
  EXPECT_FALSE(lhs < lhs || rhs < rhs);
  EXPECT_EQ(lhs == rhs, rhs == lhs);
  EXPECT_EQ(lhs != rhs, rhs != lhs);
  EXPECT_NE(lhs == rhs, lhs != rhs);
  EXPECT_EQ((lhs < rhs), (rhs > lhs));
  EXPECT_EQ((rhs < lhs), (lhs > rhs));
  EXPECT_EQ(lhs <= rhs, !(rhs < lhs));
  EXPECT_EQ(rhs <= lhs, !(lhs < rhs));
}

TEST(VectorComparison, EmptyVsEmpty)
{
  const VectorT lhs;
  const VectorT rhs;
  EXPECT_TRUE(lhs == rhs);
  test_comparison(lhs, rhs);
}

TEST(VectorComparison, EmptyVsNonEmpty)
{
  const VectorT lhs;
  const VectorT rhs{ 1 };
  EXPECT_TRUE(lhs < rhs);
  test_comparison(lhs, rhs);
}

TEST(VectorComparison, SameSizeAndValues)
{
  const VectorT lhs{ 1, 2, 3, 4, 5 };
  const VectorT rhs{ 1, 2, 3, 4, 5 };
  EXPECT_TRUE(lhs == rhs);
  test_comparison(lhs, rhs);
}

TEST(VectorComparison, DifferentValues)
{
  const VectorT lhs{ 1, 2, 3, 4, 5 };
  const VectorT rhs{ 1, 2, 3, 4, 6 };
  EXPECT_TRUE(lhs < rhs);
  test_comparison(lhs, rhs);
}

TEST(VectorComparison, DifferentSizes)
{
  const VectorT lhs{ 1, 2, 3, 4 };
  const VectorT rhs{ 1, 2, 3, 4, 0 };
  EXPECT_TRUE(lhs < rhs);
  test_comparison(lhs, rhs);
}
