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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwSettings.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwSettings_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwResource.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Webkit::PluginState_ON;
using Elastos::Droid::Webkit::PluginState_OFF;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//            AwSettings::SetInitialPageScaleRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwSettings::SetInitialPageScaleRunnable, Object, IRunnable);

AwSettings::SetInitialPageScaleRunnable::SetInitialPageScaleRunnable(
    /* [in] */ AwSettings* owner)
    : mOwner(owner)
{
}

ECode AwSettings::SetInitialPageScaleRunnable::Run()
{
    if (mOwner->mNativeAwSettings != 0) {
        mOwner->NativeUpdateInitialPageScaleLocked(mOwner->mNativeAwSettings);
    }

    return NOERROR;
}

//===============================================================
//           AwSettings::SetSaveFormDataRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwSettings::SetSaveFormDataRunnable, Object, IRunnable);

AwSettings::SetSaveFormDataRunnable::SetSaveFormDataRunnable(
    /* [in] */ AwSettings* owner)
    : mOwner(owner)
{
}

ECode AwSettings::SetSaveFormDataRunnable::Run()
{
    if (mOwner->mNativeAwSettings != 0) {
        mOwner->NativeUpdateFormDataPreferencesLocked(mOwner->mNativeAwSettings);
    }

    return NOERROR;
}

//===============================================================
//            AwSettings::SetUserAgentStringRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwSettings::SetUserAgentStringRunnable, Object, IRunnable);

AwSettings::SetUserAgentStringRunnable::SetUserAgentStringRunnable(
    /* [in] */ AwSettings* owner)
    : mOwner(owner)
{
}

ECode AwSettings::SetUserAgentStringRunnable::Run()
{
    if (mOwner->mNativeAwSettings != 0) {
        mOwner->NativeUpdateUserAgentLocked(mOwner->mNativeAwSettings);
    }
    return NOERROR;
}

//===============================================================
//          AwSettings::SetLoadWithOverviewModeRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwSettings::SetLoadWithOverviewModeRunnable, Object, IRunnable);

AwSettings::SetLoadWithOverviewModeRunnable::SetLoadWithOverviewModeRunnable(
    /* [in] */ AwSettings* owner)
    : mOwner(owner)
{
}

ECode AwSettings::SetLoadWithOverviewModeRunnable::Run()
{
    if (mOwner->mNativeAwSettings != 0) {
        mOwner->UpdateWebkitPreferencesOnUiThreadLocked();
        mOwner->NativeResetScrollAndScaleState(mOwner->mNativeAwSettings);
    }
    return NOERROR;
}

//===============================================================
//       AwSettings::OnGestureZoomSupportChangedRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwSettings::OnGestureZoomSupportChangedRunnable, Object, IRunnable);

AwSettings::OnGestureZoomSupportChangedRunnable::OnGestureZoomSupportChangedRunnable(
    /* [in] */ AwSettings* owner,
    /* [in] */ Boolean supportsDoubleTapZoom,
    /* [in] */ Boolean supportsMultiTouchZoom)
    : mOwner(owner)
    , mSupportsDoubleTapZoom(supportsDoubleTapZoom)
    , mSupportsMultiTouchZoom(supportsMultiTouchZoom)
{
}

ECode AwSettings::OnGestureZoomSupportChangedRunnable::Run()
{
    AutoLock lock(mOwner->mAwSettingsLock);
    if (mOwner->mZoomChangeListener != NULL) {
        mOwner->mZoomChangeListener->OnGestureZoomSupportChanged(
                mSupportsDoubleTapZoom, mSupportsMultiTouchZoom);
    }

    return NOERROR;
}

//============================================================================
//           AwSettings::SetVideoOverlayForEmbeddedVideoEnabledRunnable
//============================================================================
CAR_INTERFACE_IMPL(AwSettings::SetVideoOverlayForEmbeddedVideoEnabledRunnable, Object, IRunnable);

AwSettings::SetVideoOverlayForEmbeddedVideoEnabledRunnable::SetVideoOverlayForEmbeddedVideoEnabledRunnable(
    /* [in] */ AwSettings* owner)
    : mOwner(owner)
{
}

ECode AwSettings::SetVideoOverlayForEmbeddedVideoEnabledRunnable::Run()
{
    if (mOwner->mNativeAwSettings != 0) {
        mOwner->NativeUpdateRendererPreferencesLocked(mOwner->mNativeAwSettings);
    }

    return NOERROR;
}

//===============================================================
//              AwSettings::LazyDefaultUserAgent
//===============================================================
String AwSettings::LazyDefaultUserAgent::sInstance;

//===============================================================
//             AwSettings::EventHandler::InnerHandler
//===============================================================

AwSettings::EventHandler::InnerHandler::InnerHandler(
    /* [in] */ EventHandler* owner,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mOwner(owner)
{
}

ECode AwSettings::EventHandler::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case RUN_RUNNABLE_BLOCKING:
            {
                AutoLock lock(&(mOwner->mOwner->mAwSettingsLock));
                if (mOwner->mOwner->mNativeAwSettings != 0) {
                    AutoPtr<IInterface> runnableObj;
                    msg->GetObj((IInterface**)&runnableObj);
                    IRunnable* runnable = IRunnable::Probe(runnableObj);
                    runnable->Run();
                }
                mOwner->mSynchronizationPending = FALSE;
                mOwner->mOwner->mAwSettingsLock.NotifyAll();
            }
            break;
    }

    return NOERROR;
}

//===============================================================
//            AwSettings::EventHandler::InnerRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwSettings::EventHandler::InnerRunnable, Object, IRunnable);

AwSettings::EventHandler::InnerRunnable::InnerRunnable(
    /* [in] */ EventHandler* owner)
    : mOwner(owner)
{
}

ECode AwSettings::EventHandler::InnerRunnable::Run()
{
    mOwner->mOwner->UpdateWebkitPreferencesOnUiThreadLocked();
    return NOERROR;
}

//===============================================================
//                   AwSettings::EventHandler
//===============================================================

const Int32 AwSettings::EventHandler::RUN_RUNNABLE_BLOCKING;

AwSettings::EventHandler::EventHandler(
    /* [in] */ AwSettings* owner)
    : mOwner(owner)
    , mSynchronizationPending(FALSE)
{
}

void AwSettings::EventHandler::BindUiThread()
{
    if (mHandler != NULL) return;

    AutoPtr<ILooper> looper = ThreadUtils::GetUiThreadLooper();
    mHandler = new InnerHandler(this, looper);
}

void AwSettings::EventHandler::RunOnUiThreadBlockingAndLocked(
    /* [in] */ IRunnable* r)
{
    AutoLock lock(&(mOwner->mAwSettingsLock));

    if (mHandler == NULL) return;

    if (ThreadUtils::RunningOnUiThread()) {
        r->Run();
    }
    else {
        assert(!mSynchronizationPending);
        mSynchronizationPending = TRUE;
        AutoPtr<IMessageHelper> msgHelper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&msgHelper);
        AutoPtr<IMessage> msg;
        msgHelper->Obtain(NULL, RUN_RUNNABLE_BLOCKING, r, (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessage(msg, &result);
        // try {
            while (mSynchronizationPending) {
                ECode ecode = mOwner->mAwSettingsLock.Wait();
                if (FAILED(ecode))
                {
                    if (ecode == (ECode)E_INTERRUPTED_EXCEPTION)
                        mSynchronizationPending = false;
                    break;
                }
            }
        // } catch (InterruptedException e) {
        //     Log.e(TAG, "Interrupted waiting a Runnable to complete", e);
        //     mSynchronizationPending = false;
        // }
    }
}

void AwSettings::EventHandler::MaybePostOnUiThread(
    /* [in] */ IRunnable* r)
{
    if (mHandler != NULL) {
        Boolean res;
        mHandler->Post(r, &res);
    }
}

void AwSettings::EventHandler::UpdateWebkitPreferencesLocked()
{
    AutoPtr<IRunnable> runnable = new InnerRunnable(this);
    RunOnUiThreadBlockingAndLocked(runnable);
}

//===============================================================
//                         AwSettings
//===============================================================

const Int32 AwSettings::MIXED_CONTENT_ALWAYS_ALLOW;
const Int32 AwSettings::MIXED_CONTENT_NEVER_ALLOW;
const Int32 AwSettings::MIXED_CONTENT_COMPATIBILITY_MODE;
const String AwSettings::TAG("AwSettings");
Object AwSettings::sGlobalContentSettingsLock;

Boolean AwSettings::sAppCachePathIsSet = FALSE;
const Int32 AwSettings::MINIMUM_FONT_SIZE;
const Int32 AwSettings::MAXIMUM_FONT_SIZE;

AwSettings::AwSettings(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isAccessFromFileURLsGrantedByDefault,
    /* [in] */ Boolean supportsLegacyQuirks)
    : mDIPScale(1.0)
    , mLayoutAlgorithm(NARROW_COLUMNS)
    , mTextSizePercent(100)
    , mStandardFontFamily("sans-serif")
    , mFixedFontFamily("monospace")
    , mSansSerifFontFamily("sans-serif")
    , mSerifFontFamily("serif")
    , mCursiveFontFamily("cursive")
    , mFantasyFontFamily("fantasy")
    , mMinimumFontSize(8)
    , mMinimumLogicalFontSize(8)
    , mDefaultFontSize(16)
    , mDefaultFixedFontSize(13)
    , mLoadsImagesAutomatically(TRUE)
    , mImagesEnabled(TRUE)
    , mJavaScriptEnabled(FALSE)
    , mAllowUniversalAccessFromFileURLs(FALSE)
    , mAllowFileAccessFromFileURLs(FALSE)
    , mJavaScriptCanOpenWindowsAutomatically(FALSE)
    , mSupportMultipleWindows(FALSE)
    , mPluginState(PluginState_OFF)
    , mAppCacheEnabled(FALSE)
    , mDomStorageEnabled(FALSE)
    , mDatabaseEnabled(FALSE)
    , mUseWideViewport(FALSE)
    , mZeroLayoutHeightDisablesViewportQuirk(FALSE)
    , mForceZeroLayoutHeight(FALSE)
    , mLoadWithOverviewMode(FALSE)
    , mMediaPlaybackRequiresUserGesture(TRUE)
    , mInitialPageScalePercent(0)
    , mSpatialNavigationEnabled(FALSE)
    , mEnableSupportedHardwareAcceleratedFeatures(FALSE)
    , mMixedContentMode(MIXED_CONTENT_NEVER_ALLOW)
    , mVideoOverlayForEmbeddedVideoEnabled(FALSE)
    , mAcceptThirdPartyCookies(FALSE)
    , mBlockNetworkLoads(FALSE)
    , mAllowContentUrlAccess(TRUE)
    , mAllowFileUrlAccess(TRUE)
    , mCacheMode(IWebSettings::LOAD_DEFAULT)
    , mShouldFocusFirstNode(TRUE)
    , mGeolocationEnabled(TRUE)
    , mAutoCompleteEnabled(TRUE)
    , mFullscreenSupported(FALSE)
    , mSupportZoom(TRUE)
    , mBuiltInZoomControls(FALSE)
    , mDisplayZoomControls(TRUE)
    , mNativeAwSettings(0)
{
    Int32 permission;
    context->CheckPermission(
                Elastos::Droid::Manifest::permission::INTERNET,
                Process::MyPid(),
                Process::MyUid(), &permission);
    Boolean hasInternetPermission = (permission == IPackageManager::PERMISSION_GRANTED);

    {
        AutoLock lock(&mAwSettingsLock);
        mHasInternetPermission = hasInternetPermission;
        mBlockNetworkLoads = !hasInternetPermission;
        mEventHandler = new EventHandler(this);
        if (isAccessFromFileURLsGrantedByDefault) {
            mAllowUniversalAccessFromFileURLs = TRUE;
            mAllowFileAccessFromFileURLs = TRUE;
        }

        mDefaultTextEncoding = AwResource::GetDefaultTextEncoding();
        mUserAgent = GetDefaultUserAgent();

        // Best-guess a sensible initial value based on the features supported on the device.
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        pm->HasSystemFeature(IPackageManager::FEATURE_TOUCHSCREEN, &mSpatialNavigationEnabled);

        // Respect the system setting for password echoing.
        AutoPtr<IContentResolver> contentResolver;
        context->GetContentResolver((IContentResolver**)&contentResolver);
        AutoPtr<ISettingsSystem> settingsSystem;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
        Int32 value;
        settingsSystem->GetInt32(contentResolver, ISettingsSystem::TEXT_SHOW_PASSWORD, 1, &value);
        mPasswordEchoEnabled = (value == 1);

        // By default, scale the text size by the system font scale factor. Embedders
        // may override this by invoking setTextZoom().
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> configuration;
        resources->GetConfiguration((IConfiguration**)&configuration);
        Float fontScale;
        configuration->GetFontScale(&fontScale);
        mTextSizePercent *= fontScale;

        mSupportLegacyQuirks = supportsLegacyQuirks;
    }
    // Defer initializing the native side until a native WebContents instance is set.
}

//@CalledByNative
void AwSettings::NativeAwSettingsGone(
    /* [in] */ HANDLE nativeAwSettings)
{
    assert(mNativeAwSettings != 0 && mNativeAwSettings == nativeAwSettings);
    mNativeAwSettings = 0;
}

//@CalledByNative
Double AwSettings::GetDIPScaleLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDIPScale;
}

void AwSettings::SetDIPScale(
    /* [in] */ Double dipScale)
{
    AutoLock lock(&mAwSettingsLock);
    mDIPScale = dipScale;
    // TODO(joth): This should also be synced over to native side, but right now
    // the setDIPScale call is always followed by a setWebContents() which covers this.
}

void AwSettings::SetZoomListener(
    /* [in] */ ZoomSupportChangeListener* zoomChangeListener)
{
    AutoLock lock(&mAwSettingsLock);
    mZoomChangeListener = zoomChangeListener;
}

void AwSettings::SetWebContents(
    /* [in] */ HANDLE nativeWebContents)
{
    AutoLock lock(&mAwSettingsLock);
    if (mNativeAwSettings != 0) {
        NativeDestroy(mNativeAwSettings);
        assert(mNativeAwSettings == 0);  // nativeAwSettingsGone should have been called.
    }
    if (nativeWebContents != 0) {
        mEventHandler->BindUiThread();
        mNativeAwSettings = NativeInit(nativeWebContents);
        UpdateEverythingLocked();
    }
}

void AwSettings::UpdateEverythingLocked()
{
    AutoLock lock(&mAwSettingsLock);
    assert(mNativeAwSettings != 0);
    NativeUpdateEverythingLocked(mNativeAwSettings);
    OnGestureZoomSupportChanged(
            SupportsDoubleTapZoomLocked(), SupportsMultiTouchZoomLocked());
}

/**
 * See {@link android.webkit.WebSettings#setBlockNetworkLoads}.
 */
void AwSettings::SetBlockNetworkLoads(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (!flag && !mHasInternetPermission) {
        // throw new SecurityException("Permission denied - " +
        //         "application missing INTERNET permission");
        Logger::E(TAG, "Permission denied,  missing INTERNET permission");
        assert(0);
    }
    mBlockNetworkLoads = flag;
}

/**
 * See {@link android.webkit.WebSettings#getBlockNetworkLoads}.
 */
Boolean AwSettings::GetBlockNetworkLoads()
{
    AutoLock lock(&mAwSettingsLock);
    return mBlockNetworkLoads;
}

/**
 * Enable/disable third party cookies for an AwContents
 * @param accept true if we should accept third party cookies
 */
void AwSettings::SetAcceptThirdPartyCookies(
    /* [in] */ Boolean accept)
{
    AutoLock lock(&mAwSettingsLock);
    if (mAcceptThirdPartyCookies != accept) {
        mAcceptThirdPartyCookies = accept;
    }
}

/**
 * Return whether third party cookies are enabled for an AwContents
 * @return true if accept third party cookies
 */
Boolean AwSettings::GetAcceptThirdPartyCookies()
{
    AutoLock lock(&mAwSettingsLock);
    return mAcceptThirdPartyCookies;
}

/**
 * See {@link android.webkit.WebSettings#setAllowFileAccess}.
 */
void AwSettings::SetAllowFileAccess(
    /* [in] */ Boolean allow)
{
    AutoLock lock(&mAwSettingsLock);
    if (mAllowFileUrlAccess != allow) {
        mAllowFileUrlAccess = allow;
    }
}

/**
 * See {@link android.webkit.WebSettings#getAllowFileAccess}.
 */
Boolean AwSettings::GetAllowFileAccess()
{
    AutoLock lock(&mAwSettingsLock);
    return mAllowFileUrlAccess;
}

/**
 * See {@link android.webkit.WebSettings#setAllowContentAccess}.
 */
void AwSettings::SetAllowContentAccess(
    /* [in] */ Boolean allow)
{
    AutoLock lock(&mAwSettingsLock);
    if (mAllowContentUrlAccess != allow) {
        mAllowContentUrlAccess = allow;
    }
}

/**
 * See {@link android.webkit.WebSettings#getAllowContentAccess}.
 */
Boolean AwSettings::GetAllowContentAccess()
{
    AutoLock lock(&mAwSettingsLock);
    return mAllowContentUrlAccess;
}

/**
 * See {@link android.webkit.WebSettings#setCacheMode}.
 */
void AwSettings::SetCacheMode(
    /* [in] */ Int32 mode)
{
    AutoLock lock(&mAwSettingsLock);
    if (mCacheMode != mode) {
        mCacheMode = mode;
    }
}

/**
 * See {@link android.webkit.WebSettings#getCacheMode}.
 */
Int32 AwSettings::GetCacheMode()
{
    AutoLock lock(&mAwSettingsLock);
    return mCacheMode;
}

/**
 * See {@link android.webkit.WebSettings#setNeedInitialFocus}.
 */
void AwSettings::SetShouldFocusFirstNode(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    mShouldFocusFirstNode = flag;
}

/**
 * See {@link android.webkit.WebView#setInitialScale}.
 */
void AwSettings::SetInitialPageScale(
    /* [in] */ Float scaleInPercent)
{
    AutoLock lock(&mAwSettingsLock);
    if (mInitialPageScalePercent != scaleInPercent) {
        mInitialPageScalePercent = scaleInPercent;
        AutoPtr<IRunnable> runnable = new SetInitialPageScaleRunnable(this);
        mEventHandler->RunOnUiThreadBlockingAndLocked(runnable);
    }
}

//@CalledByNative
Float AwSettings::GetInitialPageScalePercentLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mInitialPageScalePercent;
}

void AwSettings::SetSpatialNavigationEnabled(
    /* [in] */ Boolean enable)
{
    AutoLock lock(&mAwSettingsLock) ;
    if (mSpatialNavigationEnabled != enable) {
        mSpatialNavigationEnabled = enable;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

//@CalledByNative
Boolean AwSettings::GetSpatialNavigationLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSpatialNavigationEnabled;
}

void AwSettings::SetEnableSupportedHardwareAcceleratedFeatures(
    /* [in] */ Boolean enable)
{
    AutoLock lock(&mAwSettingsLock);
    if (mEnableSupportedHardwareAcceleratedFeatures != enable) {
        mEnableSupportedHardwareAcceleratedFeatures = enable;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

//@CalledByNative
Boolean AwSettings::GetEnableSupportedHardwareAcceleratedFeaturesLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mEnableSupportedHardwareAcceleratedFeatures;
}

void AwSettings::SetFullscreenSupported(
    /* [in] */ Boolean supported)
{
    AutoLock lock(&mAwSettingsLock);
    if (mFullscreenSupported != supported) {
        mFullscreenSupported = supported;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

//@CalledByNative
Boolean AwSettings::GetFullscreenSupportedLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mFullscreenSupported;
}

/**
 * See {@link android.webkit.WebSettings#setNeedInitialFocus}.
 */
Boolean AwSettings::ShouldFocusFirstNode()
{
    AutoLock lock(&mAwSettingsLock);
    return mShouldFocusFirstNode;
}

/**
 * See {@link android.webkit.WebSettings#setGeolocationEnabled}.
 */
void AwSettings::SetGeolocationEnabled(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mGeolocationEnabled != flag) {
        mGeolocationEnabled = flag;
    }
}

/**
 * @return Returns if geolocation is currently enabled.
 */
Boolean AwSettings::GetGeolocationEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return mGeolocationEnabled;
}

/**
 * See {@link android.webkit.WebSettings#setSaveFormData}.
 */
void AwSettings::SetSaveFormData(
    /* [in] */ Boolean enable)
{
    AutoLock lock(&mAwSettingsLock);
        if (mAutoCompleteEnabled != enable) {
            mAutoCompleteEnabled = enable;
            AutoPtr<IRunnable> runnable =  new SetSaveFormDataRunnable(this);
            mEventHandler->RunOnUiThreadBlockingAndLocked(runnable);
        }
}

/**
 * See {@link android.webkit.WebSettings#getSaveFormData}.
 */
Boolean AwSettings::GetSaveFormData()
{
    AutoLock lock(&mAwSettingsLock);
    return GetSaveFormDataLocked();
}

//@CalledByNative
Boolean AwSettings::GetSaveFormDataLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mAutoCompleteEnabled;
}

/**
 * @returns the default User-Agent used by each ContentViewCore instance, i.e. unless
 * overridden by {@link #setUserAgentString()}
 */
String AwSettings::GetDefaultUserAgent()
{
    if (LazyDefaultUserAgent::sInstance == NULL) {
        LazyDefaultUserAgent::sInstance = AwSettings::NativeGetDefaultUserAgent();
    }

    return LazyDefaultUserAgent::sInstance;
}

/**
 * See {@link android.webkit.WebSettings#setUserAgentString}.
 */
void AwSettings::SetUserAgentString(const String& ua)
{
    AutoLock lock(&mAwSettingsLock);
    const String oldUserAgent = mUserAgent;
    if (ua.IsNullOrEmpty()) {
        mUserAgent = GetDefaultUserAgent();
    }
    else {
        mUserAgent = ua;
    }

    if (!oldUserAgent.Equals(mUserAgent)) {
        AutoPtr<IRunnable> runnable = new SetUserAgentStringRunnable(this);
        mEventHandler->RunOnUiThreadBlockingAndLocked(runnable);
    }
}

/**
 * See {@link android.webkit.WebSettings#getUserAgentString}.
 */
String AwSettings::GetUserAgentString()
{
    AutoLock lock(&mAwSettingsLock);
    return GetUserAgentLocked();
}

//@CalledByNative
String AwSettings::GetUserAgentLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mUserAgent;
}

/**
 * See {@link android.webkit.WebSettings#setLoadWithOverviewMode}.
 */
void AwSettings::SetLoadWithOverviewMode(
    /* [in] */ Boolean overview)
{
    AutoLock lock(&mAwSettingsLock);
    if (mLoadWithOverviewMode != overview) {
        mLoadWithOverviewMode = overview;
        AutoPtr<IRunnable> runnable = new SetLoadWithOverviewModeRunnable(this);
        mEventHandler->RunOnUiThreadBlockingAndLocked(runnable);
    }
}

/**
 * See {@link android.webkit.WebSettings#getLoadWithOverviewMode}.
 */
Boolean AwSettings::GetLoadWithOverviewMode()
{
    AutoLock lock(&mAwSettingsLock);
    return GetLoadWithOverviewModeLocked();
}

//@CalledByNative
Boolean AwSettings::GetLoadWithOverviewModeLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mLoadWithOverviewMode;
}

/**
 * See {@link android.webkit.WebSettings#setTextZoom}.
 */
void AwSettings::SetTextZoom(
    /* [in] */ Int32 textZoom)
{
    AutoLock lock(&mAwSettingsLock);
    if (mTextSizePercent != textZoom) {
        mTextSizePercent = textZoom;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getTextZoom}.
 */
Int32 AwSettings::GetTextZoom()
{
    AutoLock lock(&mAwSettingsLock);
    return GetTextSizePercentLocked();
}

//@CalledByNative
Int32 AwSettings::GetTextSizePercentLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mTextSizePercent;
}

/**
 * See {@link android.webkit.WebSettings#setStandardFontFamily}.
 */
void AwSettings::SetStandardFontFamily(
    /* [in] */ const String& font)
{
    AutoLock lock(&mAwSettingsLock);
    if (font != NULL && !mStandardFontFamily.Equals(font)) {
        mStandardFontFamily = font;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getStandardFontFamily}.
 */
String AwSettings::GetStandardFontFamily()
{
    AutoLock lock(&mAwSettingsLock);
    return GetStandardFontFamilyLocked();
}

//@CalledByNative
String AwSettings::GetStandardFontFamilyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mStandardFontFamily;
}

/**
 * See {@link android.webkit.WebSettings#setFixedFontFamily}.
 */
void AwSettings::SetFixedFontFamily(
    /* [in] */ const String& font)
{
    AutoLock lock(&mAwSettingsLock);
    if (!font.IsNull() && !mFixedFontFamily.Equals(font)) {
        mFixedFontFamily = font;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getFixedFontFamily}.
 */
String AwSettings::GetFixedFontFamily()
{
    AutoLock lock(&mAwSettingsLock);
    return GetFixedFontFamilyLocked();
}

//@CalledByNative
String AwSettings::GetFixedFontFamilyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mFixedFontFamily;
}

/**
 * See {@link android.webkit.WebSettings#setSansSerifFontFamily}.
 */
void AwSettings::SetSansSerifFontFamily(
    /* [in] */ const String& font)
{
    AutoLock lock(&mAwSettingsLock);
    if (!font.IsNull() && !mSansSerifFontFamily.Equals(font)) {
        mSansSerifFontFamily = font;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getSansSerifFontFamily}.
 */
String AwSettings::GetSansSerifFontFamily()
{
    AutoLock lock(&mAwSettingsLock);
    return GetSansSerifFontFamilyLocked();
}

//@CalledByNative
String AwSettings::GetSansSerifFontFamilyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSansSerifFontFamily;
}

/**
 * See {@link android.webkit.WebSettings#setSerifFontFamily}.
 */
void AwSettings::SetSerifFontFamily(
    /* [in] */ const String& font)
{
    AutoLock lock(&mAwSettingsLock);
    if (!font.IsNull() && !mSerifFontFamily.Equals(font)) {
        mSerifFontFamily = font;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getSerifFontFamily}.
 */
String AwSettings::GetSerifFontFamily()
{
    AutoLock lock(&mAwSettingsLock);
    return GetSerifFontFamilyLocked();
}

//@CalledByNative
String AwSettings::GetSerifFontFamilyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSerifFontFamily;
}

/**
 * See {@link android.webkit.WebSettings#setCursiveFontFamily}.
 */
void AwSettings::SetCursiveFontFamily(
    /* [in] */ const String& font)
{
    AutoLock lock(&mAwSettingsLock);
    if (!font.IsNull() && !mCursiveFontFamily.Equals(font)) {
        mCursiveFontFamily = font;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getCursiveFontFamily}.
 */
String AwSettings::GetCursiveFontFamily()
{
    AutoLock lock(&mAwSettingsLock);
    return GetCursiveFontFamilyLocked();
}

//@CalledByNative
String AwSettings::GetCursiveFontFamilyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mCursiveFontFamily;
}

/**
 * See {@link android.webkit.WebSettings#setFantasyFontFamily}.
 */
void AwSettings::SetFantasyFontFamily(
    /* [in] */ const String& font)
{
    AutoLock lock(&mAwSettingsLock);
    if (!font.IsNull() && !mFantasyFontFamily.Equals(font)) {
        mFantasyFontFamily = font;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getFantasyFontFamily}.
 */
String AwSettings::GetFantasyFontFamily()
{
    AutoLock lock(&mAwSettingsLock);
    return GetFantasyFontFamilyLocked();
}

//@CalledByNative
String AwSettings::GetFantasyFontFamilyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mFantasyFontFamily;
}

/**
 * See {@link android.webkit.WebSettings#setMinimumFontSize}.
 */
void AwSettings::SetMinimumFontSize(
    /* [in] */ Int32 size)
{
    AutoLock lock(&mAwSettingsLock);
    size = ClipFontSize(size);
    if (mMinimumFontSize != size) {
        mMinimumFontSize = size;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getMinimumFontSize}.
 */
Int32 AwSettings::GetMinimumFontSize()
{
    AutoLock lock(&mAwSettingsLock);
    return GetMinimumFontSizeLocked();
}

//@CalledByNative
Int32 AwSettings::GetMinimumFontSizeLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mMinimumFontSize;
}

/**
 * See {@link android.webkit.WebSettings#setMinimumLogicalFontSize}.
 */
void AwSettings::SetMinimumLogicalFontSize(
    /* [in] */ Int32 size)
{
    AutoLock lock(&mAwSettingsLock);
    size = ClipFontSize(size);
    if (mMinimumLogicalFontSize != size) {
        mMinimumLogicalFontSize = size;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getMinimumLogicalFontSize}.
 */
Int32 AwSettings::GetMinimumLogicalFontSize()
{
    AutoLock lock(&mAwSettingsLock);
    return GetMinimumLogicalFontSizeLocked();
}

//@CalledByNative
Int32 AwSettings::GetMinimumLogicalFontSizeLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mMinimumLogicalFontSize;
}

/**
 * See {@link android.webkit.WebSettings#setDefaultFontSize}.
 */
