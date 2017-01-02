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

#include "elastos/droid/media/CFaceDetector.h"
#include "elastos/droid/media/CFace.h"
#include <elastos/utility/logging/Slogger.h>

#include <utils/misc.h>
#include <utils/String8.h>
#include <utils/Log.h>

#include <skia/core/SkBitmap.h>

using namespace Elastos::Core;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {

/*static {
    sInitialized = FALSE;
    try {
        System.loadLibrary("FFTEm");
    } catch (UnsatisfiedLinkError e) {
        Log.d("FFTEm", "face detection library not found!");
    }
}*/

CAR_INTERFACE_IMPL(CFaceDetector, Object, IFaceDetector)

CAR_OBJECT_IMPL(CFaceDetector)

CFaceDetector::CFaceDetector()
    : mFD(0)
    , mSDK(0)
    , mDCR(0)
    , mWidth(0)
    , mHeight(0)
    , mMaxFaces(0)
{
}

CFaceDetector::~CFaceDetector()
{
    Finalize();
}

ECode CFaceDetector::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 maxFaces)
{
    Int32 status;
    FAIL_RETURN(fft_initialize(width, height, maxFaces, &status));
    mWidth = width;
    mHeight = height;
    mMaxFaces = maxFaces;
    mBWBuffer = ArrayOf<Byte>::Alloc(width * height);
    return NOERROR;
}

