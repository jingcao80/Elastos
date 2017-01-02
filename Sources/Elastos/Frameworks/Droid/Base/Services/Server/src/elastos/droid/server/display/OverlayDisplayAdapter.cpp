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
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/server/display/OverlayDisplayAdapter.h"
#include "elastos/droid/server/display/OverlayDisplayWindow.h"
#include "elastos/droid/server/display/DisplayDeviceInfo.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using  Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const String OverlayDisplayAdapter::TAG("OverlayDisplayAdapter");
const Boolean OverlayDisplayAdapter::DEBUG = TRUE;
const Int32 OverlayDisplayAdapter::MIN_WIDTH;
const Int32 OverlayDisplayAdapter::MIN_HEIGHT;
const Int32 OverlayDisplayAdapter::MAX_WIDTH;
const Int32 OverlayDisplayAdapter::MAX_HEIGHT;

AutoPtr<IPattern> InitPattern()
{
    AutoPtr<IPatternHelper> ph;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&ph);
    AutoPtr<IPattern> pattern;
    ph->Compile(String("(\\d+)x(\\d+)/(\\d+)(,[a-z]+)*"), (IPattern**)&pattern);

    return pattern;
}

const AutoPtr<IPattern> OverlayDisplayAdapter::SETTING_PATTERN = InitPattern();

//===================================================================================
//          OverlayDisplayAdapter::OverlayDisplayDevice
//===================================================================================
OverlayDisplayAdapter::OverlayDisplayDevice::OverlayDisplayDevice(
    /* [in] */ OverlayDisplayAdapter* owner,
    /* [in] */ IBinder* displayToken,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float refreshRate,
    /* [in] */ Int64 presentationDeadlineNanos,
    /* [in] */ Int32 densityDpi,
    /* [in] */ Boolean secure,
    /* [in] */ Int32 state,
    /* [in] */ ISurfaceTexture* surfaceTexture)
    : DisplayDevice(owner, displayToken)
    , mName(name)
    , mWidth(width)
    , mHeight(height)
    , mRefreshRate(refreshRate)
    , mDisplayPresentationDeadlineNanos(presentationDeadlineNanos)
    , mDensityDpi(densityDpi)
    , mSecure(secure)
    , mState(state)
    , mSurfaceTexture(surfaceTexture)
    , mHost(owner)
{
}

void OverlayDisplayAdapter::OverlayDisplayDevice::DestroyLocked()
{
    if (mSurfaceTexture != NULL) {
        mSurfaceTexture = NULL;
    }
    mHost->SendTraversalRequestLocked();

    mSurfaceTexture = NULL;
    if (mSurface != NULL) {
        mSurface->ReleaseResources();
        mSurface = NULL;
    }
    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
    surfaceControl->DestroyDisplay(GetDisplayTokenLocked());
}

void OverlayDisplayAdapter::OverlayDisplayDevice::PerformTraversalInTransactionLocked()
{
    if (mSurfaceTexture != NULL) {
        if (mSurface == NULL) {
            ASSERT_SUCCEEDED(CSurface::New(mSurfaceTexture, (ISurface**)&mSurface));
        }
        SetSurfaceInTransactionLocked(mSurface);
    }
}

void OverlayDisplayAdapter::OverlayDisplayDevice::SetStateLocked(
    /* [in] */ Int32 state)
{
    mState = state;
    mInfo = NULL;
}

AutoPtr<DisplayDeviceInfo> OverlayDisplayAdapter::OverlayDisplayDevice::GetDisplayDeviceInfoLocked()
{
    if (mInfo == NULL) {
        mInfo = new DisplayDeviceInfo();
        mInfo->mName = mName;
        mInfo->mWidth = mWidth;
        mInfo->mHeight = mHeight;
        mInfo->mRefreshRate = mRefreshRate;
        mInfo->mDensityDpi = mDensityDpi;
        mInfo->mXDpi = mDensityDpi;
        mInfo->mYDpi = mDensityDpi;

        mInfo->mPresentationDeadlineNanos = mDisplayPresentationDeadlineNanos +
            1000000000L / (Int32) mRefreshRate;   // display's deadline + 1 frame
        mInfo->mFlags = DisplayDeviceInfo::FLAG_PRESENTATION;
        if (mSecure) {
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_SECURE;
        }
        mInfo->mType = IDisplay::TYPE_OVERLAY;
        mInfo->mTouch = DisplayDeviceInfo::TOUCH_NONE;
        mInfo->mState = mState;
    }
    return mInfo;
}

//===================================================================================
//          OverlayDisplayAdapter::OverlayDisplayHandle::ShowRunnable
//===================================================================================
OverlayDisplayAdapter::OverlayDisplayHandle::ShowRunnable::ShowRunnable(
    /* [in] */ OverlayDisplayHandle* host)
    : mHost(host)
{}

ECode OverlayDisplayAdapter::OverlayDisplayHandle::ShowRunnable::Run()
{
    mHost->HandleShow();
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayAdapter::OverlayDisplayHandle::DismissRunnable
//===================================================================================
OverlayDisplayAdapter::OverlayDisplayHandle::DismissRunnable::DismissRunnable(
    /* [in] */ OverlayDisplayHandle* host)
    : mHost(host)
{}

ECode OverlayDisplayAdapter::OverlayDisplayHandle::DismissRunnable::Run()
{
    mHost->HandleDismiss();
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayAdapter::OverlayDisplayHandle
//===================================================================================
CAR_INTERFACE_IMPL(OverlayDisplayAdapter::OverlayDisplayHandle, Object, IOverlayDisplayWindowListener)

OverlayDisplayAdapter::OverlayDisplayHandle::OverlayDisplayHandle(
    /* [in] */ OverlayDisplayAdapter* owner,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ Int32 gravity,
    /* [in] */ Boolean secure)
    : mName(name)
    , mWidth(width)
    , mHeight(height)
    , mDensityDpi(densityDpi)
    , mGravity(gravity)
    , mSecure(secure)
    , mHost(owner)
{
    mShowRunnable = new ShowRunnable(this);
    mDismissRunnable = new DismissRunnable(this);

    Boolean result;
    mHost->mUiHandler->Post(mShowRunnable, &result);
}

void OverlayDisplayAdapter::OverlayDisplayHandle::DismissLocked()
{
    mHost->mUiHandler->RemoveCallbacks(mShowRunnable);
    Boolean result;
    mHost->mUiHandler->Post(mDismissRunnable, &result);
}

ECode OverlayDisplayAdapter::OverlayDisplayHandle::OnWindowCreated(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Float refreshRate,
    /* [in] */ Int64 presentationDeadlineNanos,
    /* [in] */ Int32 state)
{
    Slogger::D(TAG, "===================OverlayDisplayAdapter::OverlayDisplayHandle::OnWindowCreated()===========");
    Object* obj = mHost->GetSyncRoot();
    AutoLock lock(obj);

    AutoPtr<ISurfaceControlHelper> surfaceControl;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&surfaceControl);
    AutoPtr<IBinder> displayToken;
    surfaceControl->CreateDisplay(mName, mSecure, (IBinder**)&displayToken);
    mDevice = new OverlayDisplayDevice(mHost, displayToken, mName,
        mWidth, mHeight, refreshRate, presentationDeadlineNanos,
        mDensityDpi, mSecure, state, surfaceTexture);

    mHost->SendDisplayDeviceEventLocked(mDevice, DISPLAY_DEVICE_EVENT_ADDED);
    return NOERROR;
}

ECode OverlayDisplayAdapter::OverlayDisplayHandle::OnWindowDestroyed()
{
    Object* obj = mHost->GetSyncRoot();
    AutoLock lock(obj);
    if (mDevice != NULL) {
        mDevice->DestroyLocked();
        mHost->SendDisplayDeviceEventLocked(mDevice, DISPLAY_DEVICE_EVENT_REMOVED);
    }
    return NOERROR;
}

// Called on the UI thread.
// @Override
ECode OverlayDisplayAdapter::OverlayDisplayHandle::OnStateChanged(
    /* [in] */ Int32 state)
{
    Object* obj = mHost->GetSyncRoot();
    AutoLock lock(obj);

    if (mDevice != NULL) {
        mDevice->SetStateLocked(state);
        mHost->SendDisplayDeviceEventLocked(mDevice, DISPLAY_DEVICE_EVENT_CHANGED);
    }

    return NOERROR;
}

void OverlayDisplayAdapter::OverlayDisplayHandle::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("  ") + mName + ":");
    pw->Println(String("    mWidth=") + StringUtils::ToString(mWidth));
    pw->Println(String("    mHeight=") + StringUtils::ToString(mHeight));
    pw->Println(String("    mDensityDpi=") + StringUtils::ToString(mDensityDpi));
    pw->Println(String("    mGravity=") + StringUtils::ToString(mGravity));
    pw->Println(String("    mSecure=") + StringUtils::BooleanToString(mSecure));

    // // Try to dump the window state.
    // if (mWindow != NULL) {
    //     final IndentingPrintWriter ipw = new IndentingPrintWriter(pw, "    ");
    //     ipw.increaseIndent();
    //     DumpUtils.dumpAsync(mUiHandler, mWindow, ipw, 200);
    // }
}

void OverlayDisplayAdapter::OverlayDisplayHandle::HandleShow()
{
    AutoPtr<OverlayDisplayWindow> window = new OverlayDisplayWindow(
        mHost->GetContext(), mName, mWidth, mHeight, mDensityDpi,
        mGravity, mSecure, this);
    window->Show();

    Object* obj = mHost->GetSyncRoot();
    AutoLock lock(obj);
    mWindow = window;
}

void OverlayDisplayAdapter::OverlayDisplayHandle::HandleDismiss()
{
    AutoPtr<OverlayDisplayWindow> window;
    {
        Object* obj = mHost->GetSyncRoot();
        AutoLock lock(obj);
        window = mWindow;
        mWindow = NULL;
    }

    if (window != NULL) {
        window->Dismiss();
    }
}

//===================================================================================
//          OverlayDisplayAdapter::RegisterRunnable
//===================================================================================
OverlayDisplayAdapter::RegisterRunnable::RegisterRunnable(
    /* [in] */ OverlayDisplayAdapter* host)
    : mHost(host)
{}

ECode OverlayDisplayAdapter::RegisterRunnable::Run()
{
    AutoPtr<IContentResolver> cr;
    mHost->GetContext()->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::OVERLAY_DISPLAY_DEVICES, (IUri**)&uri);
    AutoPtr<OverlayDisplayContentObserver> ovdco = new OverlayDisplayContentObserver(mHost);
    ovdco->constructor(mHost->GetHandler());
    cr->RegisterContentObserver(uri, TRUE, (IContentObserver*)ovdco.Get());

    mHost->UpdateOverlayDisplayDevices();

    return NOERROR;
}

//===================================================================================
//          OverlayDisplayAdapter::OverlayDisplayContentObserver
//===================================================================================

OverlayDisplayAdapter::OverlayDisplayContentObserver::OverlayDisplayContentObserver(
    /* [in] */ OverlayDisplayAdapter* host)
    : mHost(host)
{}

ECode OverlayDisplayAdapter::OverlayDisplayContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->UpdateOverlayDisplayDevices();
    return NOERROR;
}

//===================================================================================
//          OverlayDisplayAdapter
//===================================================================================
OverlayDisplayAdapter::OverlayDisplayAdapter(
    /* [in] */ Object* syncRoot,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IDisplayAdapterListener* listener,
    /* [in] */ IHandler* uiHandler)
    : DisplayAdapter(syncRoot, context, handler, listener, String(TAG))
    , mCurrentOverlaySetting("")
{
    mUiHandler = uiHandler;
}

void OverlayDisplayAdapter::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{
    DisplayAdapter::DumpLocked(pw);

    pw->Println(String("mCurrentOverlaySetting=") + mCurrentOverlaySetting);
    pw->Println(String("mOverlays: size=") + StringUtils::ToString((Int32)mOverlays.GetSize()));

    List<AutoPtr<OverlayDisplayHandle> >::Iterator iter = mOverlays.Begin();
    for (; iter != mOverlays.End(); ++iter) {
        (*iter)->DumpLocked(pw);
    }
}

