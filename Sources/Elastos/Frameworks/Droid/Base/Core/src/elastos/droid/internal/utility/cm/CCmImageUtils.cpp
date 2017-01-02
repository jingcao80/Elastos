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

#include "elastos/droid/internal/utility/cm/CCmImageUtils.h"
#include "elastos/droid/internal/utility/cm/CmImageUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CCmImageUtils, Singleton, ICmImageUtils)

CAR_SINGLETON_IMPL(CCmImageUtils)

ECode CCmImageUtils::GetImageDimension(
    /* [in] */ IInputStream* inputStream,
    /* [out] */ IPoint** result)
{
    return CmImageUtils::GetImageDimension(inputStream, result);
}

ECode CCmImageUtils::CropImage(
    /* [in] */ IInputStream* inputStream,
    /* [in] */ Int32 imageWidth,
    /* [in] */ Int32 imageHeight,
    /* [in] */ Int32 outWidth,
    /* [in] */ Int32 outHeight,
    /* [out] */ IInputStream** result)
{
    return CmImageUtils::CropImage(inputStream, imageWidth,
            imageHeight, outWidth, outHeight, result);
}

ECode CCmImageUtils::GetCroppedKeyguardStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    return CmImageUtils::GetCroppedKeyguardStream(pkgName, context, result);
}

ECode CCmImageUtils::GetCroppedWallpaperStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    return CmImageUtils::GetCroppedWallpaperStream(pkgName, context, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos
