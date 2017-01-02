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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/Display.h"
#include "elastos/droid/view/DisplayInfo.h"
#include "elastos/droid/content/res/CCompatibilityInfoHelper.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::View::Display;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Content::Res::ICompatibilityInfoHelper;
using Elastos::Droid::Content::Res::CCompatibilityInfoHelper;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL_2(DisplayInfo, Object, IDisplayInfo, IParcelable)

DisplayInfo::DisplayInfo()
    : mLayerStack(0)
    , mFlags(0)
    , mType(0)
    , mAppWidth(0)
    , mAppHeight(0)
    , mSmallestNominalAppWidth(0)
    , mSmallestNominalAppHeight(0)
    , mLargestNominalAppWidth(0)
    , mLargestNominalAppHeight(0)
    , mLogicalWidth(0)
    , mLogicalHeight(0)
    , mRotation(0)
    , mRefreshRate(0.0f)
    , mLogicalDensityDpi(0)
    , mPhysicalXDpi(0.0f)
    , mPhysicalYDpi(0.0f)
{
}

ECode DisplayInfo::constructor()
{
    return NOERROR;
}

ECode DisplayInfo::constructor(
    /* [in] */ IDisplayInfo* other)
{
    return CopyFrom(other);
}

ECode DisplayInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mLayerStack);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mType);
    source->ReadString(&mAddress);
    source->ReadString(&mName);
    source->ReadInt32(&mAppWidth);
    source->ReadInt32(&mAppHeight);
    source->ReadInt32(&mSmallestNominalAppWidth);
    source->ReadInt32(&mSmallestNominalAppHeight);
    source->ReadInt32(&mLargestNominalAppWidth);
    source->ReadInt32(&mLargestNominalAppHeight);
    source->ReadInt32(&mLogicalWidth);
    source->ReadInt32(&mLogicalHeight);
    source->ReadInt32(&mOverscanLeft);
    source->ReadInt32(&mOverscanTop);
    source->ReadInt32(&mOverscanRight);
    source->ReadInt32(&mOverscanBottom);
    source->ReadInt32(&mRotation);
    source->ReadFloat(&mRefreshRate);

    Int32 count;
    source->ReadInt32(&count);
    mSupportedRefreshRates = ArrayOf<Float>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        Float tmp;
        source->ReadFloat(&tmp);
        mSupportedRefreshRates->Set(i, tmp);
    };

    source->ReadInt32(&mLogicalDensityDpi);
    source->ReadFloat(&mPhysicalXDpi);
    source->ReadFloat(&mPhysicalYDpi);
    source->ReadInt64(&mAppVsyncOffsetNanos);
    source->ReadInt64(&mPresentationDeadlineNanos);
    source->ReadInt32(&mState);
    source->ReadInt32(&mOwnerUid);
    source->ReadString(&mOwnerPackageName);
    return NOERROR;
}

ECode DisplayInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mLayerStack);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mType);
    dest->WriteString(mAddress);
    dest->WriteString(mName);
    dest->WriteInt32(mAppWidth);
    dest->WriteInt32(mAppHeight);
    dest->WriteInt32(mSmallestNominalAppWidth);
    dest->WriteInt32(mSmallestNominalAppHeight);
    dest->WriteInt32(mLargestNominalAppWidth);
    dest->WriteInt32(mLargestNominalAppHeight);
    dest->WriteInt32(mLogicalWidth);
    dest->WriteInt32(mLogicalHeight);
    dest->WriteInt32(mOverscanLeft);
    dest->WriteInt32(mOverscanTop);
    dest->WriteInt32(mOverscanRight);
    dest->WriteInt32(mOverscanBottom);
    dest->WriteInt32(mRotation);
    dest->WriteFloat(mRefreshRate);

    Int32 count = mSupportedRefreshRates->GetLength();
    dest->WriteInt32(count);
    for (Int32 i = 0; i < count; i++) {
        dest->WriteFloat((*mSupportedRefreshRates)[i]);
    }

    dest->WriteInt32(mLogicalDensityDpi);
    dest->WriteFloat(mPhysicalXDpi);
    dest->WriteFloat(mPhysicalYDpi);
    dest->WriteInt64(mAppVsyncOffsetNanos);
    dest->WriteInt64(mPresentationDeadlineNanos);
    dest->WriteInt32(mState);
    dest->WriteInt32(mOwnerUid);
    dest->WriteString(mOwnerPackageName);
    return NOERROR;
}

ECode DisplayInfo::GetLayerStack(
    /* [out] */ Int32* layerStack)
{
    VALIDATE_NOT_NULL(layerStack);
    *layerStack = mLayerStack;
    return NOERROR;
}

ECode DisplayInfo::SetLayerStack(
    /* [in] */ Int32 layerStack)
{
    mLayerStack = layerStack;
    return NOERROR;
}

ECode DisplayInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode DisplayInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode DisplayInfo::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode DisplayInfo::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode DisplayInfo::GetAddress(
    /* [out] */ String* address)
{
    VALIDATE_NOT_NULL(address);
    *address = mAddress;
    return NOERROR;
}

ECode DisplayInfo::SetAddress(
    /* [in] */ const String& address)
{
    mAddress = address;
    return NOERROR;
}

ECode DisplayInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode DisplayInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode DisplayInfo::GetAppWidth(
    /* [out] */ Int32* appWidth)
{
    VALIDATE_NOT_NULL(appWidth);
    *appWidth = mAppWidth;
    return NOERROR;
}

ECode DisplayInfo::SetAppWidth(
    /* [in] */ Int32 appWidth)
{
    mAppWidth = appWidth;
    return NOERROR;
}

ECode DisplayInfo::GetAppHeight(
    /* [out] */ Int32* appHeight)
{
    VALIDATE_NOT_NULL(appHeight);
    *appHeight = mAppHeight;
    return NOERROR;
}

ECode DisplayInfo::SetAppHeight(
    /* [in] */ Int32 appHeight)
{
    mAppHeight = appHeight;
    return NOERROR;
}

ECode DisplayInfo::GetSmallestNominalAppWidth(
    /* [out] */ Int32* smallestNominalAppWidth)
{
    VALIDATE_NOT_NULL(smallestNominalAppWidth);
    *smallestNominalAppWidth = mSmallestNominalAppWidth;
    return NOERROR;
}

ECode DisplayInfo::SetSmallestNominalAppWidth(
    /* [in] */ Int32 smallestNominalAppWidth)
{
    mSmallestNominalAppWidth = smallestNominalAppWidth;
    return NOERROR;
}

ECode DisplayInfo::GetSmallestNominalAppHeight(
    /* [out] */ Int32* smallestNominalAppHeight)
{
    VALIDATE_NOT_NULL(smallestNominalAppHeight);
    *smallestNominalAppHeight = mSmallestNominalAppHeight;
    return NOERROR;
}

ECode DisplayInfo::SetSmallestNominalAppHeight(
    /* [in] */ Int32 smallestNominalAppHeight)
{
    mSmallestNominalAppHeight = smallestNominalAppHeight;
    return NOERROR;
}

ECode DisplayInfo::GetLargestNominalAppWidth(
    /* [out] */ Int32* largestNominalAppWidth)
{
    VALIDATE_NOT_NULL(largestNominalAppWidth);
    *largestNominalAppWidth = mLargestNominalAppWidth;
    return NOERROR;
}

ECode DisplayInfo::SetLargestNominalAppWidth(
    /* [in] */ Int32 largestNominalAppWidth)
{
    mLargestNominalAppWidth = largestNominalAppWidth;
    return NOERROR;
}

ECode DisplayInfo::GetLargestNominalAppHeight(
    /* [out] */ Int32* largestNominalAppHeight)
{
    VALIDATE_NOT_NULL(largestNominalAppHeight);
    *largestNominalAppHeight = mLargestNominalAppHeight;
    return NOERROR;
}

ECode DisplayInfo::SetLargestNominalAppHeight(
    /* [in] */ Int32 largestNominalAppHeight)
{
    mLargestNominalAppHeight = largestNominalAppHeight;
    return NOERROR;
}

