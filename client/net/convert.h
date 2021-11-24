#pragma once

#include <array>
#include <utils/vec.h>

#include "net_types.h"

void convertToNet(const std::array<char, 8> &data, void *outdata);
void convertToNet(const vec2i &data, void *outdata);
void convertToNet(const vec2b &data, void *outdata);

// disable implicit conversion
template <class T>
void convertToNet(const T &data, void *outdata) = delete;

void convertToHost(const void *data, u8 &outdata);
void convertToHost(const void *data, vec2i &outdata);
void convertToHost(const void *data, AddPowerup &outdata);
void convertToHost(const void *data, AddRemMsg &outdata);
void convertToHost(const void *data, PlayerName &outdata);
void convertToHost(const void *data, PlayerNew &outdata);
void convertToHost(const void *data, PlayerAnim &outdata);
void convertToHost(const void *data, PlayerPos &outdata);

// disable implicit conversion
template <class T>
void convertToHost(const void *data, T &outdata) = delete;