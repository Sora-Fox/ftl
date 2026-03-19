#include <iterator>
#include <numeric>
#include <random>
#include <vector>
#include <ftl/map.hpp>
#include <gtest/gtest.h>

using MapT = ftl::map<int, std::string>;

void test_invariants(const MapT& map)
{
  EXPECT_EQ(map.empty(), (map.size() == 0));
  EXPECT_EQ(map.empty(), (map.begin() == map.end()));
  EXPECT_EQ(std::distance(map.begin(), map.end()), map.size());
  EXPECT_TRUE(std::is_sorted(map.begin(), map.end(), map.value_comp()));
}

void test_comparison(const MapT& lhs, const MapT& rhs)
{
  EXPECT_EQ((lhs == rhs), !(lhs != rhs));
  EXPECT_EQ((lhs < rhs), (rhs > lhs));
  EXPECT_EQ((lhs < rhs), !(lhs >= rhs));
  EXPECT_EQ((lhs > rhs), !(lhs <= rhs));
  EXPECT_EQ((lhs == rhs), (lhs <= rhs && lhs >= rhs));
}

TEST(MapConstructor, DefaultConstructor)
{
  const MapT map;
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapConstructor, CopyConstructorFromEmpty)
{
  const MapT map;
  const MapT copied(map);
  test_invariants(copied);
  EXPECT_TRUE(copied.empty());
}

TEST(MapConstructor, CopyConstructor)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT copied(map);
  test_invariants(copied);
  EXPECT_EQ(copied, map);
}

TEST(MapConstructor, move_constructor)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT copied(map);
  const MapT moved(std::move(map));
  test_invariants(moved);
  EXPECT_EQ(moved, copied);
}

TEST(MapConstructor, empty_range_constructor)
{
  const std::initializer_list<MapT::value_type> init{};
  const MapT map(init.begin(), init.end());
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapConstructor, range_constructor)
{
  const std::initializer_list<MapT::value_type> init{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT map(init.begin(), init.end());
  test_invariants(map);
  EXPECT_EQ(map.size(), init.size());
  EXPECT_TRUE(std::equal(map.begin(), map.end(), init.begin()));
}

TEST(MapConstructor, initializer_list_constructor)
{
  const std::initializer_list<MapT::value_type> init{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT map(init);
  test_invariants(map);
  EXPECT_EQ(map.size(), init.size());
  EXPECT_TRUE(std::equal(map.begin(), map.end(), init.begin()));
}

TEST(MapAssignment, CopyAssignmentOperator)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  MapT copied;
  copied = map;
  test_invariants(copied);
  EXPECT_EQ(map, copied);
}

TEST(MapAssignment, MoveAssignmentOperator)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT copied(map);
  MapT moved;
  moved = std::move(map);
  test_invariants(moved);
  EXPECT_EQ(moved, copied);
}

TEST(MapAssignment, InitializerListAssignmentOperator)
{
  const std::initializer_list<MapT::value_type> init{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT initied(init);
  MapT map;
  map = init;
  test_invariants(map);
  EXPECT_EQ(map, initied);
}

TEST(MapElementAccess, SquareBracketsAccess)
{
  MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT copied(map);
  const MapT::mapped_type& val1 = map[2];
  const MapT::mapped_type& val2 = map[2];
  test_invariants(map);
  EXPECT_EQ(map, copied);
  EXPECT_EQ(std::addressof(val1), std::addressof(val2));
}

TEST(MapElementAccess, SquareBracketsInsertion)
{
  const MapT expected{ { 1, "abc" } };
  MapT map;
  map[1] = "abc";
  test_invariants(map);
  EXPECT_EQ(map, expected);
}

TEST(MapElementAccess, AtAccess)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT::mapped_type& val1 = map.at(2);
  const MapT::mapped_type& val2 = map.at(2);
  test_invariants(map);
  EXPECT_EQ(std::addressof(val1), std::addressof(val2));
}

TEST(MapElementAccess, AtOutOfRange)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  EXPECT_THROW(map.at(4), std::out_of_range);
  test_invariants(map);
}