void OverlayDisplayAdapter::RegisterLocked()
{
    DisplayAdapter::RegisterLocked();

    AutoPtr<IRunnable> runnable = new RegisterRunnable(this);
    Boolean result;
    GetHandler()->Post(runnable, &result);
}

void OverlayDisplayAdapter::UpdateOverlayDisplayDevices()
{
    Object* obj = GetSyncRoot();
    AutoLock lock(obj);

    UpdateOverlayDisplayDevicesLocked();
}

void OverlayDisplayAdapter::UpdateOverlayDisplayDevicesLocked()
{
    AutoPtr<IContext> context = GetContext();

    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    String value;
    Settings::Global::GetString(cr, ISettingsGlobal::OVERLAY_DISPLAY_DEVICES, &value);
    if (value.IsNull()) {
        value = String("");
    }

    if (value.Equals(mCurrentOverlaySetting)) {
        return;
    }
    mCurrentOverlaySetting = value;

    if (!mOverlays.IsEmpty()) {
        Slogger::I(TAG, "Dismissing all overlay display devices.");

        List<AutoPtr<OverlayDisplayHandle> >::Iterator iter = mOverlays.Begin();
        for (; iter != mOverlays.End(); ++iter) {
            (*iter)->DismissLocked();
        }
        mOverlays.Clear();
    }



    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(value, String(";"), (ArrayOf<String> **)&parts);
    if (parts != NULL) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        assert(resources != NULL);

        String part;
        Int32 count = 0, width, height, densityDpi;
        Boolean matched;
        ECode ec = NOERROR;
        for (Int32 i = 0; i < parts->GetLength(); ++i) {
            part = (*parts)[i];

            AutoPtr<IMatcher> matcher;
            SETTING_PATTERN->Matcher(part, (IMatcher**)&matcher);
            matcher->Matches(&matched);
            if (matched) {
                if (count >= 4) {
                    Slogger::W(TAG, "Too many overlay display devices specified: %s", value.string());
                    break;
                }

                IMatchResult* mr = IMatchResult::Probe(matcher);
                String strGroup;
                mr->Group(1, &strGroup);
                ec = StringUtils::Parse(strGroup, 10, &width);
                if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) continue;
                mr->Group(2, &strGroup);
                ec = StringUtils::Parse(strGroup, 10, &height);
                if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) continue;
                mr->Group(3, &strGroup);
                ec = StringUtils::Parse(strGroup, 10, &densityDpi);
                if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) continue;

                String flagString;
                mr->Group(4, &flagString);
                if (width >= MIN_WIDTH && width <= MAX_WIDTH
                    && height >= MIN_HEIGHT && height <= MAX_HEIGHT
                    && densityDpi >= IDisplayMetrics::DENSITY_LOW
                    && densityDpi <= IDisplayMetrics::DENSITY_XXHIGH) {
                    Int32 number = ++count;

                    String name;
                    resources->GetString(R::string::display_manager_overlay_display_name, &name);
                    Int32 gravity = ChooseOverlayGravity(number);

                    Boolean secure = !flagString.IsNull() && flagString.Contains(",secure");

                    Slogger::I(TAG, "Showing overlay display device #%d: name=%s, width=%d, height=%d, densityDpi=%d, secure=%d",
                        number, name.string(), width, height, densityDpi, secure);

                    AutoPtr<OverlayDisplayHandle> handle = new OverlayDisplayHandle(this, name,
                        width, height, densityDpi, gravity, secure);
                    mOverlays.PushBack(handle);

                    continue;
                }
            }
            else if (part.IsEmpty()) {
                continue;
            }

            Slogger::W(TAG, "Malformed overlay display devices setting: %s", value.string());
        }
    }
}

Int32 OverlayDisplayAdapter::ChooseOverlayGravity(
    /* [in] */ Int32 overlayNumber)
{
    switch (overlayNumber) {
        case 1:
            return IGravity::TOP | IGravity::LEFT;
        case 2:
            return IGravity::BOTTOM | IGravity::RIGHT;
        case 3:
            return IGravity::TOP | IGravity::RIGHT;
        case 4:
        default:
            return IGravity::BOTTOM | IGravity::LEFT;
    }
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
