//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __ELASTOS_DISPLAY_H__
#define __ELASTOS_DISPLAY_H__

#include <elasys_server.h>

// Color Format
// If more color formats are added, this enum should be modified.
// NOTE: The current value shouldn't be changed.
typedef enum
{
    COLOR_RGB565        = 0,
    COLOR_RGBT666       = 1,
    COLOR_RGBT666PACKED = 2,
    COLOR_RGB2          = 3,
    COLOR_RGB4          = 4, // Using palette
    COLOR_RGB8          = 5, // Using palette
    COLOR_RGBT555       = 6,
    COLOR_RGB24         = 7,
    COLOR_ARGB          = 8,
    COLOR_YCbCr422      = 100,
    COLOR_YCbCr420      = 101
} ColorFormat;

typedef struct
{
    PixelFormat Color;
    int Bpp; // Bits per pixel
    int Width;
    int Height;
} FrameBufferFormat;

typedef struct
{
    // Match FrameBufferFormat
    ColorFormat Color;
    int Bpp;   // Bits per pixel
    int Width;
    int Height;

    // Special information
    _ELASTOS Boolean Enable;   // TRUE: enable overlay2  FALSE: disable overlay2
    int PosX;
    int PosY;
    int Layer; // Reserved Now
    _ELASTOS UInt16 Flag;//0 : underlay ; 1 : overlay
    //int Layer;
    // RGB formats only use 1 channel
    // YCbCr(YUV) formats use all 3 channels
    void *pChannelFb[3];
    unsigned int ChannelSize[3];
} OverlayFormat;

struct BitmapBufInfo {
    Address address;
    Int32 width;
    Int32 height;
    Int32 pitch;
    PixelFormat pixelFormat;
};

typedef struct
{
    BitmapBufInfo srcBuffer;
    _ELASTOS Int32 startX;
    _ELASTOS Int32 startY;
} RenderFrame;

#endif //__ELASTOS_DISPLAY_H__
