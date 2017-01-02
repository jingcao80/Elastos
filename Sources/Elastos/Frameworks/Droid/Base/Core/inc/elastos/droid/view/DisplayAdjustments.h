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

#ifndef __ELASTOS_DROID_VIEW_DISPLAYADJUSTMENTS_H__
#define __ELASTOS_DROID_VIEW_DISPLAYADJUSTMENTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::Res::ICompatibilityInfo;

namespace Elastos {
namespace Droid {
namespace View {

class DisplayAdjustments
    : public Object
    , public IDisplayAdjustments
{
public:
    CAR_INTERFACE_DECL()

    DisplayAdjustments();

    DisplayAdjustments(
        /* [in] */ IBinder* token);

    DisplayAdjustments(
        /* [in] */ IDisplayAdjustments* daj);

    DisplayAdjustments(
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token);

    CARAPI SetCompatibilityInfo(
        /* [in] */ ICompatibilityInfo* compatInfo);

    CARAPI GetCompatibilityInfo(
        /* [out] */ ICompatibilityInfo** info);

    CARAPI SetActivityToken(
        /* [in] */ IBinder* token);

    CARAPI GetActivityToken(
        /* [out] */ IBinder** token);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* equals);

private:
    CARAPI Init(
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IBinder* token);
public:
    static CARAPI GetDEFAULT_DISPLAY_ADJUSTMENTS(
        /* [out] */ IDisplayAdjustments** da);

    static AutoPtr<IDisplayAdjustments> DEFAULT_DISPLAY_ADJUSTMENTS;

private:
    AutoPtr<ICompatibilityInfo> mCompatInfo;
    AutoPtr<IBinder> mActivityToken;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif