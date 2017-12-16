//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/YuvToJpegEncoder.h"
#include <skia/images/SkJPEGWriteUtility.h>
#include <hardware/hardware.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

YuvToJpegEncoder* YuvToJpegEncoder::Create(
    /* [in] */ Int32 format,
    /* [in] */ Int32* strides)
{
    //Only ImageFormat.NV21 and ImageFormat.YUY2 are supported
    //for now.
    if (format == HAL_PIXEL_FORMAT_YCrCb_420_SP) {
        return new Yuv420SpToJpegEncoder(strides);
    }
    else if (format == HAL_PIXEL_FORMAT_YCbCr_422_I) {
        return new Yuv422IToJpegEncoder(strides);
    }
    else {
        return NULL;
    }
}

YuvToJpegEncoder::YuvToJpegEncoder(
    /* [in] */ Int32* strides)
    : mStrides(strides)
{}

Boolean YuvToJpegEncoder::Encode(
    /* [in] */ SkWStream* stream,
    /* [in] */ void* inYuv,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32* offsets,
    /* [in] */ Int32 jpegQuality)
{
    jpeg_compress_struct    cinfo;
    skjpeg_error_mgr        sk_err;
    skjpeg_destination_mgr  sk_wstream(stream);

    cinfo.err = jpeg_std_error(&sk_err);
    sk_err.error_exit = skjpeg_error_exit;
    if (setjmp(sk_err.fJmpBuf)) {
        return FALSE;
    }
    jpeg_create_compress(&cinfo);

    cinfo.dest = &sk_wstream;

    SetJpegCompressStruct(&cinfo, width, height, jpegQuality);

    jpeg_start_compress(&cinfo, TRUE);

    Compress(&cinfo, (uint8_t*)inYuv, offsets);

    jpeg_finish_compress(&cinfo);

    return TRUE;
}

void YuvToJpegEncoder::SetJpegCompressStruct(
    /* [in] */ jpeg_compress_struct* cinfo,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 quality)
{
    cinfo->image_width = width;
    cinfo->image_height = height;
    cinfo->input_components = 3;
    cinfo->in_color_space = JCS_YCbCr;
    jpeg_set_defaults(cinfo);

    jpeg_set_quality(cinfo, quality, TRUE);
    jpeg_set_colorspace(cinfo, JCS_YCbCr);
    cinfo->raw_data_in = TRUE;
    cinfo->dct_method = JDCT_IFAST;
    ConfigSamplingFactors(cinfo);
}

///////////////////////////////////////////////////////////////////
Yuv420SpToJpegEncoder::Yuv420SpToJpegEncoder(
    /* [in] */ Int32* strides)
    : YuvToJpegEncoder(strides)
{
    mNumPlanes = 2;
}

void Yuv420SpToJpegEncoder::Compress(
    /* [in] */ jpeg_compress_struct* cinfo,
    /* [in] */ uint8_t* yuv,
    /* [in] */ Int32* offsets)
{
    SkDebugf("onFlyCompress");
    JSAMPROW y[16];
    JSAMPROW cb[8];
    JSAMPROW cr[8];
    JSAMPARRAY planes[3];
    planes[0] = y;
    planes[1] = cb;
    planes[2] = cr;

    Int32 width = cinfo->image_width;
    Int32 height = cinfo->image_height;
    uint8_t* yPlanar = yuv + offsets[0];
    uint8_t* vuPlanar = yuv + offsets[1]; //width * height;
    uint8_t* uRows = new uint8_t[8 * (width >> 1)];
    uint8_t* vRows = new uint8_t[8 * (width >> 1)];


    // process 16 lines of Y and 8 lines of U/V each time.
    while (cinfo->next_scanline < cinfo->image_height) {
        //deitnerleave u and v
        Deinterleave(vuPlanar, uRows, vRows, cinfo->next_scanline, width, height);

        for (Int32 i = 0; i < 16; i++) {
            // y row
            y[i] = yPlanar + (cinfo->next_scanline + i) * mStrides[0];

            // construct u row and v row
            if ((i & 1) == 0) {
                // height and width are both halved because of downsampling
                Int32 offset = (i >> 1) * (width >> 1);
                cb[i/2] = uRows + offset;
                cr[i/2] = vRows + offset;
            }
          }
        jpeg_write_raw_data(cinfo, planes, 16);
    }
    delete [] uRows;
    delete [] vRows;
}

