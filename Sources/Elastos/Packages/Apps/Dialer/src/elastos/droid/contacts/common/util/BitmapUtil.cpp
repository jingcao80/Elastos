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

#include "elastos/droid/contacts/common/util/BitmapUtil.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

Int32 BitmapUtil::GetSmallerExtentFromBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    AutoPtr<IBitmapFactoryOptions> options;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);

    // don't actually decode the picture, just return its bounds
    options->SetInJustDecodeBounds(TRUE);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bm;
    factory->DecodeByteArray(bytes, 0, bytes->GetLength(), options, (IBitmap**)&bm);

    // test what the best sample size is
    Int32 outWidth, outHeight;
    options->GetOutWidth(&outWidth);
    options->GetOutHeight(&outHeight);
    return Elastos::Core::Math::Min(outWidth, outHeight);
}

Int32 BitmapUtil::FindOptimalSampleSize(
    /* [in] */ Int32 originalSmallerExtent,
    /* [in] */ Int32 targetExtent)
{
    // If we don't know sizes, we can't do sampling.
    if (targetExtent < 1) return 1;
    if (originalSmallerExtent < 1) return 1;

    // Test what the best sample size is. To do that, we find the sample size that gives us
    // the best trade-off between resulting image size and memory requirement. We allow
    // the down-sampled image to be 20% smaller than the target size. That way we can get around
    // unfortunate cases where e.g. a 720 picture is requested for 362 and not down-sampled at
    // all. Why 20%? Why not. Prove me wrong.
    Int32 extent = originalSmallerExtent;
    Int32 sampleSize = 1;
    while ((extent >> 1) >= targetExtent * 0.8f) {
        sampleSize <<= 1;
        extent >>= 1;
    }

    return sampleSize;
}

AutoPtr<IBitmap> BitmapUtil::DecodeBitmapFromBytes(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 sampleSize)
{
    AutoPtr<IBitmapFactoryOptions> options;
    if (sampleSize > 1) {
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&options);
        options->SetInSampleSize(sampleSize);
    }
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bm;
    factory->DecodeByteArray(bytes, 0, bytes->GetLength(), options, (IBitmap**)&bm);
    return bm;
}

AutoPtr<IDrawable> BitmapUtil::GetRotatedDrawable(
    /* [in] */ IResources* resources,
    /* [in] */ Int32 resourceId,
    /* [in] */ Float angle)
{
    // Get the original drawable and make a copy which will be rotated.
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> original;
    factory->DecodeResource(resources, resourceId, (IBitmap**)&original);
    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    Int32 width, height;
    original->GetWidth(&width);
    original->GetHeight(&height);
    AutoPtr<IBitmap> rotated;
    helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&rotated);

    // Perform the rotation.
    AutoPtr<ICanvas> tempCanvas;
    CCanvas::New(rotated, (ICanvas**)&tempCanvas);
    original->GetWidth(&width);
    original->GetHeight(&height);
    tempCanvas->Rotate(angle, width / 2, height / 2);
    tempCanvas->DrawBitmap(original, 0.0, 0.0, NULL);

    AutoPtr<IDrawable> drawable;
    CBitmapDrawable::New(resources, rotated, (IDrawable**)&drawable);
    return drawable;
}

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