ECode DisplayInfo::GetLogicalWidth(
    /* [out] */ Int32* logicalWidth)
{
    VALIDATE_NOT_NULL(logicalWidth);
    *logicalWidth = mLogicalWidth;
    return NOERROR;
}

ECode DisplayInfo::SetLogicalWidth(
    /* [in] */ Int32 logicalWidth)
{
    mLogicalWidth = logicalWidth;
    return NOERROR;
}

ECode DisplayInfo::GetLogicalHeight(
    /* [out] */ Int32* logicalHeight)
{
    VALIDATE_NOT_NULL(logicalHeight);
    *logicalHeight = mLogicalHeight;
    return NOERROR;
}

ECode DisplayInfo::SetLogicalHeight(
    /* [in] */ Int32 logicalHeight)
{
    mLogicalHeight = logicalHeight;
    return NOERROR;
}

ECode DisplayInfo::GetOverscanLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left)
    *left = mOverscanLeft;
    return NOERROR;
}

ECode DisplayInfo::GetOverscanTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top)
    *top = mOverscanTop;
    return NOERROR;
}

ECode DisplayInfo::GetOverscanRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right)
    *right = mOverscanRight;
    return NOERROR;
}

ECode DisplayInfo::GetOverscanBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom)
    *bottom = mOverscanBottom;
    return NOERROR;
}

ECode DisplayInfo::SetOverscanLeft(
    /* [in] */ Int32 left)
{
    mOverscanLeft = left;
    return NOERROR;
}

ECode DisplayInfo::SetOverscanTop(
    /* [in] */ Int32 top)
{
    mOverscanTop = top;
    return NOERROR;
}

ECode DisplayInfo::SetOverscanRight(
    /* [in] */ Int32 right)
{
    mOverscanRight = right;
    return NOERROR;
}

ECode DisplayInfo::SetOverscanBottom(
    /* [in] */ Int32 bottom)
{
    mOverscanBottom = bottom;
    return NOERROR;
}

ECode DisplayInfo::GetRotation(
    /* [out] */ Int32* rotation)
{
    VALIDATE_NOT_NULL(rotation);
    *rotation = mRotation;
    return NOERROR;
}

ECode DisplayInfo::SetRotation(
    /* [in] */ Int32 rotation)
{
    mRotation = rotation;
    return NOERROR;
}

ECode DisplayInfo::GetRefreshRate(
    /* [out] */ Float* refreshRate)
{
    VALIDATE_NOT_NULL(refreshRate);
    *refreshRate = mRefreshRate;
    return NOERROR;
}

ECode DisplayInfo::SetRefreshRate(
    /* [in] */ Float refreshRate)
{
    mRefreshRate = refreshRate;
    return NOERROR;
}

ECode DisplayInfo::GetSupportedRefreshRates(
    /* [out, callee] */ ArrayOf<Float>** rates)
{
    VALIDATE_NOT_NULL(rates)
    *rates = mSupportedRefreshRates;
    REFCOUNT_ADD(*rates)
    return NOERROR;
}

ECode DisplayInfo::SetSupportedRefreshRates(
    /* [in] */ ArrayOf<Float>* rates)
{
    mSupportedRefreshRates = rates;
    return NOERROR;
}

ECode DisplayInfo::GetLogicalDensityDpi(
    /* [out] */ Int32* logicalDensityDpi)
{
    VALIDATE_NOT_NULL(logicalDensityDpi);
    *logicalDensityDpi = mLogicalDensityDpi;
    return NOERROR;
}

ECode DisplayInfo::SetLogicalDensityDpi(
    /* [in] */ Int32 logicalDensityDpi)
{
    mLogicalDensityDpi = logicalDensityDpi;
    return NOERROR;
}

ECode DisplayInfo::GetPhysicalXDpi(
    /* [out] */ Float* physicalXDpi)
{
    VALIDATE_NOT_NULL(physicalXDpi);
    *physicalXDpi = mPhysicalXDpi;
    return NOERROR;
}

