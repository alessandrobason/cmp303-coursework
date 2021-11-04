#pragma once

#include <string>
#include <iterator>
#include <cJSON.h>
#include <utils/types.h>

namespace json {
    class Document;
    class Object;

    const char *getError();

    class Document {
    public:
        Document(const std::string &fname);
        Document(cJSON *from) : m_data(from) {}
        Document(Document &&other);
        Document(Document &other) = delete;
        ~Document();

        Object operator[](const char *key);
        Object getRoot();

        std::string print(bool formatted = true);

        inline cJSON *cjsonPtr() { return m_data; }
    
    private:
        cJSON *m_data = nullptr;
    };

    struct Iterator {
        Iterator(cJSON *m_data) : m_data(m_data) {}

        Object operator*();
        Iterator &operator++();
        Iterator &operator--();
        bool operator==(const Iterator &other) const;
        bool operator!=(const Iterator &other) const;

    private:
        cJSON *m_data;
    };

    class Array {
    public:
        Array() = default;
        Array(int size, cJSON *data) : 
            m_size(size), m_data(data) {}
        inline int size() const { return m_size; }

        Object operator[](int index);

        inline Iterator begin() { return m_data->child; }
        inline Iterator end() { return nullptr; }

        inline const char *key() { return m_data->string; }

    private:
        int m_size = 0;
        cJSON *m_data = nullptr;
    };

    class Object {
    public:
        Object() = default;
        Object(cJSON *from) : m_data(from) {}

        Object operator[](const char *key);
        Object operator[](int index);

        inline Iterator begin() { return m_data->child; }
        inline Iterator end() { return nullptr; }

        operator double() { return getDouble(); }
        operator i64() { return getInteger(); }
        operator Array() { return getArray(); }
        operator char *() { return getString(); }
        operator bool() { return getBool(); }

        double getDouble();
        i64 getInteger();
        Array getArray();
        char *getString();
        bool getBool();

        std::string print(bool formatted = true);

        inline bool isValid()  const { return !cJSON_IsInvalid(m_data); }
        inline bool isFalse()  const { return cJSON_IsFalse(m_data); }
        inline bool isTrue()   const { return cJSON_IsTrue(m_data); }
        inline bool isBool()   const { return cJSON_IsBool(m_data); }
        inline bool isNull()   const { return cJSON_IsNull(m_data); }
        inline bool isNumber() const { return cJSON_IsNumber(m_data); }
        inline bool isString() const { return cJSON_IsString(m_data); }
        inline bool isArray()  const { return cJSON_IsArray(m_data); }
        inline bool isObject() const { return cJSON_IsObject(m_data); }
        inline bool isRaw()    const { return cJSON_IsRaw(m_data); }

        inline cJSON *cjsonPtr() { return m_data; }
        inline const char *key() { return m_data->string; }

    private:
        cJSON *m_data = nullptr;
    };

    // == ITERATOR INLINE FUNCTIONS =================================

    inline Object Iterator::operator*() {
        return m_data;
    }

    inline Iterator &Iterator::operator++() {
        m_data = m_data->next;
        return *this;
    }

    inline Iterator &Iterator::operator--() {
        m_data = m_data->prev;
        return *this;
    }

    inline bool Iterator::operator==(const Iterator &other) const {
        return m_data == other.m_data;
    }

    inline bool Iterator::operator!=(const Iterator &other) const {
        return m_data != other.m_data;
    }
} // namespace json