void AwSettings::SetDefaultFontSize(
    /* [in] */ Int32 size)
{
    AutoLock lock(&mAwSettingsLock);
    size = ClipFontSize(size);
    if (mDefaultFontSize != size) {
        mDefaultFontSize = size;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getDefaultFontSize}.
 */
Int32 AwSettings::GetDefaultFontSize()
{
    AutoLock lock(&mAwSettingsLock);
    return GetDefaultFontSizeLocked();
}

//@CalledByNative
Int32 AwSettings::GetDefaultFontSizeLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDefaultFontSize;
}

/**
 * See {@link android.webkit.WebSettings#setDefaultFixedFontSize}.
 */
void AwSettings::SetDefaultFixedFontSize(
    /* [in] */ Int32 size)
{
    AutoLock lock(&mAwSettingsLock);
    size = ClipFontSize(size);
    if (mDefaultFixedFontSize != size) {
        mDefaultFixedFontSize = size;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getDefaultFixedFontSize}.
 */
Int32 AwSettings::GetDefaultFixedFontSize()
{
    AutoLock lock(&mAwSettingsLock);
    return GetDefaultFixedFontSizeLocked();
}

//@CalledByNative
Int32 AwSettings::GetDefaultFixedFontSizeLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDefaultFixedFontSize;
}

/**
 * See {@link android.webkit.WebSettings#setJavaScriptEnabled}.
 */
void AwSettings::SetJavaScriptEnabled(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mJavaScriptEnabled != flag) {
        mJavaScriptEnabled = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#setAllowUniversalAccessFromFileURLs}.
 */
void AwSettings::SetAllowUniversalAccessFromFileURLs(Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mAllowUniversalAccessFromFileURLs != flag) {
        mAllowUniversalAccessFromFileURLs = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#setAllowFileAccessFromFileURLs}.
 */
void AwSettings::SetAllowFileAccessFromFileURLs(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mAllowFileAccessFromFileURLs != flag) {
        mAllowFileAccessFromFileURLs = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#setLoadsImagesAutomatically}.
 */
void AwSettings::SetLoadsImagesAutomatically(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mLoadsImagesAutomatically != flag) {
        mLoadsImagesAutomatically = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getLoadsImagesAutomatically}.
 */
Boolean AwSettings::GetLoadsImagesAutomatically()
{
    AutoLock lock(&mAwSettingsLock);
    return GetLoadsImagesAutomaticallyLocked();
}

//@CalledByNative
Boolean AwSettings::GetLoadsImagesAutomaticallyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mLoadsImagesAutomatically;
}

/**
 * See {@link android.webkit.WebSettings#setImagesEnabled}.
 */
void AwSettings::SetImagesEnabled(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mImagesEnabled != flag) {
        mImagesEnabled = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getImagesEnabled}.
 */
Boolean AwSettings::GetImagesEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return mImagesEnabled;
}

//@CalledByNative
Boolean AwSettings::GetImagesEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mImagesEnabled;
}

/**
 * See {@link android.webkit.WebSettings#getJavaScriptEnabled}.
 */
Boolean AwSettings::GetJavaScriptEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return mJavaScriptEnabled;
}

//@CalledByNative
Boolean AwSettings::GetJavaScriptEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mJavaScriptEnabled;
}

/**
 * See {@link android.webkit.WebSettings#getAllowUniversalAccessFromFileURLs}.
 */
Boolean AwSettings::GetAllowUniversalAccessFromFileURLs()
{
    AutoLock lock(&mAwSettingsLock);
    return GetAllowUniversalAccessFromFileURLsLocked();
}

//@CalledByNative
Boolean AwSettings::GetAllowUniversalAccessFromFileURLsLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mAllowUniversalAccessFromFileURLs;
}

/**
 * See {@link android.webkit.WebSettings#getAllowFileAccessFromFileURLs}.
 */
Boolean AwSettings::GetAllowFileAccessFromFileURLs()
{
    AutoLock lock(&mAwSettingsLock);
    return GetAllowFileAccessFromFileURLsLocked();
}

//@CalledByNative
Boolean AwSettings::GetAllowFileAccessFromFileURLsLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mAllowFileAccessFromFileURLs;
}

/**
 * See {@link android.webkit.WebSettings#setPluginsEnabled}.
 */
void AwSettings::SetPluginsEnabled(Boolean flag)
{
    SetPluginState(flag ? PluginState_ON : PluginState_OFF);
}

/**
 * See {@link android.webkit.WebSettings#setPluginState}.
 */
void AwSettings::SetPluginState(PluginState state)
{
    AutoLock lock(&mAwSettingsLock);
    if (mPluginState != state) {
        mPluginState = state;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getPluginsEnabled}.
 */
Boolean AwSettings::GetPluginsEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return mPluginState == PluginState_ON;
}

/**
 * Return true if plugins are disabled.
 * @return True if plugins are disabled.
 */
//@CalledByNative
Boolean AwSettings::GetPluginsDisabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mPluginState == PluginState_OFF;
}

/**
 * See {@link android.webkit.WebSettings#getPluginState}.
 */
PluginState AwSettings::GetPluginState()
{
    AutoLock lock(&mAwSettingsLock);
    return mPluginState;
}


/**
 * See {@link android.webkit.WebSettings#setJavaScriptCanOpenWindowsAutomatically}.
 */
void AwSettings::SetJavaScriptCanOpenWindowsAutomatically(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mJavaScriptCanOpenWindowsAutomatically != flag) {
        mJavaScriptCanOpenWindowsAutomatically = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getJavaScriptCanOpenWindowsAutomatically}.
 */
Boolean AwSettings::GetJavaScriptCanOpenWindowsAutomatically()
{
    AutoLock lock(&mAwSettingsLock);
    return GetJavaScriptCanOpenWindowsAutomaticallyLocked();
}

//@CalledByNative
Boolean AwSettings::GetJavaScriptCanOpenWindowsAutomaticallyLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mJavaScriptCanOpenWindowsAutomatically;
}

/**
 * See {@link android.webkit.WebSettings#setLayoutAlgorithm}.
 */
void AwSettings::SetLayoutAlgorithm(
    /* [in] */ LayoutAlgorithm l)
{
    AutoLock lock(&mAwSettingsLock);
    if (mLayoutAlgorithm != l) {
        mLayoutAlgorithm = l;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getLayoutAlgorithm}.
 */
AwSettings::LayoutAlgorithm AwSettings::GetLayoutAlgorithm()
{
    AutoLock lock(&mAwSettingsLock);
    return mLayoutAlgorithm;
}

/**
 * Gets whether Text Auto-sizing layout algorithm is enabled.
 *
 * @return true if Text Auto-sizing layout algorithm is enabled
 */
//@CalledByNative
Boolean AwSettings::GetTextAutosizingEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mLayoutAlgorithm == TEXT_AUTOSIZING;
}

/**
 * See {@link android.webkit.WebSettings#setSupportMultipleWindows}.
 */
void AwSettings::SetSupportMultipleWindows(
    /* [in] */ Boolean support)
{
    AutoLock lock(&mAwSettingsLock);
    if (mSupportMultipleWindows != support) {
        mSupportMultipleWindows = support;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#supportMultipleWindows}.
 */
Boolean AwSettings::SupportMultipleWindows()
{
    AutoLock lock(&mAwSettingsLock);
    return mSupportMultipleWindows;
}

//@CalledByNative
Boolean AwSettings::GetSupportMultipleWindowsLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSupportMultipleWindows;
}

//@CalledByNative
Boolean AwSettings::GetSupportLegacyQuirksLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSupportLegacyQuirks;
}

