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
#include "elastos/droid/server/display/LogicalDisplay.h"
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::View::IDisplay;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const Int32 LogicalDisplay::BLANK_LAYER_STACK;

LogicalDisplay::LogicalDisplay(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 layerStack,
    /* [in] */ DisplayDevice* primaryDisplayDevice)
    : mDisplayId(displayId)
    , mLayerStack(layerStack)
    , mPrimaryDisplayDevice(primaryDisplayDevice)
    , mHasContent(FALSE)
    , mRequestedRefreshRate(0)
{
    ASSERT_SUCCEEDED(CDisplayInfo::New((IDisplayInfo**)&mBaseDisplayInfo));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempLayerStackRect));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempDisplayRect));
}

/**
 * Gets the logical display id of this logical display.
 *
 * @return The logical display id.
 */
Int32 LogicalDisplay::GetDisplayIdLocked()
{
    return mDisplayId;
}

/**
 * Gets the primary display device associated with this logical display.
 *
 * @return The primary display device.
 */
AutoPtr<DisplayDevice> LogicalDisplay::GetPrimaryDisplayDeviceLocked()
{
    return mPrimaryDisplayDevice;
}

/**
 * Gets information about the logical display.
 *
 * @return The device info, which should be treated as immutable by the caller.
 * The logical display should allocate a new display info object whenever
 * the data changes.
 */
AutoPtr<IDisplayInfo> LogicalDisplay::GetDisplayInfoLocked()
{
    if (mInfo == NULL) {
        CDisplayInfo::New((IDisplayInfo**)&mInfo);
        if (mOverrideDisplayInfo != NULL) {
            mInfo->CopyFrom(mOverrideDisplayInfo);
            Int32 layerStack;
            mBaseDisplayInfo->GetLayerStack(&layerStack);
            mInfo->SetLayerStack(layerStack);
            String name;
            mBaseDisplayInfo->GetName(&name);
            mInfo->SetName(name);
            Int32 state;
            mBaseDisplayInfo->GetState(&state);
            mInfo->SetState(state);
        }
        else {
            mInfo->CopyFrom(mBaseDisplayInfo);
        }
    }
    return mInfo;
}

/**
 * Sets overridden logical display information from the window manager.
 * This method can be used to adjust application insets, rotation, and other
 * properties that the window manager takes care of.
 *
 * @param info The logical display information, may be NULL.
 */
Boolean LogicalDisplay::SetDisplayInfoOverrideFromWindowManagerLocked(
    /* [in] */ IDisplayInfo* info)
{
    if (info != NULL) {
        if (mOverrideDisplayInfo == NULL) {
            CDisplayInfo::New(info, (IDisplayInfo**)&mOverrideDisplayInfo);
            mInfo = NULL;
            return TRUE;
        }
        else if (!Object::Equals(mOverrideDisplayInfo, info)) {
            mOverrideDisplayInfo->CopyFrom(info);
            mInfo = NULL;
            return TRUE;
        }
    }
    else if (mOverrideDisplayInfo != NULL) {
        mOverrideDisplayInfo = NULL;
        mInfo = NULL;
        return TRUE;
    }
    return FALSE;
}

/**
 * Returns true if the logical display is in a valid state.
 * This method should be checked after calling {@link #updateLocked} to handle the
 * case where a logical display should be removed because all of its associated
 * display devices are gone or if it is otherwise no longer needed.
 *
 * @return True if the logical display is still valid.
 */
Boolean LogicalDisplay::IsValidLocked()
{
    return mPrimaryDisplayDevice != NULL;
}

/**
 * Updates the state of the logical display based on the available display devices.
 * The logical display might become invalid if it is attached to a display device
 * that no longer exists.
 *
 * @param devices The list of all connected display devices.
 */
