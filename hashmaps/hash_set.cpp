/*
Design a HashSet without using any built-in hash table libraries.

Implement MyHashSet class:

void add(key) Inserts the value key into the HashSet.
bool contains(key) Returns whether the value key exists in the HashSet or not.
void remove(key) Removes the value key in the HashSet. If key does not exist in the HashSet, do nothing.

Constraints:
0 <= key <= 106
At most 10'000 calls will be made to add, remove, and contains.
 */
#include <cassert>
#include <cstdint>
#include <memory>
#include <iostream>


namespace {
    /**
     * I don't care too much about all cases, so I do not implement extendable buckets.
     * Each bucket has 8 entries, and the maximum number of entries is 8192.
     * I don't use key value as a hash value, but I use a hash function to get the bucket.
     *
     * PS: it could be easy to make it fully constexpr
     */
    class MyHashSet {
    public:
        using key_type = int;

    private:
        static constexpr size_t entriesPerBucket = 4;
        static constexpr size_t maxEntries = 8192 * entriesPerBucket;
        static_assert((maxEntries & (maxEntries - 1)) == 0, "maxEntries must be a power of two");

        static constexpr int bucketMask = (maxEntries / entriesPerBucket) - 1;
        static constexpr int bucketShift = [](const size_t perBucket) constexpr -> int {
            int shift = 0;
            for (int i = 1; (perBucket & i) == 0; ++shift, i <<= 1) {}
            return shift;
        }(entriesPerBucket);




        struct Entry {
            key_type key;

            [[nodiscard]] bool isEntry() const noexcept {
                return key >= 0;
            }

            void reset() noexcept {
                key = -1;
            }
        };

        Entry entries[maxEntries]{};
        uint16_t size = 0;

        static uint32_t fnv_hash(const int key) noexcept {
            uint32_t result = 0x811c9dc5;
            result ^= key;
            result *= 0x01000193;
            return result;
        }

        static uint32_t getBucket(const int key) noexcept {
            return (fnv_hash(key) & bucketMask) << bucketShift;
        }

        void addWithBucket(const key_type key, const uint32_t bucket) noexcept {
            const Entry newEntry{ key };

            for (size_t i = 0; i < entriesPerBucket; ++i) {
                if (!entries[bucket + i].isEntry()) {
                    entries[bucket + i] = newEntry;
                    ++size;
                    return;
                }
                if (entries[bucket + i].key == key) {
                    return;
                }
            }
            assert(false && "Should not reach here");
        }

        void removeWithBucket(const key_type key, const uint32_t bucket) noexcept {
            int lastEntry = entriesPerBucket - 1;
            for (size_t i = 0; i < entriesPerBucket; ++i) {
                if (entries[bucket + i].key == key) {
                    for (int j = entriesPerBucket - 1; j > i; --j) {
                        if (!entries[bucket + j].isEntry()) {
                            lastEntry = j - 1;
                        }
                    }
                    // move entry
                    entries[bucket + i] = entries[bucket + lastEntry];
                    entries[bucket + lastEntry].reset();
                    --size;
                    return;
                }
            }
        }

        [[nodiscard]] bool lookupWithBucket(const key_type key, const uint32_t bucket) const noexcept {
            for (int i = 0; i < entriesPerBucket; ++i) {
                if (entries[bucket + i].isEntry() && entries[bucket + i].key == key) {
                    return true;
                }
            }
            return false;
        }
    public:
        MyHashSet() {
            for(auto&& e : entries) {
                e.reset();
            }
        }

        void Add(const key_type key) noexcept {
            addWithBucket(key, getBucket(key));
        }

        void Remove(const key_type key) {
            if (size) {
                removeWithBucket(key, getBucket(key));
            }
        }

        [[nodiscard]] bool Contains(const key_type key) const {
            if (size) {
                return lookupWithBucket(key, getBucket(key));
            }
            return false;
        }
    };


    /**
     * Your MyHashSet object will be instantiated and called as such:
     * obj := Constructor();
     * obj.Add(key);
     * obj.Remove(key);
     * param_3 := obj.Contains(key);
     */
}

int main() {
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
