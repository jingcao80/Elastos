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

#include <Elastos.Droid.Content.h>
#include "elastos/droid/server/display/LocalDisplayAdapter.h"
#include <elastos/droid/R.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Arrays.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::View::CPhysicalDisplayInfo;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const String LocalDisplayAdapter::TAG("LocalDisplayAdapter");
const Int32 LocalDisplayAdapter::BUILT_IN_DISPLAY_IDS_TO_SCAN[] = {
    ISurfaceControl::BUILT_IN_DISPLAY_ID_MAIN,
    ISurfaceControl::BUILT_IN_DISPLAY_ID_HDMI,
};
const Int32 LocalDisplayAdapter::BUILT_IN_DISPLAY_IDS_COUNT = 2;


LocalDisplayAdapter::RequestDisplayStateRunnable::RequestDisplayStateRunnable(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 mode)
    : mToken(token)
    , mMode(mode)
{}

ECode LocalDisplayAdapter::RequestDisplayStateRunnable::Run()
{
    // Trace.traceBegin(Trace.TRACE_TAG_POWER, "requestDisplayState("
    //         + Display.stateToString(state) + ", id=" + displayId + ")");
    // try {

    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
    return surfaceControl->SetDisplayPowerMode(mToken, mMode);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_POWER);
    // }
}

//===================================================================================
// LocalDisplayAdapter::LocalDisplayDevice
//===================================================================================
LocalDisplayAdapter::LocalDisplayDevice::LocalDisplayDevice(
    /* [in] */ LocalDisplayAdapter* owner,
    /* [in] */ IBinder* displayToken,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ ArrayOf<IPhysicalDisplayInfo*>* physicalDisplayInfos,
    /* [in] */ Int32 activeDisplayInfo)
    : DisplayDevice(owner, displayToken)
    , mBuiltInDisplayId(builtInDisplayId)
    , mDefaultPhysicalDisplayInfo(0)
    , mHavePendingChanges(FALSE)
    , mState(IDisplay::STATE_UNKNOWN)
    , mLastRequestedRefreshRate(0)
    , mHost(owner)
{
    ASSERT_SUCCEEDED(CPhysicalDisplayInfo::New(
        (*physicalDisplayInfos)[activeDisplayInfo], (IPhysicalDisplayInfo**)&mPhys));
    mDefaultPhysicalDisplayInfo = activeDisplayInfo;
    UpdateSupportedRefreshRatesLocked(physicalDisplayInfos, mPhys);
}

Boolean LocalDisplayAdapter::LocalDisplayDevice::UpdatePhysicalDisplayInfoLocked(
    /* [in] */ ArrayOf<IPhysicalDisplayInfo*>* physicalDisplayInfos,
    /* [in] */ Int32 activeDisplayInfo)
{
    AutoPtr<IPhysicalDisplayInfo> newPhys = (*physicalDisplayInfos)[activeDisplayInfo];
    if (!Object::Equals(mPhys, newPhys)) {
        mPhys->CopyFrom(newPhys);
        UpdateSupportedRefreshRatesLocked(physicalDisplayInfos, mPhys);
        mHavePendingChanges = TRUE;
        return TRUE;
    }
    return FALSE;
}

void LocalDisplayAdapter::LocalDisplayDevice::ApplyPendingDisplayDeviceInfoChangesLocked()
{
    if (mHavePendingChanges) {
        mInfo = NULL;
        mHavePendingChanges = FALSE;
    }
}