void LogicalDisplay::UpdateLocked(
    /* [in] */ const List< AutoPtr<DisplayDevice> >& devices)
{
    // Nothing to update if already invalid.
    if (mPrimaryDisplayDevice == NULL) {
        return;
    }

    // Check whether logical display has become invalid.
    if (Find(devices.Begin(), devices.End(), mPrimaryDisplayDevice) == devices.End()) {
        mPrimaryDisplayDevice = NULL;
        return;
    }

    // Bootstrap the logical display using its associated primary physical display.
    // We might use more elaborate configurations later.  It's possible that the
    // configuration of several physical displays might be used to determine the
    // logical display that they are sharing.  (eg. Adjust size for pixel-perfect
    // mirroring over HDMI.)
    AutoPtr<DisplayDeviceInfo> deviceInfo = mPrimaryDisplayDevice->GetDisplayDeviceInfoLocked();
    if ((mPrimaryDisplayDeviceInfo == NULL && deviceInfo != NULL) || !mPrimaryDisplayDeviceInfo->Equals(deviceInfo)) {
        mBaseDisplayInfo->SetLayerStack(mLayerStack);
        mBaseDisplayInfo->SetFlags(0);
        if ((deviceInfo->mFlags & DisplayDeviceInfo::FLAG_SUPPORTS_PROTECTED_BUFFERS) != 0) {
            Int32 flags;
            mBaseDisplayInfo->GetFlags(&flags);
            flags |= IDisplay::FLAG_SUPPORTS_PROTECTED_BUFFERS;
            mBaseDisplayInfo->SetFlags(flags);
        }
        if ((deviceInfo->mFlags & DisplayDeviceInfo::FLAG_SECURE) != 0) {
            Int32 flags;
            mBaseDisplayInfo->GetFlags(&flags);
            flags |= IDisplay::FLAG_SECURE;
            mBaseDisplayInfo->SetFlags(flags);
        }
        if ((deviceInfo->mFlags & DisplayDeviceInfo::FLAG_PRIVATE) != 0) {
            Int32 flags;
            mBaseDisplayInfo->GetFlags(&flags);
            flags |= IDisplay::FLAG_PRIVATE;
            mBaseDisplayInfo->SetFlags(flags);
        }
        if ((deviceInfo->mFlags & DisplayDeviceInfo::FLAG_PRESENTATION) != 0) {
            Int32 flags;
            mBaseDisplayInfo->GetFlags(&flags);
            flags |= IDisplay::FLAG_PRESENTATION;
            mBaseDisplayInfo->SetFlags(flags);
        }
        mBaseDisplayInfo->SetType(deviceInfo->mType);
        mBaseDisplayInfo->SetAddress(deviceInfo->mAddress);
        mBaseDisplayInfo->SetName(deviceInfo->mName);
        mBaseDisplayInfo->SetAppWidth(deviceInfo->mWidth);
        mBaseDisplayInfo->SetAppHeight(deviceInfo->mHeight);
        mBaseDisplayInfo->SetLogicalWidth(deviceInfo->mWidth);
        mBaseDisplayInfo->SetLogicalHeight(deviceInfo->mHeight);
        mBaseDisplayInfo->SetRotation(ISurface::ROTATION_0);
        mBaseDisplayInfo->SetRefreshRate(deviceInfo->mRefreshRate);

        AutoPtr<ArrayOf<Float> > rates;
        Arrays::CopyOf(deviceInfo->mSupportedRefreshRates,
            deviceInfo->mSupportedRefreshRates->GetLength(), (ArrayOf<Float>**)&rates);
        mBaseDisplayInfo->SetSupportedRefreshRates(rates);

        mBaseDisplayInfo->SetLogicalDensityDpi(deviceInfo->mDensityDpi);
        mBaseDisplayInfo->SetPhysicalXDpi(deviceInfo->mXDpi);
        mBaseDisplayInfo->SetPhysicalYDpi(deviceInfo->mYDpi);
        mBaseDisplayInfo->SetAppVsyncOffsetNanos(deviceInfo->mAppVsyncOffsetNanos);
        mBaseDisplayInfo->SetPresentationDeadlineNanos(deviceInfo->mPresentationDeadlineNanos);
        mBaseDisplayInfo->SetState(deviceInfo->mState);
        mBaseDisplayInfo->SetSmallestNominalAppWidth(deviceInfo->mWidth);
        mBaseDisplayInfo->SetSmallestNominalAppHeight(deviceInfo->mHeight);
        mBaseDisplayInfo->SetLargestNominalAppWidth(deviceInfo->mWidth);
        mBaseDisplayInfo->SetLargestNominalAppHeight(deviceInfo->mHeight);
        mBaseDisplayInfo->SetOwnerUid(deviceInfo->mOwnerUid);
        mBaseDisplayInfo->SetOwnerPackageName(deviceInfo->mOwnerPackageName);

        mPrimaryDisplayDeviceInfo = deviceInfo;
        mInfo = NULL;
    }
}

/**
 * Applies the layer stack and transformation to the given display device
 * so that it shows the contents of this logical display.
 *
 * We know that the given display device is only ever showing the contents of
 * a single logical display, so this method is expected to blow away all of its
 * transformation properties to make it happen regardless of what the
 * display device was previously showing.
 *
 * The caller must have an open Surface transaction.
 *
 * The display device may not be the primary display device, in the case
 * where the display is being mirrored.
 *
 * @param device The display device to modify.
 * @param isBlanked True if the device is being blanked.
 */