ECode DisplayInfo::SetPhysicalXDpi(
    /* [in] */ Float physicalXDpi)
{
    mPhysicalXDpi = physicalXDpi;
    return NOERROR;
}

ECode DisplayInfo::GetPhysicalYDpi(
    /* [out] */ Float* physicalYDpi)
{
    VALIDATE_NOT_NULL(physicalYDpi);
    *physicalYDpi = mPhysicalYDpi;
    return NOERROR;
}

ECode DisplayInfo::SetPhysicalYDpi(
    /* [in] */ Float physicalYDpi)
{
    mPhysicalYDpi = physicalYDpi;
    return NOERROR;
}

ECode DisplayInfo::GetAppVsyncOffsetNanos(
    /* [out] */ Int64* nanos)
{
    VALIDATE_NOT_NULL(nanos)
    *nanos = mAppVsyncOffsetNanos;
    return NOERROR;
}

ECode DisplayInfo::SetAppVsyncOffsetNanos(
    /* [in] */ Int64 nanos)
{
    mAppVsyncOffsetNanos = nanos;
    return NOERROR;
}

ECode DisplayInfo::GetPresentationDeadlineNanos(
    /* [out] */ Int64* nanos)
{
    VALIDATE_NOT_NULL(nanos)
    *nanos = mPresentationDeadlineNanos;
    return NOERROR;
}

ECode DisplayInfo::SetPresentationDeadlineNanos(
    /* [in] */ Int64 nanos)
{
    mPresentationDeadlineNanos = nanos;
    return NOERROR;
}

ECode DisplayInfo::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = mState;
    return NOERROR;
}

ECode DisplayInfo::SetState(
    /* [in] */ Int32 state)
{
    mState = state ;
    return NOERROR;
}

ECode DisplayInfo::GetOwnerUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mOwnerUid;
    return NOERROR;
}

ECode DisplayInfo::SetOwnerUid(
    /* [in] */ Int32 uid)
{
    mOwnerUid = uid ;
    return NOERROR;
}

ECode DisplayInfo::GetOwnerPackageName(
    /* [out] */ String* name)
{
     VALIDATE_NOT_NULL(name)
    *name = mOwnerPackageName;
    return NOERROR;
}

ECode DisplayInfo::SetOwnerPackageName(
    /* [in] */ const String& name)
{
    mOwnerPackageName = name;
    return NOERROR;
}

ECode DisplayInfo::Equals(
    /* [in] */ IDisplayInfo* otherObj,
    /* [out] */ Boolean* isEquals)
{
    VALIDATE_NOT_NULL(isEquals);
    DisplayInfo* other = (DisplayInfo*)otherObj;

    *isEquals = other != NULL
        && mLayerStack == other->mLayerStack
        && mFlags == other->mFlags
        && mType == other->mType
        && mAddress.Equals(other->mAddress)
        && mName.Equals(other->mName)
        && mAppWidth == other->mAppWidth
        && mAppHeight == other->mAppHeight
        && mSmallestNominalAppWidth == other->mSmallestNominalAppWidth
        && mSmallestNominalAppHeight == other->mSmallestNominalAppHeight
        && mLargestNominalAppWidth == other->mLargestNominalAppWidth
        && mLargestNominalAppHeight == other->mLargestNominalAppHeight
        && mLogicalWidth == other->mLogicalWidth
        && mLogicalHeight == other->mLogicalHeight
        && mOverscanLeft == other->mOverscanLeft
        && mOverscanTop == other->mOverscanTop
        && mOverscanRight == other->mOverscanRight
        && mOverscanBottom == other->mOverscanBottom
        && mRotation == other->mRotation
        && mRefreshRate == other->mRefreshRate
        && mLogicalDensityDpi == other->mLogicalDensityDpi
        && mPhysicalXDpi == other->mPhysicalXDpi
        && mPhysicalYDpi == other->mPhysicalYDpi
        && mAppVsyncOffsetNanos == other->mAppVsyncOffsetNanos
        && mPresentationDeadlineNanos == other->mPresentationDeadlineNanos
        && mState == other->mState
        && mOwnerUid == other->mOwnerUid
        && mOwnerPackageName.Equals(other->mOwnerPackageName);

    return NOERROR;
}

