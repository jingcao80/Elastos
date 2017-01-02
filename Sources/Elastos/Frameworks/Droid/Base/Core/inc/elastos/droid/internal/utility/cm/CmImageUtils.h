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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CMIMAGEUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CMIMAGEUTILS_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPoint;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

class CmImageUtils
{
public:
    /**
     * Gets the Width and Height of the image
     *
     * @param inputStream The input stream of the image
     *
     * @return A point structure that holds the Width and Height (x and y)
     */
    static CARAPI GetImageDimension(
        /* [in] */ IInputStream* inputStream,
        /* [out] */ IPoint** result);

    /**
     * Crops the input image and returns a new InputStream of the cropped area
     *
     * @param inputStream The input stream of the image
     * @param imageWidth Width of the input image
     * @param imageHeight Height of the input image
     * @param inputStream Desired Width
     * @param inputStream Desired Width
     *
     * @return a new InputStream of the cropped area
     */
    static CARAPI CropImage(
        /* [in] */ IInputStream* inputStream,
        /* [in] */ Int32 imageWidth,
        /* [in] */ Int32 imageHeight,
        /* [in] */ Int32 outWidth,
        /* [in] */ Int32 outHeight,
        /* [out] */ IInputStream** result);

    /**
     * Crops the lock screen image and returns a new InputStream of the cropped area
     *
     * @param pkgName Name of the theme package
     * @param context The context
     *
     * @return a new InputStream of the cropped image
     */
    static CARAPI GetCroppedKeyguardStream(
        /* [in] */ const String& pkgName,
        /* [in] */ IContext* context,
        /* [out] */ IInputStream** result);

    /**
     * Crops the wallpaper image and returns a new InputStream of the cropped area
     *
     * @param pkgName Name of the theme package
     * @param context The context
     *
     * @return a new InputStream of the cropped image
     */
    static CARAPI GetCroppedWallpaperStream(
        /* [in] */ const String& pkgName,
        /* [in] */ IContext* context,
        /* [out] */ IInputStream** result);

private:
    static CARAPI_(AutoPtr<IInputStream>) GetOriginalKeyguardStream(
        /* [in] */ const String& pkgName,
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IInputStream>) GetOriginalWallpaperStream(
        /* [in] */ const String& pkgName,
        /* [in] */ IContext* context);

private:
    const static String TAG;
    const static String ASSET_URI_PREFIX;
    const static Int32 DEFAULT_IMG_QUALITY;
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CMIMAGEUTILS_H__
