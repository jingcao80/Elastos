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
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/AutoDecoderCancel.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBitmapFactoryOptions);
CAR_INTERFACE_IMPL(CBitmapFactoryOptions, Object, IBitmapFactoryOptions);

CBitmapFactoryOptions::CBitmapFactoryOptions()
    : mInMutable(FALSE)
    , mInJustDecodeBounds(FALSE)
    , mInSampleSize(0)
    , mInPreferredConfig(BitmapConfig_ARGB_8888)
    , mInPremultiplied(TRUE)
    , mInDither(FALSE)
    , mInDensity(0)
    , mInTargetDensity(0)
    , mInScreenDensity(0)
    , mInScaled(TRUE)
    , mInPurgeable(FALSE)
    , mInInputShareable(FALSE)
    , mInPreferQualityOverSpeed(FALSE)
    , mOutWidth(0)
    , mOutHeight(0)
    , mCancel(FALSE)
{}

void CBitmapFactoryOptions::RequestCancel()
{
    AutoDecoderCancel::RequestCancel(this);
}

ECode CBitmapFactoryOptions::RequestCancelDecode()
{
    mCancel = TRUE;
    RequestCancel();
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInBitmap(
    /* [out] */ IBitmap** bm)
{
    VALIDATE_NOT_NULL(bm);
    *bm = mInBitmap;
    REFCOUNT_ADD(*bm);
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInBitmap(
    /* [in] */ IBitmap* bm)
{
    mInBitmap = bm;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInMutable(
    /* [out] */ Boolean* m)
{
    VALIDATE_NOT_NULL(m);
    *m = mInMutable;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInMutable(
    /* [in] */ Boolean m)
{
    mInMutable = m;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInJustDecodeBounds(
    /* [out] */ Boolean* justDecodeBounds)
{
    VALIDATE_NOT_NULL(justDecodeBounds);
    *justDecodeBounds = mInJustDecodeBounds;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInJustDecodeBounds(
    /* [in] */ Boolean justDecodeBounds)
{
    mInJustDecodeBounds = justDecodeBounds;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInSampleSize(
    /* [out] */ Int32* sampleSize)
{
    VALIDATE_NOT_NULL(sampleSize);
    *sampleSize = mInSampleSize;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInSampleSize(
    /* [in] */ Int32 sampleSize)
{
    mInSampleSize = sampleSize;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInPreferredConfig(
    /* [out] */ BitmapConfig* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mInPreferredConfig;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInPreferredConfig(
    /* [in] */ BitmapConfig config)
{
    mInPreferredConfig = config;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInDither(
    /* [out] */ Boolean* dither)
{
    VALIDATE_NOT_NULL(dither);
    *dither = mInDither;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInDither(
    /* [in] */ Boolean dither)
{
    mInDither = dither;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mInDensity;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInDensity(
    /* [in] */ Int32 density)
{
    mInDensity = density;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInTargetDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mInTargetDensity;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInTargetDensity(
    /* [in] */ Int32 density)
{
    mInTargetDensity = density;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInScreenDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mInScreenDensity;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInScreenDensity(
    /* [in] */ Int32 density)
{
    mInScreenDensity = density;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInScaled(
    /* [out] */ Boolean* scaled)
{
    VALIDATE_NOT_NULL(scaled);
    *scaled = mInScaled;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInScaled(
    /* [in] */ Boolean scaled)
{
    mInScaled = scaled;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInPremultiplied(
    /* [out] */ Boolean* premultiplied)
{
    VALIDATE_NOT_NULL(premultiplied);
    *premultiplied = mInPremultiplied;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInPremultiplied(
    /* [in] */ Boolean premultiplied)
{
    mInPremultiplied = premultiplied;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInPurgeable(
    /* [out] */ Boolean* purgeable)
{
    VALIDATE_NOT_NULL(purgeable);
    *purgeable = mInPurgeable;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInPurgeable(
    /* [in] */ Boolean purgeable)
{
    mInPurgeable = purgeable;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInInputShareable(
    /* [out] */ Boolean* shareable)
{
    VALIDATE_NOT_NULL(shareable);
    *shareable = mInInputShareable;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInInputShareable(
    /* [in] */ Boolean shareable)
{
    mInInputShareable = shareable;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInPreferQualityOverSpeed(
    /* [out] */ Boolean* preferQualityOverSpeed)
{
    VALIDATE_NOT_NULL(preferQualityOverSpeed);
    *preferQualityOverSpeed = mInPreferQualityOverSpeed;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInPreferQualityOverSpeed(
    /* [in] */ Boolean preferQualityOverSpeed)
{
    mInPreferQualityOverSpeed = preferQualityOverSpeed;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetOutWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mOutWidth;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetOutWidth(
    /* [in] */ Int32 width)
{
    mOutWidth = width;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetOutHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mOutHeight;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetOutHeight(
    /* [in] */ Int32 height)
{
    mOutHeight = height;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetOutMimeType(
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType);
    *mimeType = mOutMimeType;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetOutMimeType(
    /* [in] */ const String& mimeType)
{
    mOutMimeType = mimeType;
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetInTempStorage(
    /* [in] */ ArrayOf<Byte>* storage)
{
    mInTempStorage = storage;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetInTempStorage(
    /* [out, callee] */ ArrayOf<Byte>** storage)
{
    VALIDATE_NOT_NULL(storage);
    *storage = mInTempStorage;
    REFCOUNT_ADD(*storage);
    return NOERROR;
}

ECode CBitmapFactoryOptions::SetCancel(
    /* [in] */ Boolean cancel)
{
    mCancel = cancel;
    return NOERROR;
}

ECode CBitmapFactoryOptions::GetCancel(
    /* [out] */ Boolean* cancel)
{
    VALIDATE_NOT_NULL(cancel);
    *cancel = mCancel;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