/**
 * See {@link android.webkit.WebSettings#setUseWideViewPort}.
 */
void AwSettings::SetUseWideViewPort(
    /* [in] */ Boolean use)
{
    AutoLock lock(&mAwSettingsLock);
    if (mUseWideViewport != use) {
        mUseWideViewport = use;
        OnGestureZoomSupportChanged(
                SupportsDoubleTapZoomLocked(), SupportsMultiTouchZoomLocked());
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getUseWideViewPort}.
 */
Boolean AwSettings::GetUseWideViewPort()
{
    AutoLock lock(&mAwSettingsLock);
    return GetUseWideViewportLocked();
}

//@CalledByNative
Boolean AwSettings::GetUseWideViewportLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mUseWideViewport;
}

void AwSettings::SetZeroLayoutHeightDisablesViewportQuirk(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(&mAwSettingsLock);
    if (mZeroLayoutHeightDisablesViewportQuirk != enabled) {
        mZeroLayoutHeightDisablesViewportQuirk = enabled;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

Boolean AwSettings::GetZeroLayoutHeightDisablesViewportQuirk()
{
    AutoLock lock(&mAwSettingsLock);
    return GetZeroLayoutHeightDisablesViewportQuirkLocked();
}

//@CalledByNative
Boolean AwSettings::GetZeroLayoutHeightDisablesViewportQuirkLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mZeroLayoutHeightDisablesViewportQuirk;
}

void AwSettings::SetForceZeroLayoutHeight(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(&mAwSettingsLock);
    if (mForceZeroLayoutHeight != enabled) {
        mForceZeroLayoutHeight = enabled;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

Boolean AwSettings::GetForceZeroLayoutHeight()
{
    AutoLock lock(&mAwSettingsLock);
    return GetForceZeroLayoutHeightLocked();
}

//@CalledByNative
Boolean AwSettings::GetForceZeroLayoutHeightLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mForceZeroLayoutHeight;
}

//@CalledByNative
Boolean AwSettings::GetPasswordEchoEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mPasswordEchoEnabled;
}

/**
 * See {@link android.webkit.WebSettings#setAppCacheEnabled}.
 */
void AwSettings::SetAppCacheEnabled(Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mAppCacheEnabled != flag) {
        mAppCacheEnabled = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#setAppCachePath}.
 */
void AwSettings::SetAppCachePath(
    /* [in] */ const String& path)
{
    Boolean needToSync = FALSE;
    {
        AutoLock lock(&sGlobalContentSettingsLock);
        // AppCachePath can only be set once.
        if (!sAppCachePathIsSet && !path.IsNull() && !path.IsEmpty()) {
            sAppCachePathIsSet = TRUE;
            needToSync = TRUE;
        }
    }
    // The obvious problem here is that other WebViews will not be updated,
    // until they execute synchronization from Java to the native side.
    // But this is the same behaviour as it was in the legacy WebView.
    if (needToSync) {
        AutoLock lock(&mAwSettingsLock);
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * Gets whether Application Cache is enabled.
 *
 * @return true if Application Cache is enabled
 */
//@CalledByNative
Boolean AwSettings::GetAppCacheEnabledLocked()
{
    AutoLock autoLock(&mAwSettingsLock);
    if (!mAppCacheEnabled) {
        return FALSE;
    }

    AutoLock lock(&sGlobalContentSettingsLock);
    return sAppCachePathIsSet;
}

/**
 * See {@link android.webkit.WebSettings#setDomStorageEnabled}.
 */
void AwSettings::SetDomStorageEnabled(
    /* [in] */ Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    {
        if (mDomStorageEnabled != flag) {
            mDomStorageEnabled = flag;
            mEventHandler->UpdateWebkitPreferencesLocked();
        }
    }
}

/**
 * See {@link android.webkit.WebSettings#getDomStorageEnabled}.
 */
Boolean AwSettings::GetDomStorageEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return mDomStorageEnabled;
}

//@CalledByNative
Boolean AwSettings::GetDomStorageEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDomStorageEnabled;
}

/**
 * See {@link android.webkit.WebSettings#setDatabaseEnabled}.
 */
void AwSettings::SetDatabaseEnabled(Boolean flag)
{
    AutoLock lock(&mAwSettingsLock);
    if (mDatabaseEnabled != flag) {
        mDatabaseEnabled = flag;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getDatabaseEnabled}.
 */
Boolean AwSettings::GetDatabaseEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return mDatabaseEnabled;
}

//@CalledByNative
Boolean AwSettings::GetDatabaseEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDatabaseEnabled;
}

/**
 * See {@link android.webkit.WebSettings#setDefaultTextEncodingName}.
 */
void AwSettings::SetDefaultTextEncodingName(
    /* [in] */ const String& encoding)
{
    AutoLock lock(&mAwSettingsLock);
    if (!encoding.IsNull() && !mDefaultTextEncoding.Equals(encoding)) {
        mDefaultTextEncoding = encoding;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getDefaultTextEncodingName}.
 */
String AwSettings::GetDefaultTextEncodingName()
{
    AutoLock lock(&mAwSettingsLock);
    return GetDefaultTextEncodingLocked();
}

//@CalledByNative
String AwSettings::GetDefaultTextEncodingLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDefaultTextEncoding;
}

/**
 * See {@link android.webkit.WebSettings#setMediaPlaybackRequiresUserGesture}.
 */
void AwSettings::SetMediaPlaybackRequiresUserGesture(
    /* [in] */ Boolean require)
{
    AutoLock lock(&mAwSettingsLock);
    if (mMediaPlaybackRequiresUserGesture != require) {
        mMediaPlaybackRequiresUserGesture = require;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getMediaPlaybackRequiresUserGesture}.
 */
Boolean AwSettings::GetMediaPlaybackRequiresUserGesture()
{
    AutoLock lock(&mAwSettingsLock);
    return GetMediaPlaybackRequiresUserGestureLocked();
}

//@CalledByNative
Boolean AwSettings::GetMediaPlaybackRequiresUserGestureLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mMediaPlaybackRequiresUserGesture;
}

/**
 * See {@link android.webkit.WebSettings#setDefaultVideoPosterURL}.
 */
void AwSettings::SetDefaultVideoPosterURL(
    /* [in] */ const String& url)
{
    AutoLock lock(&mAwSettingsLock);
    if ((!mDefaultVideoPosterURL.IsNull() && !mDefaultVideoPosterURL.Equals(url)) ||
            (mDefaultVideoPosterURL.IsNull() && !url.IsNull())) {
        mDefaultVideoPosterURL = url;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

/**
 * See {@link android.webkit.WebSettings#getDefaultVideoPosterURL}.
 */
String AwSettings::GetDefaultVideoPosterURL()
{
    AutoLock lock(&mAwSettingsLock);
    return GetDefaultVideoPosterURLLocked();
}

//@CalledByNative
String AwSettings::GetDefaultVideoPosterURLLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mDefaultVideoPosterURL;
}

void AwSettings::OnGestureZoomSupportChanged(
    /* [in] */ Boolean supportsDoubleTapZoom,
    /* [in] */ Boolean supportsMultiTouchZoom)
{
    // Always post asynchronously here, to avoid doubling back onto the caller.
    AutoPtr<IRunnable> runnable = new OnGestureZoomSupportChangedRunnable(this, supportsDoubleTapZoom, supportsMultiTouchZoom);
    mEventHandler->MaybePostOnUiThread(runnable);
}

/**
 * See {@link android.webkit.WebSettings#setSupportZoom}.
 */
void AwSettings::SetSupportZoom(
    /* [in] */ Boolean support)
{
    AutoLock lock(&mAwSettingsLock);
    if (mSupportZoom != support) {
        mSupportZoom = support;
        OnGestureZoomSupportChanged(
                SupportsDoubleTapZoomLocked(), SupportsMultiTouchZoomLocked());
    }
}

/**
 * See {@link android.webkit.WebSettings#supportZoom}.
 */
Boolean AwSettings::SupportZoom()
{
    AutoLock lock(&mAwSettingsLock);
    return mSupportZoom;
}

/**
 * See {@link android.webkit.WebSettings#setBuiltInZoomControls}.
 */
void AwSettings::SetBuiltInZoomControls(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(&mAwSettingsLock);
    if (mBuiltInZoomControls != enabled) {
        mBuiltInZoomControls = enabled;
        OnGestureZoomSupportChanged(
                SupportsDoubleTapZoomLocked(), SupportsMultiTouchZoomLocked());
    }
}

/**
 * See {@link android.webkit.WebSettings#getBuiltInZoomControls}.
 */
Boolean AwSettings::GetBuiltInZoomControls()
{
    AutoLock lock(&mAwSettingsLock);
    return mBuiltInZoomControls;
}

/**
 * See {@link android.webkit.WebSettings#setDisplayZoomControls}.
 */
void AwSettings::SetDisplayZoomControls(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(&mAwSettingsLock);
    mDisplayZoomControls = enabled;
}

/**
 * See {@link android.webkit.WebSettings#getDisplayZoomControls}.
 */
Boolean AwSettings::GetDisplayZoomControls()
{
    AutoLock lock(&mAwSettingsLock);
    return mDisplayZoomControls;
}

void AwSettings::SetMixedContentMode(
    /* [in] */ Int32 mode)
{
    AutoLock lock(&mAwSettingsLock);
    if (mMixedContentMode != mode) {
        mMixedContentMode = mode;
        mEventHandler->UpdateWebkitPreferencesLocked();
    }
}

Int32 AwSettings::GetMixedContentMode()
{
    AutoLock lock(&mAwSettingsLock);
    return mMixedContentMode;
}

//@CalledByNative
Boolean AwSettings::GetAllowRunningInsecureContentLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mMixedContentMode == MIXED_CONTENT_ALWAYS_ALLOW;
}

//@CalledByNative
Boolean AwSettings::GetAllowDisplayingInsecureContentLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mMixedContentMode == MIXED_CONTENT_ALWAYS_ALLOW ||
            mMixedContentMode == MIXED_CONTENT_COMPATIBILITY_MODE;
}

/**
 * Sets whether to use the video overlay for the embedded video.
 * @param flag whether to enable the video overlay for the embedded video.
 */
void AwSettings::SetVideoOverlayForEmbeddedVideoEnabled(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(&mAwSettingsLock);
    if (mVideoOverlayForEmbeddedVideoEnabled != enabled) {
        mVideoOverlayForEmbeddedVideoEnabled = enabled;
        AutoPtr<IRunnable> runnable = new SetVideoOverlayForEmbeddedVideoEnabledRunnable(this);
        mEventHandler->RunOnUiThreadBlockingAndLocked(runnable);
    }
}

/**
 * Gets whether to use the video overlay for the embedded video.
 * @return true if the WebView enables the video overlay for the embedded video.
 */
Boolean AwSettings::GetVideoOverlayForEmbeddedVideoEnabled()
{
    AutoLock lock(&mAwSettingsLock);
    return GetVideoOverlayForEmbeddedVideoEnabledLocked();
}

//@CalledByNative
Boolean AwSettings::GetVideoOverlayForEmbeddedVideoEnabledLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mVideoOverlayForEmbeddedVideoEnabled;
}

//@CalledByNative
Boolean AwSettings::SupportsDoubleTapZoomLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSupportZoom && mBuiltInZoomControls && mUseWideViewport;
}

Boolean AwSettings::SupportsMultiTouchZoomLocked()
{
    AutoLock lock(&mAwSettingsLock);
    return mSupportZoom && mBuiltInZoomControls;
}

Boolean AwSettings::SupportsMultiTouchZoom() {
    AutoLock lock(&mAwSettingsLock);
    return SupportsMultiTouchZoomLocked();
}

Boolean AwSettings::ShouldDisplayZoomControls()
{
    AutoLock lock(&mAwSettingsLock);
    return SupportsMultiTouchZoomLocked() && mDisplayZoomControls;
}

Int32 AwSettings::ClipFontSize(
    /* [in] */ Int32 size)
{
    if (size < MINIMUM_FONT_SIZE) {
        return MINIMUM_FONT_SIZE;
    }
    else if (size > MAXIMUM_FONT_SIZE) {
        return MAXIMUM_FONT_SIZE;
    }
    return size;
}

//@CalledByNative
void AwSettings::UpdateEverything()
{
    AutoLock lock(&mAwSettingsLock);
    UpdateEverythingLocked();
}

//@CalledByNative
void AwSettings::PopulateWebPreferences(
    /* [in] */ HANDLE webPrefsPtr)
{
    AutoLock lock(&mAwSettingsLock);
    assert(mNativeAwSettings != 0);
    NativePopulateWebPreferencesLocked(mNativeAwSettings, webPrefsPtr);
}

void AwSettings::UpdateWebkitPreferencesOnUiThreadLocked()
{
    assert(mEventHandler->mHandler != NULL);
    ThreadUtils::AssertOnUiThread();
    if (mNativeAwSettings != 0) {
        NativeUpdateWebkitPreferencesLocked(mNativeAwSettings);
    }
}

HANDLE AwSettings::NativeInit(
    /* [in] */ HANDLE webContentsPtr)
{
    return Elastos_AwSettings_nativeInit(TO_IINTERFACE(this), webContentsPtr);
}

void AwSettings::NativeDestroy(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeDestroy(TO_IINTERFACE(this), nativeAwSettings);
}

void AwSettings::NativePopulateWebPreferencesLocked(
    /* [in] */ HANDLE nativeAwSettings,
    /* [in] */ HANDLE webPrefsPtr)
{
    Elastos_AwSettings_nativePopulateWebPreferencesLocked(TO_IINTERFACE(this), nativeAwSettings, webPrefsPtr);
}

void AwSettings::NativeResetScrollAndScaleState(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeResetScrollAndScaleState(TO_IINTERFACE(this), nativeAwSettings);
}

void AwSettings::NativeUpdateEverythingLocked(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeUpdateEverythingLocked(TO_IINTERFACE(this), nativeAwSettings);
}

void AwSettings::NativeUpdateInitialPageScaleLocked(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeUpdateInitialPageScaleLocked(TO_IINTERFACE(this), nativeAwSettings);
}

void AwSettings::NativeUpdateUserAgentLocked(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeUpdateUserAgentLocked(TO_IINTERFACE(this), nativeAwSettings);
}

void AwSettings::NativeUpdateWebkitPreferencesLocked(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeUpdateWebkitPreferencesLocked(TO_IINTERFACE(this), nativeAwSettings);
}

String AwSettings::NativeGetDefaultUserAgent()
{
    return Elastos_AwSettings_nativeGetDefaultUserAgent();
}

