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

#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Utility.h>
#include "elastos/droid/server/display/DisplayDeviceInfo.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const Int32 DisplayDeviceInfo::FLAG_DEFAULT_DISPLAY;
const Int32 DisplayDeviceInfo::FLAG_ROTATES_WITH_CONTENT;
const Int32 DisplayDeviceInfo::FLAG_SECURE;
const Int32 DisplayDeviceInfo::FLAG_SUPPORTS_PROTECTED_BUFFERS;
const Int32 DisplayDeviceInfo::FLAG_PRIVATE;
const Int32 DisplayDeviceInfo::FLAG_NEVER_BLANK;
const Int32 DisplayDeviceInfo::FLAG_PRESENTATION;
const Int32 DisplayDeviceInfo::FLAG_OWN_CONTENT_ONLY;

const Int32 DisplayDeviceInfo::TOUCH_NONE;
const Int32 DisplayDeviceInfo::TOUCH_INTERNAL;
const Int32 DisplayDeviceInfo::TOUCH_EXTERNAL;

DisplayDeviceInfo::DisplayDeviceInfo()
    : mWidth(0)
    , mHeight(0)
    , mRefreshRate(0)
    , mDensityDpi(0)
    , mXDpi(0)
    , mYDpi(0)
    , mAppVsyncOffsetNanos(0)
    , mPresentationDeadlineNanos(0)
    , mFlags(0)
    , mTouch(0)
    , mRotation(ISurface::ROTATION_0)
    , mType(0)
    , mState(IDisplay::STATE_ON)
    , mOwnerUid(0)
{
    mSupportedRefreshRates = EmptyArray::FLOAT;
}

void DisplayDeviceInfo::SetAssumedDensityForExternalDisplay(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mDensityDpi = Elastos::Core::Math::Min(width, height) * IDisplayMetrics::DENSITY_XHIGH / 1080;
    // Technically, these values should be smaller than the apparent density
    // but we don't know the physical size of the display.
    mXDpi = mDensityDpi;
    mYDpi = mDensityDpi;
}

Boolean DisplayDeviceInfo::Equals(
    /* [in] */ DisplayDeviceInfo* other)
{
    return other != NULL
        && ((mName.IsNull() && other->mName.IsNull()) || mName.Equals(other->mName))
        && mWidth == other->mWidth
        && mHeight == other->mHeight
        && mRefreshRate == other->mRefreshRate
        && Arrays::Equals(mSupportedRefreshRates, other->mSupportedRefreshRates)
        && mDensityDpi == other->mDensityDpi
        && mXDpi == other->mXDpi
        && mYDpi == other->mYDpi
        && mAppVsyncOffsetNanos == other->mAppVsyncOffsetNanos
        && mPresentationDeadlineNanos == other->mPresentationDeadlineNanos
        && mFlags == other->mFlags
        && mTouch == other->mTouch
        && mRotation == other->mRotation
        && mType == other->mType
        && mAddress.Equals(other->mAddress)
        && mState == other->mState
        && mOwnerUid == other->mOwnerUid
        && mOwnerPackageName.Equals(other->mOwnerPackageName);
}

ECode DisplayDeviceInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    DisplayDeviceInfo* o = (DisplayDeviceInfo*)IObject::Probe(other);
    *result = Equals(o);
    return NOERROR;
}

//@Override
ECode DisplayDeviceInfo::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = 0; // don't care
    return NOERROR;
}

void DisplayDeviceInfo::CopyFrom(
    /* [in] */ DisplayDeviceInfo* other)
{
    mName = other->mName;
    mWidth = other->mWidth;
    mHeight = other->mHeight;
    mRefreshRate = other->mRefreshRate;
    mSupportedRefreshRates = other->mSupportedRefreshRates;
    mDensityDpi = other->mDensityDpi;
    mXDpi = other->mXDpi;
    mYDpi = other->mYDpi;
    mAppVsyncOffsetNanos = other->mAppVsyncOffsetNanos;
    mPresentationDeadlineNanos = other->mPresentationDeadlineNanos;
    mFlags = other->mFlags;
    mTouch = other->mTouch;
    mRotation = other->mRotation;
    mType = other->mType;
    mAddress = other->mAddress;
    mState = other->mState;
    mOwnerUid = other->mOwnerUid;
    mOwnerPackageName = other->mOwnerPackageName;
}

//@Override
ECode DisplayDeviceInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("DisplayDeviceInfo{\"");
    sb += mName;
    sb += "\": ";
    sb += mWidth;
    sb += " x ";
    sb += mHeight;
    sb += ", ";
    sb += mRefreshRate;
    sb += " fps, ";
    sb += "density ";
    sb += mDensityDpi;
    sb += ", ";
    sb += mXDpi;
    sb += " x ";
    sb += mYDpi;
    sb += " dpi, touch ";
    sb += TouchToString(mTouch);
    sb += FlagsToString(mFlags);
    sb += ", rotation ";
    sb += mRotation;
    sb += ", type ";
    sb += mType;//Display.typeToString(type)
    sb += ", address ";
    sb += mAddress;
    sb += "}";

    *str = sb.ToString();
    return NOERROR;
}

String DisplayDeviceInfo::TouchToString(
    /* [in] */ Int32 touch)
{
    switch (touch) {
        case TOUCH_NONE:
            return String("NONE");
        case TOUCH_INTERNAL:
            return String("INTERNAL");
        case TOUCH_EXTERNAL:
            return String("EXTERNAL");
        default:
            return StringUtils::ToString(touch);
    }
}

String DisplayDeviceInfo::FlagsToString(
    /* [in] */ Int32 flags)
{
    StringBuilder msg;
    if ((flags & FLAG_DEFAULT_DISPLAY) != 0) {
        msg += (", FLAG_DEFAULT_DISPLAY");
    }
    if ((flags & FLAG_ROTATES_WITH_CONTENT) != 0) {
        msg += (", FLAG_ROTATES_WITH_CONTENT");
    }
    if ((flags & FLAG_SECURE) != 0) {
        msg += (", FLAG_SECURE");
    }
    if ((flags & FLAG_SUPPORTS_PROTECTED_BUFFERS) != 0) {
        msg += (", FLAG_SUPPORTS_PROTECTED_BUFFERS");
    }
    if ((flags & FLAG_PRIVATE) != 0) {
        msg += (", FLAG_PRIVATE");
    }
    if ((flags & FLAG_NEVER_BLANK) != 0) {
        msg += (", FLAG_NEVER_BLANK");
    }
    if ((flags & FLAG_PRESENTATION) != 0) {
        msg += (", FLAG_PRESENTATION");
    }
    if ((flags & FLAG_OWN_CONTENT_ONLY) != 0) {
        msg += (", FLAG_OWN_CONTENT_ONLY");
    }
    return msg.ToString();
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
