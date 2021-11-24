#include "convert.h"

#include <socket.h>

void convertToNet(const u8 &data, void *outdata) {
    memcpy(outdata, &data, sizeof(data));
}

void convertToNet(const vec2i &data, void *outdata) {
    i32 *out = (i32 *)outdata;
    out[0] = htonl(data.x);
    out[1] = htonl(data.y);
}

void convertToNet(const AddPowerup &data, void *outdata) {
    u8 *outtype = (u8 *)outdata;
    memcpy(outtype, &data.type, sizeof(data.type));
    outtype += sizeof(data.type);
    i32 *outvec = (i32 *)outtype;
    outvec[0] = htonl(data.pos.x);
    outvec[1] = htonl(data.pos.y);
}

void convertToNet(const AddRemMsg &data, void *outdata) {
    u8 *outid = (u8 *)outdata;
    memcpy(outid, &data.id, sizeof(data.id));
    outid += sizeof(data.id);
    i32 *outvec = (i32 *)outid;
    outvec[0] = htonl(data.pos.x);
    outvec[1] = htonl(data.pos.y);
}

void convertToNet(const PlayerName &data, void *outdata) {
    memcpy(outdata, &data, sizeof(data));
}

void convertToNet(const PlayerNew &data, void *outdata) {
    memcpy(outdata, &data, sizeof(data));
}

void convertToNet(const PlayerAnim &data, void *outdata) {
    memcpy(outdata, &data, sizeof(data));
}

void convertToNet(const PlayerPos &data, void *outdata) {
    u8 *outid = (u8 *)outdata;
    memcpy(outid, &data.id, sizeof(data.id));
    outid += sizeof(data.id);
    i32 *outvec = (i32 *)outid;
    outvec[0] = htonl(data.pos.x);
    outvec[1] = htonl(data.pos.y);
}

/////////////////////////////////////////////////////////////////////

void convertToHost(const void *data, std::array<char, 8> &outdata) {
    memcpy(&outdata, data, sizeof(outdata));
}

void convertToHost(const void *data, vec2i &outdata) {
    i32 *in = (i32 *)data;
    outdata.x = ntohl(in[0]);
    outdata.y = ntohl(in[1]);
}

void convertToHost(const void *data, vec2b &outdata) {
    memcpy(&outdata, data, sizeof(outdata));
}