void AwSettings::NativeUpdateFormDataPreferencesLocked(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeUpdateFormDataPreferencesLocked(TO_IINTERFACE(this), nativeAwSettings);
}

void AwSettings::NativeUpdateRendererPreferencesLocked(
    /* [in] */ HANDLE nativeAwSettings)
{
    Elastos_AwSettings_nativeUpdateRendererPreferencesLocked(TO_IINTERFACE(this), nativeAwSettings);
}
//callback function definition
void AwSettings::NativeAwSettingsGone(
    /* [in] */ IInterface* obj,
    /* [in] */ HANDLE nativeAwSettings)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::NativeAwSettingsGone, mObj is NULL");
        return;
    }
    mObj->NativeAwSettingsGone(nativeAwSettings);
}

Double AwSettings::GetDIPScaleLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDIPScaleLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetDIPScaleLocked();
}

Float AwSettings::GetInitialPageScalePercentLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetInitialPageScalePercentLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetInitialPageScalePercentLocked();
}

Boolean AwSettings::GetSpatialNavigationLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetSpatialNavigationLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetSpatialNavigationLocked();
}

Boolean AwSettings::GetEnableSupportedHardwareAcceleratedFeaturesLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetEnableSupportedHardwareAcceleratedFeaturesLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetEnableSupportedHardwareAcceleratedFeaturesLocked();
}

Boolean AwSettings::GetFullscreenSupportedLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetFullscreenSupportedLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetFullscreenSupportedLocked();
}

Boolean AwSettings::GetSaveFormDataLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetSaveFormDataLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetSaveFormDataLocked();
}

String AwSettings::GetUserAgentLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetUserAgentLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetUserAgentLocked();
}

Boolean AwSettings::GetLoadWithOverviewModeLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetLoadWithOverviewModeLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetLoadWithOverviewModeLocked();
}

Int32 AwSettings::GetTextSizePercentLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetTextSizePercentLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetTextSizePercentLocked();
}

String AwSettings::GetStandardFontFamilyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetStandardFontFamilyLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetStandardFontFamilyLocked();
}

String AwSettings::GetFixedFontFamilyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetFixedFontFamilyLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetFixedFontFamilyLocked();
}

String AwSettings::GetSansSerifFontFamilyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetSansSerifFontFamilyLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetSansSerifFontFamilyLocked();
}

String AwSettings::GetSerifFontFamilyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetSerifFontFamilyLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetSerifFontFamilyLocked();
}

String AwSettings::GetCursiveFontFamilyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetCursiveFontFamilyLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetCursiveFontFamilyLocked();
}

String AwSettings::GetFantasyFontFamilyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetFantasyFontFamilyLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetFantasyFontFamilyLocked();
}

Int32 AwSettings::GetMinimumFontSizeLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetMinimumFontSizeLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetMinimumFontSizeLocked();
}

Int32 AwSettings::GetMinimumLogicalFontSizeLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetMinimumLogicalFontSizeLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetMinimumLogicalFontSizeLocked();
}

Int32 AwSettings::GetDefaultFontSizeLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDefaultFontSizeLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetDefaultFontSizeLocked();
}

Int32 AwSettings::GetDefaultFixedFontSizeLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDefaultFixedFontSizeLocked, mObj is NULL");
        return 0;
    }
    return mObj->GetDefaultFixedFontSizeLocked();
}

Boolean AwSettings::GetLoadsImagesAutomaticallyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetLoadsImagesAutomaticallyLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetLoadsImagesAutomaticallyLocked();
}

Boolean AwSettings::GetImagesEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetImagesEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetImagesEnabledLocked();
}

Boolean AwSettings::GetJavaScriptEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetJavaScriptEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetJavaScriptEnabledLocked();
}

Boolean AwSettings::GetAllowUniversalAccessFromFileURLsLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetAllowUniversalAccessFromFileURLsLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetAllowUniversalAccessFromFileURLsLocked();
}

Boolean AwSettings::GetAllowFileAccessFromFileURLsLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetAllowFileAccessFromFileURLsLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetAllowFileAccessFromFileURLsLocked();
}

Boolean AwSettings::GetPluginsDisabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetPluginsDisabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetPluginsDisabledLocked();
}

Boolean AwSettings::GetJavaScriptCanOpenWindowsAutomaticallyLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetJavaScriptCanOpenWindowsAutomaticallyLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetJavaScriptCanOpenWindowsAutomaticallyLocked();
}

Boolean AwSettings::GetTextAutosizingEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetTextAutosizingEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetTextAutosizingEnabledLocked();
}

Boolean AwSettings::GetSupportMultipleWindowsLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetSupportMultipleWindowsLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetSupportMultipleWindowsLocked();
}

Boolean AwSettings::GetSupportLegacyQuirksLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetSupportLegacyQuirksLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetSupportLegacyQuirksLocked();
}

Boolean AwSettings::GetUseWideViewportLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetUseWideViewportLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetUseWideViewportLocked();
}

Boolean AwSettings::GetZeroLayoutHeightDisablesViewportQuirkLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetZeroLayoutHeightDisablesViewportQuirkLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetZeroLayoutHeightDisablesViewportQuirkLocked();
}

Boolean AwSettings::GetForceZeroLayoutHeightLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetForceZeroLayoutHeightLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetForceZeroLayoutHeightLocked();
}

Boolean AwSettings::GetPasswordEchoEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetPasswordEchoEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetPasswordEchoEnabledLocked();
}

Boolean AwSettings::GetAppCacheEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetAppCacheEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetAppCacheEnabledLocked();
}

Boolean AwSettings::GetDomStorageEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDomStorageEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetDomStorageEnabledLocked();
}

Boolean AwSettings::GetDatabaseEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDatabaseEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetDatabaseEnabledLocked();
}

String AwSettings::GetDefaultTextEncodingLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDefaultTextEncodingLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetDefaultTextEncodingLocked();
}

Boolean AwSettings::GetMediaPlaybackRequiresUserGestureLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetMediaPlaybackRequiresUserGestureLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetMediaPlaybackRequiresUserGestureLocked();
}

String AwSettings::GetDefaultVideoPosterURLLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetDefaultVideoPosterURLLocked, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetDefaultVideoPosterURLLocked();
}

Boolean AwSettings::GetAllowRunningInsecureContentLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetAllowRunningInsecureContentLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetAllowRunningInsecureContentLocked();
}

Boolean AwSettings::GetAllowDisplayingInsecureContentLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetAllowDisplayingInsecureContentLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetAllowDisplayingInsecureContentLocked();
}

Boolean AwSettings::GetVideoOverlayForEmbeddedVideoEnabledLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::GetVideoOverlayForEmbeddedVideoEnabledLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->GetVideoOverlayForEmbeddedVideoEnabledLocked();
}

Boolean AwSettings::SupportsDoubleTapZoomLocked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::SupportsDoubleTapZoomLocked, mObj is NULL");
        return FALSE;
    }
    return mObj->SupportsDoubleTapZoomLocked();
}

void AwSettings::UpdateEverything(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::UpdateEverything, mObj is NULL");
        return;
    }
    mObj->UpdateEverything();
}

void AwSettings::PopulateWebPreferences(
    /* [in] */ IInterface* obj,
    /* [in] */ HANDLE webPrefsPtr)
{
    AutoPtr<AwSettings> mObj = (AwSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwSettings::PopulateWebPreferences, mObj is NULL");
        return;
    }
    mObj->PopulateWebPreferences(webPrefsPtr);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
