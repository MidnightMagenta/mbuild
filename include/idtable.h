#ifndef MBUILD_IDTABLE_H
#define MBUILD_IDTABLE_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <vector>

namespace mb {
template<typename T>
class Table {
public:
    using ID = size_t;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T *;
        using reference         = T &;

    public:
        Iterator(T **cur, T **end)
            : m_cur(cur),
              m_end(end) {
            skip_null();
        }

        reference operator*() const {
            return **m_cur;
        }

        pointer operator->() {
            return *m_cur;
        }

        Iterator &operator++() {
            ++m_cur;
            skip_null();
            return *this;
        }

        bool operator==(const Iterator &other) const {
            return m_cur == other.m_cur;
        }

        bool operator!=(const Iterator &other) const {
            return m_cur != other.m_cur;
        }

    private:
        void skip_null() {
            while (m_cur != m_end && *m_cur == nullptr) { m_cur++; }
        }

    private:
        T **m_cur;
        T **m_end;
    };

    class ConstIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = const T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T *;
        using reference         = const T &;

    public:
        ConstIterator(T *const *cur, T *const *end)
            : m_cur(cur),
              m_end(end) {
            skip_null();
        }

        reference operator*() const {
            return **m_cur;
        }

        pointer operator->() const {
            return *m_cur;
        }

        ConstIterator &operator++() {
            ++m_cur;
            skip_null();
            return *this;
        }

        bool operator==(const ConstIterator &other) const {
            return m_cur == other.m_cur;
        }

        bool operator!=(const ConstIterator &other) const {
            return m_cur != other.m_cur;
        }

    private:
        void skip_null() {
            while (m_cur != m_end && *m_cur == nullptr) { ++m_cur; }
        }

        T *const *m_cur;
        T *const *m_end;
    };

public:
    Table() {
        m_slots = new T *[m_capacity]();
        for (size_t i = 0; i < m_capacity; i++) { m_freeList.push_back(i); }
    }

    ~Table() {
        for (size_t i = 0; i < m_capacity; i++) { delete m_slots[i]; }
        delete[] m_slots;
    }

    Table(const Table &)            = delete;
    Table &operator=(const Table &) = delete;

    Iterator begin() {
        return Iterator(m_slots, m_slots + m_capacity);
    }

    Iterator end() {
        return Iterator(m_slots + m_capacity, m_slots + m_capacity);
    }

    ConstIterator begin() const {
        return ConstIterator(m_slots, m_slots + m_capacity);
    }

    ConstIterator end() const {
        return ConstIterator(m_slots + m_capacity, m_slots + m_capacity);
    }

    ConstIterator cbegin() const {
        return ConstIterator(m_slots, m_slots + m_capacity);
    }

    ConstIterator cend() const {
        return ConstIterator(m_slots + m_capacity, m_slots + m_capacity);
    }

    void reserve(size_t size) {
        if (size > m_capacity) { grow(size); }
    }

    template<typename... Args>
    ID emplace(Args &&...args) {
        if (m_freeList.empty()) { grow(m_capacity > 512 ? (m_capacity ? m_capacity * 2 : 64) : 1024); }
        ID id = m_freeList.back();
        m_freeList.pop_back();

        m_slots[id] = new T(std::forward<Args>(args)...);

        return id;
    }

    void free(ID id) {
        assert(id < m_capacity);
        assert(m_slots[id] != nullptr);
        delete m_slots[id];
        m_slots[id] = nullptr;
        m_freeList.push_back(id);
    }

    T &get(ID id) {
        assert(id < m_capacity);
        assert(m_slots[id] != nullptr);
        return *m_slots[id];
    }

    const T &get(ID id) const {
        assert(id < m_capacity);
        assert(m_slots[id] != nullptr);
        return *m_slots[id];
    }

    size_t capacity() const {
        return m_capacity;
    }
    size_t size() const {
        return m_capacity - m_freeList.size();
    }
    size_t free_size() const {
        return m_freeList.size();
    }

    T &operator[](ID id) {
        return get(id);
    }

    const T &operator[](ID id) const {
        return get(id);
    }

private:
    void grow(size_t newCapacity) {
        assert(newCapacity > m_capacity);
        T **newSlots = new T *[newCapacity]();
        memcpy(newSlots, m_slots, m_capacity * sizeof(T *));
        delete[] m_slots;
        m_slots = newSlots;
        for (size_t i = m_capacity; i < newCapacity; i++) { m_freeList.push_back(i); }
        m_capacity = newCapacity;
    }

private:
    T             **m_slots    = nullptr;
    size_t          m_capacity = 64;
    std::vector<ID> m_freeList;
};
}// namespace mb

#endif