TEST(MapModifiers, Insert)
{
  const MapT expected{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  MapT map{
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT::value_type value = { 1, "abc" };
  const auto res = map.insert(value);
  test_invariants(map);
  EXPECT_EQ(map, expected);
  EXPECT_EQ(res.first, map.begin());
  EXPECT_TRUE(res.second);
}

TEST(MapModifiers, InsertShuffledKeys)
{
  MapT map;
  std::vector<MapT::key_type> keys(1'500);
  std::iota(keys.begin(), keys.end(), 0);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(keys.begin(), keys.end(), gen);
  for (const auto& key : keys) {
    EXPECT_TRUE(map.insert(std::make_pair(key, "")).second);
    test_invariants(map);
  }
  std::sort(keys.begin(), keys.end(), map.key_comp());
  const auto cmp = [](const auto& lhs, const auto& rhs) -> bool {
    return lhs == rhs.first;
  };
  EXPECT_TRUE(
      std::equal(keys.begin(), keys.end(), map.begin(), map.end(), cmp));
}

TEST(MapModifiers, InsertShuffledHints)
{
  MapT map;
  std::vector<MapT::key_type> keys(1'500);
  std::iota(keys.begin(), keys.end(), 0);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(keys.begin(), keys.end(), gen);
  for (const auto& key : keys) {
    std::uniform_int_distribution<> distr(0, map.size());
    auto pos = std::next(map.begin(), distr(gen));
    map.insert(pos, std::make_pair(key, ""));
    test_invariants(map);
  }
  std::sort(keys.begin(), keys.end(), map.key_comp());
  const auto cmp = [](const auto& lhs, const auto& rhs) -> bool {
    return lhs == rhs.first;
  };
  EXPECT_TRUE(
      std::equal(keys.begin(), keys.end(), map.begin(), map.end(), cmp));
}

TEST(MapModifiers, InsertEmptyRange)
{
  MapT map;
  const std::initializer_list<MapT::value_type> init;
  map.insert(init.begin(), init.end());
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapModifiers, InsertRange)
{
  const MapT expected{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  MapT map;
  map.insert(expected.begin(), expected.end());
  test_invariants(map);
  EXPECT_EQ(map, expected);
}

TEST(MapModifiers, InsertInitializerList)
{
  const std::initializer_list<MapT::value_type> init{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT initied(init);
  MapT map;
  map.insert(init);
  test_invariants(map);
  EXPECT_EQ(map, initied);
}

TEST(MapModifiers, EraseAllFormEmpty)
{
  MapT map;
  map.erase(map.begin(), map.end());
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapModifiers, EraseAll)
{
  MapT map;
  std::vector<MapT::key_type> keys(500);
  std::iota(keys.begin(), keys.end(), -1);
  for (const auto& key : keys) {
    map.insert(std::make_pair(key, ""));
  }
  map.erase(map.begin(), map.end());
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapModifiers, EraseKey)
{
  const MapT expected{
    { 1, "abc" },
    { 3, "ghi" },
  };
  MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  map.erase(2);
  test_invariants(map);
  EXPECT_EQ(map, expected);
}

TEST(MapModifiers, EraseShuffeledKeys)
{
  MapT map;
  std::vector<MapT::key_type> keys(1'500);
  std::iota(keys.begin(), keys.end(), 0);
  for (const auto& key : keys) {
    map.insert(std::make_pair(key, ""));
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(keys.begin(), keys.end(), gen);
  size_t size = keys.size();
  for (const auto& key : keys) {
    EXPECT_EQ(map.erase(key), 1);
    test_invariants(map);
    --size;
    EXPECT_EQ(map.size(), size);
  }
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapModifiers, EraseShuffledPos)
{
  MapT map;
  std::vector<MapT::key_type> keys(1'500);
  std::iota(keys.begin(), keys.end(), 0);
  for (const auto& key : keys) {
    map.insert(std::make_pair(key, ""));
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  for (size_t i = keys.size(); i != 0; --i) {
    std::uniform_int_distribution<> distr(0, map.size() - 1);
    auto pos = std::next(map.begin(), distr(gen));
    EXPECT_EQ(map.size(), i);
    map.erase(pos);
    test_invariants(map);
  }
  EXPECT_TRUE(map.empty());
  test_invariants(map);
}

TEST(MapModifiers, ClearEmpty)
{
  MapT map;
  map.clear();
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapModifiers, Clear)
{
  MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  map.clear();
  test_invariants(map);
  EXPECT_TRUE(map.empty());
}

TEST(MapModifiers, Swap)
{
  MapT first{ { 1, "abc" } };
  MapT second{ { 2, "def" } };
  const MapT firstCopy(first);
  const MapT secondCopy(second);
  first.swap(second);
  test_invariants(first);
  test_invariants(second);
  EXPECT_EQ(first, secondCopy);
  EXPECT_EQ(second, firstCopy);
}

TEST(MapModifiers, EmplaceEmpty)
{
  const MapT::value_type value = { 1, "abc" };
  MapT map;
  const auto res = map.emplace(value);
  test_invariants(map);
  EXPECT_EQ(map.size(), 1);
  EXPECT_EQ(*map.begin(), value);
  EXPECT_EQ(res.first, map.begin());
  EXPECT_TRUE(res.second);
}

TEST(MapModifiers, Emplace)
{
  MapT map{
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT::value_type value = { 1, "abc" };
  map.emplace(value);
  test_invariants(map);
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(*map.begin(), value);
}

TEST(MapModifiers, EmplaceHint)
{
  MapT map{
    { 2, "def" },
    { 3, "ghi" },
  };
  const MapT::value_type value = { 1, "abc" };
  map.emplace_hint(map.begin(), value);
  test_invariants(map);
  EXPECT_EQ(map.size(), 3);
  EXPECT_EQ(*map.begin(), value);
}

TEST(MapOperations, FindInEmpty)
{
  const MapT map;
  const auto res = map.find(1);
  test_invariants(map);
  EXPECT_TRUE(map.empty());
  EXPECT_EQ(res, map.end());
}

TEST(MapOperations, FindExisting)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const auto res = map.find(1);
  test_invariants(map);
  EXPECT_EQ(res, map.begin());
}

TEST(MapOperations, FindNonExistent)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const auto res = map.find(4);
  test_invariants(map);
  EXPECT_EQ(res, map.end());
}

TEST(MapOperations, Count)
{
  const MapT map{
    { 2, "def" },
  };
  const size_t res = map.count(2);
  test_invariants(map);
  EXPECT_EQ(res, 1);
}

TEST(MapOperations, LowerBoundEmpty)
{
  const MapT map;
  const auto res = map.lower_bound(1);
  test_invariants(map);
  EXPECT_EQ(res, map.end());
}

TEST(MapOperations, LowerBound)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const auto res = map.lower_bound(2);
  test_invariants(map);
  EXPECT_EQ(res, ++map.begin());
}

TEST(MapOperations, UpperBoundEmpty)
{
  const MapT map;
  const auto res = map.lower_bound(1);
  test_invariants(map);
  EXPECT_EQ(res, map.end());
}

TEST(MapOperations, UpperBound)
{
  const MapT map{
    { 1, "abc" },
    { 2, "def" },
    { 3, "ghi" },
  };
  const auto res = map.upper_bound(1);
  test_invariants(map);
  EXPECT_EQ(res, ++map.begin());
}

TEST(MapOperations, EqualRange)
{
  const MapT map{
    { 1, "abc" },
  };
  const auto res = map.equal_range(1);
  test_invariants(map);
  EXPECT_EQ(res.first, map.begin());
  EXPECT_EQ(res.second, map.end());
}

TEST(MapComparison, CompareEmpty)
{
  const MapT map1;
  const MapT map2;
  EXPECT_TRUE(!(map1 < map2));
  EXPECT_TRUE(!(map1 > map2));
  EXPECT_EQ(map1, map2);
  test_comparison(map1, map2);
}

TEST(MapComparison, EqualityOperatorDifferentSizes)
{
  const MapT map1{ { 1, "" } };
  const MapT map2{ { 1, "" }, { 2, "" } };
  EXPECT_NE(map1, map2);
  test_comparison(map1, map2);
}

TEST(MapComparison, EqualityOperatorDifferentValues)
{
  const MapT map1{ { 1, "" } };
  const MapT map2{ { 2, "" } };
  EXPECT_NE(map1, map2);
  test_comparison(map1, map2);
}

TEST(MapComparison, EqualityOperatorSame)
{
  const MapT map1{ { 1, "" } };
  const MapT map2{ { 1, "" } };
  EXPECT_EQ(map1, map2);
  test_comparison(map1, map2);
}

TEST(MapComparison, LessOperatorDifferentSizes)
{
  const MapT map1{ { 1, "" } };
  const MapT map2{ { 1, "" }, { 2, "" } };
  EXPECT_LT(map1, map2);
  test_comparison(map1, map2);
}

TEST(MapComparison, LessOperatorDifferentValues)
{
  const MapT map1{ { 1, "" }, { 2, "" } };
  const MapT map2{ { 1, "" }, { 3, "" } };
  EXPECT_LT(map1, map2);
  test_comparison(map1, map2);
}

template <std::input_iterator Iter>
std::vector<int> extract_keys(Iter begin, const Iter end)
{
  std::vector<int> keys;
  for (; begin != end; ++begin) {
    keys.push_back(begin->first);
  }
  return keys;
}

struct MapIteratorFixture : testing::Test
{
  MapT map;
  std::vector<MapT::key_type> mapKeys;
  std::vector<int> bfsExpected;

  MapIteratorFixture() :
    map(),
    mapKeys(15),
    bfsExpected{ 7, 3, 11, 1, 5, 9, 13, 0, 2, 4, 6, 8, 10, 12, 14 }
  {
    std::iota(mapKeys.begin(), mapKeys.end(), 0);
    for (auto key : mapKeys) {
      map.emplace(key, "");
    }
  }
};

TEST_F(MapIteratorFixture, Iterator)
{
  auto keys = extract_keys(map.begin(), map.end());
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, LmrIterator)
{
  auto keys = extract_keys(map.lmr_begin(), map.lmr_end());
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, RmlIterator)
{
  auto keys = extract_keys(map.rml_begin(), map.rml_end());
  std::reverse(keys.begin(), keys.end());
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, BfsIterator)
{
  auto keys = extract_keys(map.bfs_begin(), map.bfs_end());
  EXPECT_EQ(keys, bfsExpected);
}

TEST_F(MapIteratorFixture, IteratorToLmr)
{
  auto keys = extract_keys(MapT::lmr_iterator(map.begin()), map.lmr_end());
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, IteratorToRml)
{
  auto keys = extract_keys(MapT::rml_iterator(map.begin()), map.rml_end());
  std::reverse(keys.begin(), keys.end());
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, IteratorToBfs)
{
  auto keys = extract_keys(MapT::bfs_iterator(map.begin()), map.bfs_end());
  EXPECT_EQ(keys, bfsExpected);
}

TEST_F(MapIteratorFixture, LmrToIterator)
{
  auto keys = extract_keys(MapT::iterator(map.lmr_begin()), map.end());
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, RmlToIterator)
{
  auto begin = std::make_reverse_iterator(MapT::iterator(map.rml_begin()));
  auto end = std::make_reverse_iterator(map.begin());
  auto keys = extract_keys(begin, end);
  std::reverse(keys.begin(), keys.end());
  keys.push_back(MapT::iterator(map.rml_begin())->first);
  EXPECT_EQ(keys, mapKeys);
  EXPECT_EQ(keys.size(), map.size());
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}

TEST_F(MapIteratorFixture, BfsToIterator)
{
  const std::vector<int> expected = { 7, 8, 9, 10, 11, 12, 13, 14 };
  auto keys = extract_keys(MapT::iterator(map.bfs_begin()), map.end());
  EXPECT_EQ(keys, expected);
  EXPECT_TRUE(std::is_sorted(keys.begin(), keys.end(), map.key_comp()));
}
