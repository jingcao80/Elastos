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

#ifndef __ELASTOS_DROID_APP_CCOMPOSEDICONINFO_H__
#define __ELASTOS_DROID_APP_CCOMPOSEDICONINFO_H__

#include "_Elastos_Droid_App_CComposedIconInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CComposedIconInfo)
    , public Object
    , public IComposedIconInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CComposedIconInfo();

    CARAPI constructor();

    CARAPI GetIconUpon(
        /* [out] */ Int32* iconUpon);

    CARAPI SetIconUpon(
        /* [in] */ Int32 iconUpon);

    CARAPI GetIconMask(
        /* [out] */ Int32* iconMask);

    CARAPI SetIconMask(
        /* [in] */ Int32 iconMask);

    CARAPI GetIconBacks(
        /* [out, callee] */ ArrayOf<Int32>** iconBacks);

    CARAPI SetIconBacks(
        /* [in] */ ArrayOf<Int32>* iconBacks);

    CARAPI GetIconScale(
        /* [out] */ Float* iconScale);

    CARAPI SetIconScale(
        /* [in] */ Float iconScale);

    CARAPI GetIconDensity(
        /* [out] */ Int32* iconDensity);

    CARAPI SetIconDensity(
        /* [in] */ Int32 iconDensity);

    CARAPI GetIconSize(
        /* [out] */ Int32* iconSize);

    CARAPI SetIconSize(
        /* [in] */ Int32 iconSize);

    CARAPI GetColorFilter(
        /* [out, callee] */ ArrayOf<Float>** colorFilter);

    CARAPI SetColorFilter(
        /* [in] */ ArrayOf<Float>* colorFilter);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    Int32 mIconUpon;
    Int32 mIconMask;
    AutoPtr< ArrayOf<Int32> > mIconBacks;
    Float mIconScale;
    Int32 mIconDensity;
    Int32 mIconSize;
    AutoPtr< ArrayOf<Float> > mColorFilter;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CCOMPOSEDICONINFO_H__
