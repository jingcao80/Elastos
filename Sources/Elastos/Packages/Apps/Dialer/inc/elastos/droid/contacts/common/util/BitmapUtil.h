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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_BITMAPUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_BITMAPUTIL_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Provides static functions to decode bitmaps at the optimal size
 */
class BitmapUtil
{
public:
    /**
     * Returns Width or Height of the picture, depending on which size is smaller. Doesn't actually
     * decode the picture, so it is pretty efficient to run.
     */
    static CARAPI_(Int32) GetSmallerExtentFromBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

    /**
     * Finds the optimal sampleSize for loading the picture
     * @param originalSmallerExtent Width or height of the picture, whichever is smaller
     * @param targetExtent Width or height of the target view, whichever is bigger.
     *
     * If either one of the parameters is 0 or smaller, no sampling is applied
     */
    static CARAPI_(Int32) FindOptimalSampleSize(
        /* [in] */ Int32 originalSmallerExtent,
        /* [in] */ Int32 targetExtent);

    /**
     * Decodes the bitmap with the given sample size
     */
    static CARAPI_(AutoPtr<IBitmap>) DecodeBitmapFromBytes(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [in] */ Int32 sampleSize);

    /**
     * Retrieves a copy of the specified drawable resource, rotated by a specified angle.
     *
     * @param resources The current resources.
     * @param resourceId The resource ID of the drawable to rotate.
     * @param angle The angle of rotation.
     * @return Rotated drawable.
     */
    static CARAPI_(AutoPtr<IDrawable>) GetRotatedDrawable(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 resourceId,
        /* [in] */ Float angle);

private:
    BitmapUtil() {}

private:
    static const String TAG;
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_BITMAPUTIL_H__
