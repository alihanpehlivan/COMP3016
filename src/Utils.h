#pragma once

static unsigned int encodeRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return (r) | (g << 8) | (b << 16) | (a << 24);
}

static float* decodeRGBA(unsigned int encodedRGBA)
{
    static float ret[4];

    ret[3] = float((encodedRGBA & 0xFF000000) >> 24) / 255.0f;
    ret[2] = float((encodedRGBA & 0x00FF0000) >> 16) / 255.0f;
    ret[1] = float((encodedRGBA & 0x0000FF00) >> 8) / 255.0f;
    ret[0] = float(encodedRGBA & 0x000000FF) / 255.0f;

    return ret;
}
