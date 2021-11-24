#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"

template<typename T, typename SizeType = u32>
class dynarray {
public:
    dynarray() = default;

    dynarray(dynarray &&other) {
        m_data = other.m_data;
        m_size = other.m_size;

        other.m_data = nullptr;
        other.m_size = 0;
    }

    dynarray(SizeType count) {
        init(count);
    }

    ~dynarray() {
        destroy();
    }

    dynarray &operator=(dynarray &&other) {
        if(this != &other) {
            m_data = other.m_data;
            m_size = other.m_size;

            other.m_data = nullptr;
            other.m_size = 0;
        }

        return *this;
    }

    void init(SizeType count) {
        m_size = count;

        assert(m_data == nullptr);
        m_data = new T[count];
        assert(m_data != nullptr);
    }

    void destroy() {
        if(m_data) {
            delete[] m_data;
        }
        m_data = nullptr;
        m_size = 0;
    }

    T *data() {
        return m_data;
    }

    const T *data() const {
        return m_data;
    }

    SizeType size() {
        return m_size;
    }
    
    const SizeType size() const {
        return m_size;
    }

    bool empty() const {
        return m_size == 0;
    }

    T &operator[](SizeType index) {
        assert(index < m_size);
        return m_data[index];
    }

    const T &operator[](SizeType index) const {
        assert(index < m_size);
        return m_data[index];
    }

    T &front() {
        return m_data[0];
    }

    const T &front() const {
        return m_data[0];
    }

    T &back() {
        return m_data[m_size - 1];
    }

    const T &back() const {
        return m_data[m_size - 1];
    }

    T *begin() {
        return m_data;
    }

    const T *begin() const {
        return m_data;
    }

    T *end() {
        return m_data + m_size;
    }

    const T *end() const {
        return m_data + m_size;
    }

private:
    T *m_data = nullptr;
    SizeType m_size = 0;
};