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

#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERTASKTHUMBNAIL_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERTASKTHUMBNAIL_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CActivityManagerTaskThumbnail.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace App {

/** @hide */
CarClass(CActivityManagerTaskThumbnail)
    , public Object
    , public IActivityManagerTaskThumbnail
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerTaskThumbnail();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetMainThumbnail(
        /* [out] */ IBitmap** thumbnail);

    CARAPI SetMainThumbnail(
        /* [in] */ IBitmap* thumbnail);

    CARAPI GetThumbnailFileDescriptor(
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI SetThumbnailFileDescriptor(
        /* [in] */ IParcelFileDescriptor* pfd);

public:
    AutoPtr<IBitmap> mMainThumbnail;

    AutoPtr<IParcelFileDescriptor> mThumbnailFileDescriptor;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERTASKTHUMBNAIL_H__
