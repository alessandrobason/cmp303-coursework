#include "convert.h"

#include <socket.h>

void convertToNet(const std::array<char, 8> &data, void *outdata) {
    memcpy(outdata, &data[0], sizeof(data));
}

void convertToNet(const vec2i &data, void *outdata) {
    i32 *out = (i32 *)outdata;
    out[0] = htonl(data.x);
    out[1] = htonl(data.y);
}

void convertToNet(const vec2b &data, void *outdata) {
    memcpy(outdata, &data, sizeof(data));
}

/////////////////////////////////////////////////////////////////////

void convertToHost(const void *data, u8 &outdata) {
    memcpy(&outdata, data, sizeof(outdata));
}

void convertToHost(const void *data, vec2i &outdata) {
    i32 *in = (i32 *)data;
    outdata.x = ntohl(in[0]);
    outdata.y = ntohl(in[1]);
}

void convertToHost(const void *data, AddPowerup &outdata) {
    u8 *in_type = (u8 *)data;
    outdata.type = *in_type;
    in_type += sizeof(u8);
    i32 *in_vec = (i32 *)in_type;
    outdata.pos.x = ntohl(in_vec[0]);
    outdata.pos.y = ntohl(in_vec[1]);
}

void convertToHost(const void *data, AddRemMsg &outdata) {
    u8 *in_id = (u8 *)data;
    outdata.id = *in_id;
    in_id += sizeof(u8);
    i32 *in_vec = (i32 *)in_id;
    outdata.pos.x = ntohl(in_vec[0]);
    outdata.pos.y = ntohl(in_vec[1]);
}

void convertToHost(const void *data, PlayerName &outdata) {
    memcpy(&outdata, data, sizeof(outdata));
}

void convertToHost(const void *data, PlayerNew &outdata) {
    memcpy(&outdata, data, sizeof(outdata));
}

void convertToHost(const void *data, PlayerAnim &outdata) {
    memcpy(&outdata, data, sizeof(outdata));
}

void convertToHost(const void *data, PlayerPos &outdata) {
    u8 *in_id = (u8 *)data;
    outdata.id = *in_id;
    in_id += sizeof(u8);
    i32 *in_vec = (i32 *)in_id;
    outdata.pos.x = ntohl(in_vec[0]);
    outdata.pos.y = ntohl(in_vec[1]);
}
