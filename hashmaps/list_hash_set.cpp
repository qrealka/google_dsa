/*
Design a HashSet without using any built-in hash table libraries.

Implement MyHashSet class:

void Add(key) Inserts the value key into the HashSet.
bool Contains(key) Returns whether the value key exists in the HashSet or not.
void Remove(key) Removes the value key in the HashSet. If key does not exist in the HashSet, do nothing.

Constraints:
0 <= key <= 1'000'000
At most 10'000 calls will be made to add, remove, and contains.
 */

#include <array>
#include <algorithm>
#include <cassert>
#include <list>
#include <memory_resource>
#include <iostream>
#include <memory>

namespace {

/**
* hashset from-scratch based on array + list to experiments with PMR
*/
class MyHashSet {
  static constexpr int maxEntries = 10'000 + 1;
  static std::pmr::monotonic_buffer_resource mbr; // non-buffered resource, I don't care about the performance

  struct Entity {
    int key;
  };

  struct bucket {
    std::pmr::list<Entity> values;
  };

  std::pmr::polymorphic_allocator<int> pa{&mbr}; // it is just a pointer to the resource
  std::array<bucket, maxEntries> buckets{};

  static int getBucket(const int key) noexcept {
    static_assert(maxEntries > 0, "maxEntries must be greater than 0");;
    return key % maxEntries;
  }
public:
  void Add(int key) noexcept {
    auto&[values] = buckets.at(getBucket(key));
    for (const auto&[k] : values) {
      if (k == key) {
        return;
      }
    }
    values.emplace_back(key);
  }

  [[nodiscard]] bool Contains(int key) const noexcept {
    auto&[values] = buckets.at(getBucket(key));
    return std::ranges::any_of(values, [key](const auto& entity) {
      return entity.key == key;
    });
  }

  void Remove(int key) noexcept {
    auto&[values] = buckets.at(getBucket(key));
    values.remove_if([key](const auto& entity) {
      return entity.key == key;
    });
  }
};

std::pmr::monotonic_buffer_resource MyHashSet::mbr{};

}

int main () {
  const auto obj = std::make_unique<MyHashSet> ();
  obj->Add(1);
  obj->Add(2);
  if (obj->Contains(1)) {
    std::cout << "1 is in the set" << std::endl;
  } else {
    assert(false && "1 is not in the set");
  }
  if (!obj->Contains(3)) {
    std::cout << "3 is not in the set" << std::endl;
  } else {
    assert(false && "3 is in the set");
  }

  obj->Add(2);
  if (obj->Contains(2)) {
    std::cout << "2 is still in the set" << std::endl;
  } else {
    assert(false && "2 is not in the set");
  }
  obj->Remove(2);
  if (!obj->Contains(2)) {
    std::cout << "2 is not in the set" << std::endl;
  } else {
    assert(false && "2 is in the set");
  }
}