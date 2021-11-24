#pragma once

#include <array>
#include <utils/vec.h>

#include "net_types.h"

void convertToNet(const u8 &data, void *outdata);
void convertToNet(const vec2i &data, void *outdata);
void convertToNet(const AddPowerup &data, void *outdata);
void convertToNet(const AddRemMsg &data, void *outdata);
void convertToNet(const PlayerName &data, void *outdata);
void convertToNet(const PlayerNew &data, void *outdata);
void convertToNet(const PlayerAnim &data, void *outdata);
void convertToNet(const PlayerPos &data, void *outdata);

// disable implicit conversion
template <class T>
void convertToNet(const T &data, void *outdata) = delete;

void convertToHost(const void *data, std::array<char, 8> &outdata);
void convertToHost(const void *data, vec2i &outdata);
void convertToHost(const void *data, vec2b &outdata);

// disable implicit conversion
template <class T>
void convertToHost(const void *data, T &outdata) = delete;