ECode CFaceDetector::FindFaces(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ArrayOf<IFace*>* faces,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = 0;
    VALIDATE_NOT_NULL(bitmap);

    Int32 w, h;
    bitmap->GetWidth(&w);
    bitmap->GetHeight(&h);

    if (w != mWidth ||h  != mHeight) {
        Slogger::E("CFaceDetector", "bitmap size doesn't match initialization");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (faces->GetLength() < mMaxFaces) {
        Slogger::E("CFaceDetector", "faces[] smaller than maxFaces");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 numFaces = fft_detect(bitmap);
    if (numFaces >= mMaxFaces)
        numFaces = mMaxFaces;

    for (Int32 i = 0 ; i < numFaces ; i++) {
        if ((*faces)[i] == NULL) {
            AutoPtr<IFace> face;
            CFace::New((IFace**)&face);
            faces->Set(i, face);
        }
        fft_get_face((*faces)[i], i);
    }

    *num = numFaces;
    return NOERROR;
}

/* no user serviceable parts here ... */
void CFaceDetector::Finalize()
{
    fft_destroy();
}

ECode CFaceDetector::fft_initialize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 maxFaces,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    // // load the configuration file
    // const char* root = getenv("ANDROID_ROOT");
    // android::String8 path(root);
    // path.appendPath("usr/share/bmd/RFFstd_501.bmd");
    // // path.appendPath("usr/share/bmd/RFFspeed_501.bmd");

    // const int MAX_FILE_SIZE = 65536;
    // void* initData = malloc( MAX_FILE_SIZE ); /* enough to fit entire file */
    // int filedesc = open(path.string(), O_RDONLY);
    // int initDataSize = read(filedesc, initData, MAX_FILE_SIZE);
    // close(filedesc);

    // // --------------------------------------------------------------------
    // btk_HSDK sdk = NULL;
    // btk_SDKCreateParam sdkParam = btk_SDK_defaultParam();
    // sdkParam.fpMalloc = malloc;
    // sdkParam.fpFree = free;
    // sdkParam.maxImageWidth = w;
    // sdkParam.maxImageHeight = h;

    // btk_Status status = btk_SDK_create(&sdkParam, &sdk);
    // // make sure everything went well
    // if (status != btk_STATUS_OK) {
    //     // XXX: be more precise about what went wrong
    //     return E_OUT_OF_MEMORY_ERROR;
    // }

    // btk_HDCR dcr = NULL;
    // btk_DCRCreateParam dcrParam = btk_DCR_defaultParam();
    // btk_DCR_create( sdk, &dcrParam, &dcr );

    // btk_HFaceFinder fd = NULL;
    // btk_FaceFinderCreateParam fdParam = btk_FaceFinder_defaultParam();
    // fdParam.pModuleParam = initData;
    // fdParam.moduleParamSize = initDataSize;
    // fdParam.maxDetectableFaces = maxFaces;
    // status = btk_FaceFinder_create( sdk, &fdParam, &fd );
    // btk_FaceFinder_setRange(fd, 20, w/2); /* set eye distance range */

    // // make sure everything went well
    // if (status != btk_STATUS_OK) {
    //     // XXX: be more precise about what went wrong
    //     return E_OUT_OF_MEMORY_ERROR;
    // }

    // // free the configuration file
    // free(initData);

    // mFD = fd;
    // mSDK = sdk;
    // mDCR = dcr;
    return NOERROR;
}

Int32  CFaceDetector::fft_detect(
    /* [in] */ IBitmap* bitmap)
{
    // get the fields we need
    // btk_HDCR hdcr = (btk_HDCR)(mDCR);
    // btk_HFaceFinder hfd = (btk_HFaceFinder)(mFD);
    // u32 maxFaces = mMaxFaces;
    // u32 width = mWidth;
    // u32 height = mHeight;

    // // get to the native bitmap
    // Handle64 nativeBitmatp = ((CBitmap*)bitmap)->mNativeBitmap;
    // SkBitmap const * nativeBitmap = (SkBitmap const *)nativeBitmatp;

    // // convert the image to B/W
    // uint8_t* dst = (uint8_t*)mBWBuffer.GetPayLoad();

    // // manage the life-time of locking our pixels
    // SkAutoLockPixels alp(*nativeBitmap);

    // uint16_t const* src = (uint16_t const*)nativeBitmap->getPixels();
    // int wpr = nativeBitmap->rowBytes() / 2;
    // for (u32 y = 0; y < height; y++) {
    //     for (u32 x = 0 ; x < width ; x++) {
    //         uint16_t rgb = src[x];
    //         int r  = rgb >> 11;
    //         int g2 = (rgb >> 5) & 0x3F;
    //         int b  = rgb & 0x1F;
    //         // L coefficients 0.299 0.587 0.11
    //         int L = (r<<1) + (g2<<1) + (g2>>1) + b;
    //         *dst++ = L;
    //     }
    //     src += wpr;
    // }

    // // run detection
    // btk_DCR_assignGrayByteImage(hdcr, bwbuffer, width, height);

    int numberOfFaces = 0;
    // if (btk_FaceFinder_putDCR(hfd, hdcr) == btk_STATUS_OK) {
    //     numberOfFaces = btk_FaceFinder_faces(hfd);
    // }
    // else {
    //     Slogger::E("CFaceDetector", "ERROR: Return 0 faces because error exists in btk_FaceFinder_putDCR.\n");
    // }

    return numberOfFaces;
}

void CFaceDetector::fft_get_face(
    /* [in] */ IFace* face,
    /* [in] */ Int32 i)
{
    if (face == NULL) {
        return;
    }

    CFace* faceObj = (CFace*)face;

    // btk_HDCR hdcr = (btk_HDCR)(mDCR);
    // btk_HFaceFinder hfd = (btk_HFaceFinder)(mFD);

    // FaceData faceData;
    // btk_FaceFinder_getDCR(hfd, hdcr);
    // getFaceData(hdcr, &faceData);

    // const float X2F = 1.0f / 65536.0f;
    // faceObj->mConfidence    = faceData.confidence;
    // faceObj->mMidpointX     = faceData.midpointx;
    // faceObj->mMidpointY     = faceData.midpointy;
    // faceObj->mEyedist       = faceData.eyedist;
    // faceObj->mEulerX = 0;
    // faceObj->mEulerY = 0;
    // faceObj->mEulerZ = 0;
}

void CFaceDetector::fft_destroy()
{
    // btk_HFaceFinder hfd = (btk_HFaceFinder)(mFD);
    // btk_FaceFinder_close( hfd );

    // btk_HDCR hdcr = (btk_HDCR)(mDCR);
    // btk_DCR_close( hdcr );

    // btk_HSDK hsdk = (btk_HSDK)(mSDK);
    // btk_SDK_close( hsdk );
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