void Yuv420SpToJpegEncoder::Deinterleave(
    /* [in] */ uint8_t* vuPlanar,
    /* [in] */ uint8_t* uRows,
    /* [in] */ uint8_t* vRows,
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 numRows = (height - rowIndex) / 2;
    if (numRows > 8) numRows = 8;
    for (Int32 row = 0; row < numRows; ++row) {
        Int32 offset = ((rowIndex >> 1) + row) * mStrides[1];
        uint8_t* vu = vuPlanar + offset;
        for (Int32 i = 0; i < (width >> 1); ++i) {
            Int32 index = row * (width >> 1) + i;
            uRows[index] = vu[1];
            vRows[index] = vu[0];
            vu += 2;
        }
    }
}

void Yuv420SpToJpegEncoder::ConfigSamplingFactors(
    /* [in] */ jpeg_compress_struct* cinfo)
{
    // cb and cr are horizontally downsampled and vertically downsampled as well.
    cinfo->comp_info[0].h_samp_factor = 2;
    cinfo->comp_info[0].v_samp_factor = 2;
    cinfo->comp_info[1].h_samp_factor = 1;
    cinfo->comp_info[1].v_samp_factor = 1;
    cinfo->comp_info[2].h_samp_factor = 1;
    cinfo->comp_info[2].v_samp_factor = 1;
}

///////////////////////////////////////////////////////////////////////////////
Yuv422IToJpegEncoder::Yuv422IToJpegEncoder(
    /* [in] */ Int32* strides)
    : YuvToJpegEncoder(strides)
{
    mNumPlanes = 1;
}

void Yuv422IToJpegEncoder::Compress(
    /* [in] */ jpeg_compress_struct* cinfo,
    /* [in] */ uint8_t* yuv,
    /* [in] */ Int32* offsets)
{
    SkDebugf("onFlyCompress_422");
    JSAMPROW y[16];
    JSAMPROW cb[16];
    JSAMPROW cr[16];
    JSAMPARRAY planes[3];
    planes[0] = y;
    planes[1] = cb;
    planes[2] = cr;

    Int32 width = cinfo->image_width;
    Int32 height = cinfo->image_height;
    uint8_t* yRows = new uint8_t [16 * width];
    uint8_t* uRows = new uint8_t [16 * (width >> 1)];
    uint8_t* vRows = new uint8_t [16 * (width >> 1)];

    uint8_t* yuvOffset = yuv + offsets[0];

    // process 16 lines of Y and 16 lines of U/V each time.
    while (cinfo->next_scanline < cinfo->image_height) {
        Deinterleave(yuvOffset, yRows, uRows, vRows, cinfo->next_scanline, width, height);

        for (Int32 i = 0; i < 16; i++) {
            // y row
            y[i] = yRows + i * width;

            // construct u row and v row
            // width is halved because of downsampling
            Int32 offset = i * (width >> 1);
            cb[i] = uRows + offset;
            cr[i] = vRows + offset;
        }

        jpeg_write_raw_data(cinfo, planes, 16);
    }
    delete [] yRows;
    delete [] uRows;
    delete [] vRows;
}

void Yuv422IToJpegEncoder::Deinterleave(
    /* [in] */ uint8_t* yuv,
    /* [in] */ uint8_t* yRows,
    /* [in] */ uint8_t* uRows,
    /* [in] */ uint8_t* vRows,
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 numRows = height - rowIndex;
    if (numRows > 16) numRows = 16;
    for (Int32 row = 0; row < numRows; ++row) {
        uint8_t* yuvSeg = yuv + (rowIndex + row) * mStrides[0];
        for (Int32 i = 0; i < (width >> 1); ++i) {
            Int32 indexY = row * width + (i << 1);
            Int32 indexU = row * (width >> 1) + i;
            yRows[indexY] = yuvSeg[0];
            yRows[indexY + 1] = yuvSeg[2];
            uRows[indexU] = yuvSeg[1];
            vRows[indexU] = yuvSeg[3];
            yuvSeg += 4;
        }
    }
}

void Yuv422IToJpegEncoder::ConfigSamplingFactors(
    /* [in] */ jpeg_compress_struct* cinfo)
{
    // cb and cr are horizontally downsampled and vertically downsampled as well.
    cinfo->comp_info[0].h_samp_factor = 2;
    cinfo->comp_info[0].v_samp_factor = 2;
    cinfo->comp_info[1].h_samp_factor = 1;
    cinfo->comp_info[1].v_samp_factor = 2;
    cinfo->comp_info[2].h_samp_factor = 1;
    cinfo->comp_info[2].v_samp_factor = 2;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
