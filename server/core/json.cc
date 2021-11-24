#include "json.h"

#include <utility> // std::move
#include <assert.h>
#include <tracelog.h>
#include <file.h>

namespace json {
    const char *getError() {
        const char *error = cJSON_GetErrorPtr();
        if(error) {
            return error - 4;
        }
        return "";
    }

    // == DOCUMENT ===========================================================================

    Document::Document(const std::string &fname) {
        str_t f_data = fileReadWholeText(fname.c_str());
        if(f_data.buf == NULL) {
            fatal("couldn't read file %s", fname.c_str());
            return;
        }

        m_data = cJSON_ParseWithLength(f_data.buf, f_data.len);
        strFree(&f_data);
        
        if(!m_data) {
            fatal("couldn't parse %s -> %s", fname.c_str(), getError());
        }
    }


    Document::Document(Document &&other) {
        m_data = other.m_data;
        other.m_data = nullptr;
    }

    Document::~Document() {
        // cJSON already checks for NULL
        cJSON_Delete(m_data);
    }

    std::string Document::print(bool formatted) {
        // this copies a lot, it's probably not a problem
        char *printed = formatted ? cJSON_Print(m_data) : cJSON_PrintUnformatted(m_data);
        std::string result{ printed };
        free(printed);
        return result;
    }

    Object Document::operator[](const char *key) {
        cJSON *obj = cJSON_GetObjectItemCaseSensitive(m_data, key);
        if(!obj) {
            fatal("error while retreiving key %s", key);
        }
        return obj;
    }

    Object Document::getRoot() {
        return m_data;
    }


    // == VALUE ==============================================================================

    Object Object::operator[](const char *key) {
        cJSON *obj = cJSON_GetObjectItemCaseSensitive(m_data, key);
        if(!obj) {
            fatal("error while retreiving key %s", key);
        }
        return obj;
    }

    Object Object::operator[](int index) {
        cJSON *value = cJSON_GetArrayItem(m_data, index);
        if(!value) {
            fatal("error while retreiving index %d", index);
        }
        return value;
    }

    double Object::getDouble() {
        if(!isNumber()) fatal("key %s is not number", key());
        return cJSON_GetNumberValue(m_data);
    }

    i64 Object::getInteger() {
        if(!isNumber()) fatal("key %s is not number", key());
        return static_cast<i64>(cJSON_GetNumberValue(m_data));
    }

    Array Object::getArray() {
        if(!isArray()) fatal("key %s is not array", key());
        int size = cJSON_GetArraySize(m_data);
        return { size, m_data };
    }

    char *Object::getString() {
        if(!isString()) fatal("key %s is not string", key());
        return cJSON_GetStringValue(m_data);;
    }

    bool Object::getBool() {
        if(!isBool()) fatal("key %s is not bool", key());
        return isTrue();
    }

    std::string Object::print(bool formatted) {
        // this copies a lot but i really don't care
        char *printed = formatted ? cJSON_Print(m_data) : cJSON_PrintUnformatted(m_data);
        std::string result{ printed };
        free(printed);
        return result;
    }

    // == ARRAY ==============================================================================
    
    Object Array::operator[](int index) {
        if(index < 0 || index >= m_size) {
            fatal("(json) index too high [index: %d, size: %d]", index, m_size);
        }
        cJSON *value = cJSON_GetArrayItem(m_data, index);
        if(!value) {
            fatal("error while retreiving index %d", index);
        }
        return value;
    }
} // namespace json