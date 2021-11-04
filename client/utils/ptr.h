#pragma once

#include <memory>

template<typename T>
using ptr = std::unique_ptr<T>;

template<typename T>
using shared_ptr = std::shared_ptr<T>;