AutoPtr<DisplayDeviceInfo> LocalDisplayAdapter::LocalDisplayDevice::GetDisplayDeviceInfoLocked()
{
    if (mInfo == NULL) {
        mInfo = new DisplayDeviceInfo();
        mPhys->GetWidth(&mInfo->mWidth);
        mPhys->GetHeight(&mInfo->mHeight);
        mPhys->GetRefreshRate(&mInfo->mRefreshRate);

        mInfo->mSupportedRefreshRates = mSupportedRefreshRates;
        mPhys->GetAppVsyncOffsetNanos(&mInfo->mAppVsyncOffsetNanos);
        mPhys->GetPresentationDeadlineNanos(&mInfo->mPresentationDeadlineNanos);
        mInfo->mState = mState;

        // Assume that all built-in displays that have secure output (eg. HDCP) also
        // support compositing from gralloc protected buffers.
        Boolean secure;
        if (mPhys->GetSecure(&secure), secure) {
            mInfo->mFlags = DisplayDeviceInfo::FLAG_SECURE
                | DisplayDeviceInfo::FLAG_SUPPORTS_PROTECTED_BUFFERS;
        }

        AutoPtr<IResources> resources;
        mHost->GetContext()->GetResources((IResources**)&resources);
        assert(resources != NULL);
        if (mBuiltInDisplayId == ISurfaceControl::BUILT_IN_DISPLAY_ID_MAIN) {
            resources->GetString(R::string::display_manager_built_in_display_name, &mInfo->mName);
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_DEFAULT_DISPLAY
                | DisplayDeviceInfo::FLAG_ROTATES_WITH_CONTENT;
            mInfo->mType = IDisplay::TYPE_BUILT_IN;
            Float density;
            mPhys->GetDensity(&density);
            mInfo->mDensityDpi = (Int32)(density * 160 + 0.5f);
            mPhys->GetXDpi(&mInfo->mXDpi);
            mPhys->GetYDpi(&mInfo->mYDpi);
            mInfo->mTouch = DisplayDeviceInfo::TOUCH_INTERNAL;
        }
        else {
            mInfo->mType = IDisplay::TYPE_HDMI;
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_PRESENTATION;
            resources->GetString(R::string::display_manager_hdmi_display_name, &mInfo->mName);
            mInfo->mTouch = DisplayDeviceInfo::TOUCH_EXTERNAL;
            Int32 width, height;
            mPhys->GetWidth(&width);
            mPhys->GetHeight(&height);
            mInfo->SetAssumedDensityForExternalDisplay(width, height);

            // For demonstration purposes, allow rotation of the external display.
            // In the future we might allow the user to configure this directly.
            AutoPtr<ISystemProperties> sysProp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
            String value;
            sysProp->Get(String("persist.demo.hdmirotation"), &value);
            if (value.Equals("portrait")) {
                mInfo->mRotation = ISurface::ROTATION_270;
            }


            // For demonstration purposes, allow rotation of the external display
            // to follow the built-in display.
            Boolean bval;
            sysProp->GetBoolean(String("persist.demo.hdmirotates"), FALSE, &bval);
            if (bval) {
                mInfo->mFlags |= DisplayDeviceInfo::FLAG_ROTATES_WITH_CONTENT;
            }
        }
    }
    return mInfo;
}

//@Override
AutoPtr<IRunnable> LocalDisplayAdapter::LocalDisplayDevice::RequestDisplayStateLocked(
    /* [in] */ Int32 state)
{
    if (mState != state) {
        AutoPtr<IBinder> token = GetDisplayTokenLocked();
        Int32 mode = GetPowerModeForState(state);
        mState = state;
        UpdateDeviceInfoLocked();

        // Defer actually setting the display power mode until we have exited
        // the critical section since it can take hundreds of milliseconds
        // to complete.
        AutoPtr<IRunnable> runnable = new RequestDisplayStateRunnable(token, mode);
        return runnable;
    }
    return NULL;
}

//@Override
void LocalDisplayAdapter::LocalDisplayDevice::RequestRefreshRateLocked(
    /* [in] */ Float refreshRate)
{
    if (mLastRequestedRefreshRate == refreshRate) {
        return;
    }

    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
    mLastRequestedRefreshRate = refreshRate;
    Boolean bval;
    if (refreshRate != 0) {
        Int32 N = mSupportedRefreshRates->GetLength();
        for (Int32 i = 0; i < N; i++) {
            if (refreshRate == (*mSupportedRefreshRates)[i]) {
                Int32 configIndex = (*mRefreshRateConfigIndices)[i];
                surfaceControl->SetActiveConfig(GetDisplayTokenLocked(), configIndex, &bval);
                return;
            }
        }
        Slogger::W(TAG, "Requested refresh rate %f is unsupported.", refreshRate);
    }
    surfaceControl->SetActiveConfig(GetDisplayTokenLocked(), mDefaultPhysicalDisplayInfo, &bval);
}

void LocalDisplayAdapter::LocalDisplayDevice::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{
//    DisplayDevice::DumpLocked(pw);
//    pw->Println(String("mBuiltInDisplayId=") + StringUtils::Int32ToString(mBuiltInDisplayId);
//    pw->Println(String("mPhys=") + mPhys->ToString());
//    pw->Println(String("mBlanked=") + StringUtils::BooleanToString(mBlanked);
    assert(0);
}

void LocalDisplayAdapter::LocalDisplayDevice::UpdateDeviceInfoLocked()
{
    mInfo = NULL;
    mHost->SendDisplayDeviceEventLocked(this, DISPLAY_DEVICE_EVENT_CHANGED);
}

void LocalDisplayAdapter::LocalDisplayDevice::UpdateSupportedRefreshRatesLocked(
    /* [in] */ ArrayOf<IPhysicalDisplayInfo*>* physicalDisplayInfos,
    /* [in] */ IPhysicalDisplayInfo* activePhys)
{
    Int32 N = physicalDisplayInfos->GetLength();
    Int32 idx = 0;
    mSupportedRefreshRates = ArrayOf<Float>::Alloc(N);
    mRefreshRateConfigIndices = ArrayOf<Int32>::Alloc(N);
    Int32 aw, ah, w, h;
    Float ad, ax, ay, d, x, y, r;

    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IPhysicalDisplayInfo> phys = (*physicalDisplayInfos)[i];
        activePhys->GetWidth(&aw);
        activePhys->GetHeight(&ah);
        activePhys->GetDensity(&ad);
        activePhys->GetXDpi(&ax);
        activePhys->GetYDpi(&ay);

        phys->GetWidth(&w);
        phys->GetHeight(&h);
        phys->GetDensity(&d);
        phys->GetXDpi(&x);
        phys->GetYDpi(&y);

        if (aw == w && ah == h && ad == d && ax == x && ay == y) {
            phys->GetRefreshRate(&r);
            mSupportedRefreshRates->Set(idx, r);
            mRefreshRateConfigIndices->Set(idx++, i);
        }
    }
    if (idx != N) {
        AutoPtr<ArrayOf<Float> > tmp = mSupportedRefreshRates;
        mSupportedRefreshRates = NULL;
        Arrays::CopyOfRange(tmp, 0, idx, (ArrayOf<Float>**)&mSupportedRefreshRates);

        AutoPtr<ArrayOf<Int32> > t = mRefreshRateConfigIndices;
        mRefreshRateConfigIndices = NULL;
        Arrays::CopyOfRange(t, 0, idx, (ArrayOf<Int32>**)&mRefreshRateConfigIndices);
    }
}

//===================================================================================
//          LocalDisplayAdapter::HotplugDisplayEventReceiver
//===================================================================================
LocalDisplayAdapter::HotplugDisplayEventReceiver::HotplugDisplayEventReceiver(
    /* [in] */ ILooper* looper,
    /* [in] */ LocalDisplayAdapter* owner)
    : DisplayEventReceiver(looper)
    , mHost(owner)
{
}

//@Override
ECode LocalDisplayAdapter::HotplugDisplayEventReceiver::OnHotplug(
    /* [in] */ Int64 timestampNanos,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ Boolean connected)
{
    Object* obj = mHost->GetSyncRoot();
    AutoLock lock(obj);
   if (connected) {
        mHost->TryConnectDisplayLocked(builtInDisplayId);
    }
    else {
        mHost->TryDisconnectDisplayLocked(builtInDisplayId);
    }
    return NOERROR;
}