ECode DisplayInfo::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    return Equals(IDisplayInfo::Probe(other), result);
}

ECode DisplayInfo::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = 0; // don't care
    return NOERROR;
}

ECode DisplayInfo::GetNaturalWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    if (mRotation == ISurface::ROTATION_0 || mRotation == ISurface::ROTATION_180) {
        *width = mLogicalWidth;
    }
    else {
        *width = mLogicalHeight;
    }
    return NOERROR;
}

ECode DisplayInfo::GetNaturalHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    if (mRotation == ISurface::ROTATION_0 || mRotation == ISurface::ROTATION_180) {
        *height = mLogicalHeight;
    }
    else {
        *height = mLogicalWidth;
    }
    return NOERROR;
}

/**
 * Returns true if the specified UID has access to this display.
 */
ECode DisplayInfo::HasAccess(
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Display::HasAccess(uid, mFlags, mOwnerUid);
    return NOERROR;
}

ECode DisplayInfo::CopyFrom(
    /* [in] */ IDisplayInfo* otherObj)
{
    assert(otherObj);
    DisplayInfo* other = (DisplayInfo*)otherObj;
    mLayerStack = other->mLayerStack;
    mFlags = other->mFlags;
    mType = other->mType;
    mAddress = other->mAddress;
    mName = other->mName;
    mAppWidth = other->mAppWidth;
    mAppHeight = other->mAppHeight;
    mOverscanLeft = other->mOverscanLeft;
    mOverscanTop = other->mOverscanTop;
    mOverscanRight = other->mOverscanRight;
    mOverscanBottom = other->mOverscanBottom;
    mSmallestNominalAppWidth = other->mSmallestNominalAppWidth;
    mSmallestNominalAppHeight = other->mSmallestNominalAppHeight;
    mLargestNominalAppWidth = other->mLargestNominalAppWidth;
    mLargestNominalAppHeight = other->mLargestNominalAppHeight;
    mLogicalWidth = other->mLogicalWidth;
    mLogicalHeight = other->mLogicalHeight;
    mRotation = other->mRotation;
    mRefreshRate = other->mRefreshRate;
    Int32 length = other->mSupportedRefreshRates->GetLength();
    mSupportedRefreshRates = ArrayOf<Float>::Alloc(length);
    mSupportedRefreshRates->Copy(other->mSupportedRefreshRates, length);
    mLogicalDensityDpi = other->mLogicalDensityDpi;
    mPhysicalXDpi = other->mPhysicalXDpi;
    mPhysicalYDpi = other->mPhysicalYDpi;
    mAppVsyncOffsetNanos = other->mAppVsyncOffsetNanos;
    mPresentationDeadlineNanos = other->mPresentationDeadlineNanos;
    mState = other->mState;
    mOwnerUid = other->mOwnerUid;
    mOwnerPackageName = other->mOwnerPackageName;

    return NOERROR;
}

ECode DisplayInfo::GetAppMetrics(
    /* [in] */ IDisplayMetrics* outMetrics)
{
    AutoPtr<ICompatibilityInfoHelper> helper;
    CCompatibilityInfoHelper::AcquireSingleton((ICompatibilityInfoHelper**)&helper);
    AutoPtr<ICompatibilityInfo> info;
    helper->GetDefault((ICompatibilityInfo**)&info);

    return GetAppMetrics(outMetrics, info, NULL);
}

ECode DisplayInfo::GetAppMetrics(
    /* [in] */ IDisplayMetrics* outMetrics,
    /* [in] */ IDisplayAdjustments* displayAdjustments)
{
    AutoPtr<ICompatibilityInfo> info;
    displayAdjustments->GetCompatibilityInfo((ICompatibilityInfo**)&info);
    AutoPtr<IBinder> token;
    displayAdjustments->GetActivityToken((IBinder**)&token);
    return GetMetricsWithSize(outMetrics, info, token, mAppWidth, mAppHeight);
}

