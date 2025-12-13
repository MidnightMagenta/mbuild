#ifndef MBUILD_TABLE_H
#define MBUILD_TABLE_H

#include <cassert>
#include <optional>
#include <unordered_map>
#include <vector>

namespace mb {
template<typename T>
class Table {
public:
    using ID = size_t;

public:
    Table() {}
    ~Table() {}

    template<typename... Args>
    ID emplace(Args &&...args) {
        ID id = m_data.size();
        m_data.emplace_back(std::forward<Args>(args)...);
        return id;
    }

    ID insert(T element) {
        ID id = m_data.size();
        m_data.push_back(std::move(element));
        return id;
    }

    size_t                size() const { return m_data.size(); }
    const std::vector<T> &data() const { return m_data; }

    T &operator[](ID id) {
        assert(id < m_data.size());
        return m_data[id];
    }

    const T &operator[](ID id) const {
        assert(id < m_data.size());
        return m_data[id];
    }

private:
    std::vector<T> m_data;
};

template<typename key_t, typename T>
class NamedTable {
public:
    using ID = mb::Table<T>::ID;

public:
    NamedTable() {}
    ~NamedTable() {}

    template<typename... Args>
    ID emplace(const key_t &name, Args &&...args) {
        ID id = m_data.emplace(std::forward<Args>(args)...);
        m_idMap.insert(name, id);
        return id;
    }

    ID insert(const key_t &name, T element) {
        ID id = m_data.insert(element);
        m_idMap.insert(name, id);
        return id;
    }

    size_t                size() const { return m_data.size(); }
    const mb::Table<T>   &table() const { return m_data; }
    const std::vector<T> &data() const { return m_data.data(); }

    std::optional<ID> get_id(const key_t &name) const {
        auto it = m_idMap.find(name);
        if (it != m_idMap.end()) { return it->second; }
        return std::nullopt;
    }

    T *get(const key_t &key) {
        auto it = m_idMap.find(key);
        if (it == m_idMap.end()) { return nullptr; }
        return &m_data[it->second];
    }

    const T *get(const key_t &key) const {
        auto it = m_idMap.find(key);
        if (it == m_idMap.end()) { return nullptr; }
        return &m_data[it->second];
    }

    T       &operator[](ID id) { return m_data[id]; }
    const T &operator[](ID id) const { return m_data[id]; }

private:
    mb::Table<T>                  m_data;
    std::unordered_map<key_t, ID> m_idMap;
};
}// namespace mb

#endif
