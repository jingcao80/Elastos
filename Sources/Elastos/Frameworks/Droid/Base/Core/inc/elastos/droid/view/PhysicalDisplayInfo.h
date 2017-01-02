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

#ifndef __ELASTOS_DROID_VIEW_PHYSICALDISPLAYINFO_H__
#define __ELASTOS_DROID_VIEW_PHYSICALDISPLAYINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace View {

class PhysicalDisplayInfo
    : public Object
    , public IPhysicalDisplayInfo
{
public:
    CAR_INTERFACE_DECL()

    PhysicalDisplayInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPhysicalDisplayInfo* other);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI GetRefreshRate(
        /* [out] */ Float* refreshRate);

    CARAPI GetDensity(
        /* [out] */ Float* density);

    CARAPI GetXDpi(
        /* [out] */ Float* xDpi);

    CARAPI GetYDpi(
        /* [out] */ Float* yDpi);

    CARAPI GetSecure(
        /* [out] */ Boolean* secure);

    CARAPI GetAppVsyncOffsetNanos(
        /* [out] */ Int64* nanos);

    CARAPI GetPresentationDeadlineNanos(
        /* [out] */ Int64* nanos);

    CARAPI SetWidth(
        /* [in] */ Int32 width);

    CARAPI SetHeight(
        /* [in] */ Int32 height);

    CARAPI SetRefreshRate(
        /* [in] */ Float refreshRate);

    CARAPI SetDensity(
        /* [in] */ Float density);

    CARAPI SetXDpi(
        /* [in] */ Float xDpi);

    CARAPI SetYDpi(
        /* [in] */ Float yDpi);

    CARAPI SetSecure(
        /* [in] */ Boolean secure);

    CARAPI SetAppVsyncOffsetNanos(
        /* [in] */ Int64 nanos);

    CARAPI SetPresentationDeadlineNanos(
        /* [in] */ Int64 nanos);

    CARAPI CopyFrom(
        /* [in] */ IPhysicalDisplayInfo* other);

private:
    Int32 mWidth;
    Int32 mHeight;
    Float mRefreshRate;
    Float mDensity;
    Float mXDpi;
    Float mYDpi;
    Boolean mSecure;
    Int64 mAppVsyncOffsetNanos;
    Int64 mPresentationDeadlineNanos;

};

} // view
} // droid
} // elastos
#endif