void LogicalDisplay::ConfigureDisplayInTransactionLocked(
    /* [in] */ DisplayDevice* device,
    /* [in] */ Boolean isBlanked)
{
    AutoPtr<IDisplayInfo> displayInfo = GetDisplayInfoLocked();
    AutoPtr<DisplayDeviceInfo> displayDeviceInfo = device->GetDisplayDeviceInfoLocked();

    // Set the layer stack.
    device->SetLayerStackInTransactionLocked(isBlanked ? BLANK_LAYER_STACK : mLayerStack);

    // Set the refresh rate
    device->RequestRefreshRateLocked(mRequestedRefreshRate);

    // Set the viewport.
    // This is the area of the logical display that we intend to show on the
    // display device.  For now, it is always the full size of the logical display.
    Int32 logicalWidth, logicalHeight;
    displayInfo->GetLogicalWidth(&logicalWidth);
    displayInfo->GetLogicalHeight(&logicalHeight);
    mTempLayerStackRect->Set(0, 0, logicalWidth, logicalHeight);

    // Set the orientation.
    // The orientation specifies how the physical coordinate system of the display
    // is rotated when the contents of the logical display are rendered.
    Int32 orientation = ISurface::ROTATION_0;
    if ((displayDeviceInfo->mFlags & DisplayDeviceInfo::FLAG_ROTATES_WITH_CONTENT) != 0) {
        displayInfo->GetRotation(&orientation);
    }

    // Apply the physical rotation of the display device itself.
    orientation = (orientation + displayDeviceInfo->mRotation) % 4;

    // Set the frame.
    // The frame specifies the rotated physical coordinates into which the viewport
    // is mapped.  We need to take care to preserve the aspect ratio of the viewport.
    // Currently we maximize the area to fill the display, but we could try to be
    // more clever and match resolutions.
    Boolean rotated = (orientation == ISurface::ROTATION_90
        || orientation == ISurface::ROTATION_270);
    Int32 physWidth = rotated ? displayDeviceInfo->mHeight : displayDeviceInfo->mWidth;
    Int32 physHeight = rotated ? displayDeviceInfo->mWidth : displayDeviceInfo->mHeight;

    // Determine whether the width or height is more constrained to be scaled.
    //    physWidth / displayInfo->mLogicalWidth    => letter box
    // or physHeight / displayInfo->mLogicalHeight  => pillar box
    //
    // We avoid a division (and possible floating point imprecision) here by
    // multiplying the fractions by the product of their denominators before
    // comparing them.
    Int32 displayRectWidth, displayRectHeight;
    if (physWidth * logicalHeight < physHeight * logicalWidth) {
        // Letter box.
        displayRectWidth = physWidth;
        displayRectHeight = logicalHeight * physWidth / logicalWidth;
    }
    else {
        // Pillar box.
        displayRectWidth = logicalWidth * physHeight / logicalHeight;
        displayRectHeight = physHeight;
    }
    Int32 displayRectTop = (physHeight - displayRectHeight) / 2;
    Int32 displayRectLeft = (physWidth - displayRectWidth) / 2;
    mTempDisplayRect->Set(displayRectLeft, displayRectTop,
            displayRectLeft + displayRectWidth, displayRectTop + displayRectHeight);

    device->SetProjectionInTransactionLocked(orientation, mTempLayerStackRect, mTempDisplayRect);
}

/**
 * Returns true if the logical display has unique content.
 * <p>
 * If the display has unique content then we will try to ensure that it is
 * visible on at least its primary display device.  Otherwise we will ignore the
 * logical display and perhaps show mirrored content on the primary display device.
 * </p>
 *
 * @return True if the display has unique content.
 */
Boolean LogicalDisplay::HasContentLocked()
{
    return mHasContent;
}

/**
 * Sets whether the logical display has unique content.
 *
 * @param hasContent True if the display has unique content.
 */
void LogicalDisplay::SetHasContentLocked(
    /* [in] */ Boolean hasContent)
{
    mHasContent = hasContent;
}

/**
 * Requests the given refresh rate.
 * @param requestedRefreshRate The desired refresh rate.
 */
void LogicalDisplay::SetRequestedRefreshRateLocked(
    /* [in] */ Float requestedRefreshRate)
{
    mRequestedRefreshRate = requestedRefreshRate;
}

/**
 * Gets the pending requested refresh rate.
 *
 * @return The pending refresh rate requested
 */
Float LogicalDisplay::GetRequestedRefreshRateLocked()
{
    return mRequestedRefreshRate;
}

void LogicalDisplay::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{
//    pw->Println(String("mDisplayId=") + StringUtils::Int32ToString(mDisplayId));
//    pw->Println(String("mLayerStack=") + StringUtils::Int32ToString(mLayerStack));
//    pw->Println(String("mHasContent=") + StringUtils::BooleanToString(mHasContent));
//    pw->Println(String("mPrimaryDisplayDevice=") + (mPrimaryDisplayDevice != NULL ?
//        mPrimaryDisplayDevice->GetNameLocked() : "NULL"));
//    pw->Println(String("mBaseDisplayInfo=") + mBaseDisplayInfo->ToString());
//    pw->Println(String("mOverrideDisplayInfo=") + mOverrideDisplayInfo->ToString());
    assert(0);
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