ECode DisplayInfo::GetAppMetrics(
    /* [in] */ IDisplayMetrics* outMetrics,
    /* [in] */ ICompatibilityInfo* ci,
    /* [in] */ IBinder* token)
{
    return GetMetricsWithSize(outMetrics, ci, token, mAppWidth, mAppHeight);
}

ECode DisplayInfo::GetLogicalMetrics(
    /* [in] */ IDisplayMetrics* outMetrics,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IBinder* token)
{
    return GetMetricsWithSize(outMetrics, compatInfo, token, mLogicalWidth, mLogicalHeight);
}

ECode DisplayInfo::GetMetricsWithSize(
    /* [in] */ IDisplayMetrics* outMetrics,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(outMetrics);

    outMetrics->SetDensityDpi(mLogicalDensityDpi);
    outMetrics->SetNoncompatDensityDpi(mLogicalDensityDpi);
    outMetrics->SetNoncompatWidthPixels(width);
    outMetrics->SetWidthPixels(width);
    outMetrics->SetNoncompatHeightPixels(height);
    outMetrics->SetHeightPixels(height);

    Float density = mLogicalDensityDpi * IDisplayMetrics::DENSITY_DEFAULT_SCALE;
    outMetrics->SetDensity(density);
    outMetrics->SetNoncompatDensity(density);
    outMetrics->GetDensity(&density);
    outMetrics->SetScaledDensity(density);
    outMetrics->SetNoncompatScaledDensity(density);

    outMetrics->SetXdpi(mPhysicalXDpi);
    outMetrics->SetNoncompatXdpi(mPhysicalXDpi);
    outMetrics->SetYdpi(mPhysicalXDpi);
    outMetrics->SetNoncompatYdpi(mPhysicalXDpi);

    if (compatInfo != NULL) {
        AutoPtr<ICompatibilityInfoHelper> helper;
        CCompatibilityInfoHelper::AcquireSingleton((ICompatibilityInfoHelper**)&helper);
        AutoPtr<ICompatibilityInfo> info;
        helper->GetDefault((ICompatibilityInfo**)&info);
        if (!Object::Equals(compatInfo, info)) {
            FAIL_RETURN(compatInfo->ApplyToDisplayMetrics(outMetrics));
        }
    }

    return NOERROR;
}

ECode DisplayInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("DisplayInfo");
    sb += "\"";
    sb += mName;
    sb += "\", app ";
    sb += mAppWidth;
    sb += " x ";
    sb += mAppHeight;
    sb += ", real ";
    sb += mLogicalWidth;
    sb += " x ";
    sb += mLogicalHeight;
    sb += ", largest app ";
    sb += mLargestNominalAppWidth;
    sb += " x ";
    sb += mLargestNominalAppHeight;
    sb += ", smallest app ";
    sb += mSmallestNominalAppWidth;
    sb += " x ";
    sb += mSmallestNominalAppHeight;
    sb += ", ";
    sb += mRefreshRate;
    sb += " fps";
    sb += ", rotation ";
    sb += mRotation;
    sb += ", density ";
    sb += mLogicalDensityDpi;
    sb += ", ";
    sb += mPhysicalXDpi;
    sb += " x ";
    sb += mPhysicalYDpi;
    sb += " dpi";
    sb += ", layerStack ";
    sb += mLayerStack;
    sb += ", type ";
    sb += Display::TypeToString(mType);
    sb += ", address ";
    sb += mAddress;
    sb += FlagsToString(mFlags);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

String DisplayInfo::FlagsToString(
    /* [in] */ Int32 flags)
{
    StringBuilder result;
    if ((flags & IDisplay::FLAG_SECURE) != 0) {
        result.Append(", FLAG_SECURE");
    }
    if ((flags & IDisplay::FLAG_SUPPORTS_PROTECTED_BUFFERS) != 0) {
        result.Append(", FLAG_SUPPORTS_PROTECTED_BUFFERS");
    }
    return result.ToString();
}

} // namespace View
} // namespace Droid
} // namespace Elastos