//===================================================================================
//          LocalDisplayAdapter
//===================================================================================
// Called with SyncRoot lock held.
LocalDisplayAdapter::LocalDisplayAdapter(
    /* [in] */ Object* syncRoot,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IDisplayAdapterListener* listener)
    : DisplayAdapter(syncRoot, context, handler, listener, TAG)
    , mDevices(5)
{
}

LocalDisplayAdapter::~LocalDisplayAdapter()
{
    mDevices.Clear();
}

//@Override
void LocalDisplayAdapter::RegisterLocked()
{
    DisplayAdapter::RegisterLocked();

    AutoPtr<ILooper> looper;
    GetHandler()->GetLooper((ILooper**)&looper);
    mHotplugReceiver = new HotplugDisplayEventReceiver(looper, this);

    for (Int32 i = 0; i < BUILT_IN_DISPLAY_IDS_COUNT; ++i) {
        Int32 builtInDisplayId = BUILT_IN_DISPLAY_IDS_TO_SCAN[i];
        TryConnectDisplayLocked(builtInDisplayId);
    }
}

void LocalDisplayAdapter::TryConnectDisplayLocked(
    /* [in] */ Int32 builtInDisplayId)
{
    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);

    AutoPtr<IBinder> displayToken;
    surfaceControl->GetBuiltInDisplay(builtInDisplayId, (IBinder**)&displayToken);
    if (displayToken != NULL) {
        AutoPtr<ArrayOf<IPhysicalDisplayInfo*> > configs;
        surfaceControl->GetDisplayConfigs(displayToken, (ArrayOf<IPhysicalDisplayInfo*>**)&configs);
        if (configs == NULL) {
            // There are no valid configs for this device, so we can't use it
            Slogger::W(TAG, "No valid configs found for display device %d", builtInDisplayId);
            return;
        }
        Int32 activeConfig;
        surfaceControl->GetActiveConfig(displayToken, &activeConfig);
        if (activeConfig < 0) {
            // There is no active config, and for now we don't have the
            // policy to set one.
            Slogger::W(TAG, "No active config found for display device %d", builtInDisplayId);
            return;
        }

        AutoPtr<LocalDisplayDevice> device;
        HashMap<Int32, AutoPtr<LocalDisplayDevice> >::Iterator it = mDevices.Find(builtInDisplayId);
        if (it != mDevices.End()) {
            device = it->mSecond;
        }

        if (device == NULL) {
            // Display was added.
            device = new LocalDisplayDevice(this, displayToken, builtInDisplayId, configs, activeConfig);
            mDevices[builtInDisplayId] = device;
            SendDisplayDeviceEventLocked(device, DISPLAY_DEVICE_EVENT_ADDED);
        }
        else if (device->UpdatePhysicalDisplayInfoLocked(configs, activeConfig)) {
            // Display properties changed.
            SendDisplayDeviceEventLocked(device, DISPLAY_DEVICE_EVENT_CHANGED);
        }
    }
    else {
        // The display is no longer available. Ignore the attempt to add it.
        // If it was connected but has already been disconnected, we'll get a
        // disconnect event that will remove it from mDevices.
    }
}

void LocalDisplayAdapter::TryDisconnectDisplayLocked(
    /* [in] */ Int32 builtInDisplayId)
{
    HashMap<Int32, AutoPtr<LocalDisplayDevice> >::Iterator it = mDevices.Find(builtInDisplayId);
    if (it != mDevices.End()) {
        AutoPtr<LocalDisplayDevice> device = it->mSecond;
        // Display was removed.
        mDevices.Erase(it);
        SendDisplayDeviceEventLocked(device, DISPLAY_DEVICE_EVENT_REMOVED);
    }
}

Int32 LocalDisplayAdapter::GetPowerModeForState(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IDisplay::STATE_OFF:
            return ISurfaceControl::POWER_MODE_OFF;
        case IDisplay::STATE_DOZE:
            return ISurfaceControl::POWER_MODE_DOZE;
        case IDisplay::STATE_DOZE_SUSPEND:
            return ISurfaceControl::POWER_MODE_DOZE_SUSPEND;
    }
    return ISurfaceControl::POWER_MODE_NORMAL;
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
