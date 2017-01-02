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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWSETTINGS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWSETTINGS_H__

#include <Elastos.Droid.Webkit.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Webkit::IWebSettings;
using Elastos::Droid::Webkit::PluginState;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Stores Android WebView specific settings that does not need to be synced to WebKit.
 * Use {@link org.chromium.content.browser.ContentSettings} for WebKit settings.
 *
 * Methods in this class can be called from any thread, including threads created by
 * the client of WebView.
 */
//@JNINamespace("android_webview")
class AwSettings
    : public Object
{
public:
    class SetInitialPageScaleRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        SetInitialPageScaleRunnable(
            /* [in] */ AwSettings* owner);

        CARAPI Run();

    private:
        AwSettings* mOwner;
    };

    class SetSaveFormDataRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        SetSaveFormDataRunnable(
            /* [in] */ AwSettings* owner);

        CARAPI Run();

    private:
        AwSettings* mOwner;
    };

    class SetUserAgentStringRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        SetUserAgentStringRunnable(
            /* [in] */ AwSettings* owner);

        CARAPI Run();

    private:
        AwSettings* mOwner;
    };

    class SetLoadWithOverviewModeRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        SetLoadWithOverviewModeRunnable(
            /* [in] */ AwSettings* owner);

        CARAPI Run();

    private:
        AwSettings* mOwner;
    };

    class OnGestureZoomSupportChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        OnGestureZoomSupportChangedRunnable(
            /* [in] */ AwSettings* owner,
            /* [in] */ Boolean supportsDoubleTapZoom,
            /* [in] */ Boolean supportsMultiTouchZoom);

        CARAPI Run();

    private:
        AwSettings* mOwner;
        Boolean mSupportsDoubleTapZoom;
        Boolean mSupportsMultiTouchZoom;
    };

    class SetVideoOverlayForEmbeddedVideoEnabledRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        SetVideoOverlayForEmbeddedVideoEnabledRunnable(
            /* [in] */ AwSettings* owner);

        CARAPI Run();

    private:
        AwSettings* mOwner;
    };

public:
    // This enum corresponds to WebSettings.LayoutAlgorithm. We use our own to be
    // able to extend it.
    enum LayoutAlgorithm
    {
        NORMAL,
        SINGLE_COLUMN,
        NARROW_COLUMNS,
        TEXT_AUTOSIZING,
    };

    class LazyDefaultUserAgent
    {
    public:
        // Lazy Holder pattern
        static String sInstance;
    };

    class ZoomSupportChangeListener
        :public Object
    {
    public:
        virtual CARAPI_(void) OnGestureZoomSupportChanged(
            /* [in] */ Boolean supportsDoubleTapZoom,
            /* [in] */ Boolean supportsMultiTouchZoom) = 0;
    };

public:
    // Class to handle messages to be processed on the UI thread.
    class EventHandler
        : public Object
    {
        friend class AwSettings;

    public:
        class InnerHandler
            : public Handler
        {
        public:
            InnerHandler(
                /* [in] */ EventHandler* owner,
                /* [in] */ ILooper* looper);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            EventHandler* mOwner;
        };

        class InnerRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()
            InnerRunnable(
                /* [in] */ EventHandler* owner);

            CARAPI Run();

        private:
            EventHandler* mOwner;
        };

    public:
        EventHandler(
            /* [in] */ AwSettings* owner);

        CARAPI_(void) BindUiThread();

        CARAPI_(void) RunOnUiThreadBlockingAndLocked(
            /* [in] */ IRunnable* r);

        CARAPI_(void) MaybePostOnUiThread(
            /* [in] */ IRunnable* r);

        CARAPI_(void) UpdateWebkitPreferencesLocked();

    private:
        AwSettings* mOwner;

        // Message id for running a Runnable with mAwSettingsLock held.
        static const Int32 RUN_RUNNABLE_BLOCKING = 0;
        // Actual UI thread handler
        AutoPtr<IHandler> mHandler;
        // Synchronization flag.
        Boolean mSynchronizationPending;
    };

public:
    // These constants must be kept in sync with the Android framework, defined in WebSettimgs.
    //@VisibleForTesting
    static const Int32 MIXED_CONTENT_ALWAYS_ALLOW = 0;
    //@VisibleForTesting
    static const Int32 MIXED_CONTENT_NEVER_ALLOW = 1;
    //@VisibleForTesting
    static const Int32 MIXED_CONTENT_COMPATIBILITY_MODE = 2;

public:
    AwSettings(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isAccessFromFileURLsGrantedByDefault,
        /* [in] */ Boolean supportsLegacyQuirks);

    CARAPI_(void) SetDIPScale(
        /* [in] */ Double dipScale);

    CARAPI_(void) SetZoomListener(
        /* [in] */ ZoomSupportChangeListener* zoomChangeListener);

    CARAPI_(void) SetWebContents(
        /* [in] */ Handle64 nativeWebContents);

    /**
     * See {@link android.webkit.WebSettings#setBlockNetworkLoads}.
     */
    CARAPI_(void) SetBlockNetworkLoads(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#getBlockNetworkLoads}.
     */
    CARAPI_(Boolean) GetBlockNetworkLoads();

    /**
     * Enable/disable third party cookies for an AwContents
     * @param accept true if we should accept third party cookies
     */
    CARAPI_(void) SetAcceptThirdPartyCookies(
        /* [in] */ Boolean accept);

    /**
     * Return whether third party cookies are enabled for an AwContents
     * @return true if accept third party cookies
     */
    CARAPI_(Boolean) GetAcceptThirdPartyCookies();

    /**
     * See {@link android.webkit.WebSettings#setAllowFileAccess}.
     */
    CARAPI_(void) SetAllowFileAccess(
        /* [in] */ Boolean allow);

    /**
     * See {@link android.webkit.WebSettings#getAllowFileAccess}.
     */
    CARAPI_(Boolean) GetAllowFileAccess();

    /**
     * See {@link android.webkit.WebSettings#setAllowContentAccess}.
     */
    CARAPI_(void) SetAllowContentAccess(
        /* [in] */ Boolean allow);

    /**
     * See {@link android.webkit.WebSettings#getAllowContentAccess}.
     */
    CARAPI_(Boolean) GetAllowContentAccess();

    /**
     * See {@link android.webkit.WebSettings#setCacheMode}.
     */
    CARAPI_(void) SetCacheMode(
        /* [in] */ Int32 mode);

    /**
     * See {@link android.webkit.WebSettings#getCacheMode}.
     */
    CARAPI_(Int32) GetCacheMode();

    /**
     * See {@link android.webkit.WebSettings#setNeedInitialFocus}.
     */
    CARAPI_(void) SetShouldFocusFirstNode(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebView#setInitialScale}.
     */
    CARAPI_(void) SetInitialPageScale(
        /* [in] */ Float scaleInPercent);

    CARAPI_(void) SetSpatialNavigationEnabled(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetEnableSupportedHardwareAcceleratedFeatures(
        /* [in] */ Boolean enable);

    CARAPI_(void) SetFullscreenSupported(
        /* [in] */ Boolean supported);

    /**
     * See {@link android.webkit.WebSettings#setNeedInitialFocus}.
     */
    CARAPI_(Boolean) ShouldFocusFirstNode();

    /**
     * See {@link android.webkit.WebSettings#setGeolocationEnabled}.
     */
    CARAPI_(void) SetGeolocationEnabled(
        /* [in] */ Boolean flag);

    /**
     * @return Returns if geolocation is currently enabled.
     */
    CARAPI_(Boolean) GetGeolocationEnabled();

    /**
     * See {@link android.webkit.WebSettings#setSaveFormData}.
     */
    CARAPI_(void) SetSaveFormData(
        /* [in] */ Boolean enable);

    /**
     * See {@link android.webkit.WebSettings#getSaveFormData}.
     */
    CARAPI_(Boolean) GetSaveFormData();

    /**
     * @returns the default User-Agent used by each ContentViewCore instance, i.e. unless
     * overridden by {@link #setUserAgentString()}
     */
    static CARAPI_(String) GetDefaultUserAgent();

    /**
     * See {@link android.webkit.WebSettings#setUserAgentString}.
     */
    CARAPI_(void) SetUserAgentString(
        /* [in] */ const String& ua);

    /**
     * See {@link android.webkit.WebSettings#getUserAgentString}.
     */
    CARAPI_(String) GetUserAgentString();

    /**
     * See {@link android.webkit.WebSettings#setLoadWithOverviewMode}.
     */
    CARAPI_(void) SetLoadWithOverviewMode(
        /* [in] */ Boolean overview);

    /**
     * See {@link android.webkit.WebSettings#getLoadWithOverviewMode}.
     */
    CARAPI_(Boolean) GetLoadWithOverviewMode();

    /**
     * See {@link android.webkit.WebSettings#setTextZoom}.
     */
    CARAPI_(void) SetTextZoom(
        /* [in] */ Int32 textZoom);

    /**
     * See {@link android.webkit.WebSettings#getTextZoom}.
     */
    CARAPI_(Int32) GetTextZoom();

    /**
     * See {@link android.webkit.WebSettings#setStandardFontFamily}.
     */
    CARAPI_(void) SetStandardFontFamily(
        /* [in] */ const String& font);

    /**
     * See {@link android.webkit.WebSettings#getStandardFontFamily}.
     */
    CARAPI_(String) GetStandardFontFamily();

    /**
     * See {@link android.webkit.WebSettings#setFixedFontFamily}.
     */
    CARAPI_(void) SetFixedFontFamily(
        /* [in] */ const String& font);

    /**
     * See {@link android.webkit.WebSettings#getFixedFontFamily}.
     */
    CARAPI_(String) GetFixedFontFamily();

    /**
     * See {@link android.webkit.WebSettings#setSansSerifFontFamily}.
     */
    CARAPI_(void) SetSansSerifFontFamily(
        /* [in] */ const String& font);

    /**
     * See {@link android.webkit.WebSettings#getSansSerifFontFamily}.
     */
    CARAPI_(String) GetSansSerifFontFamily();

    /**
     * See {@link android.webkit.WebSettings#setSerifFontFamily}.
     */
    CARAPI_(void) SetSerifFontFamily(
        /* [in] */ const String& font);

    /**
     * See {@link android.webkit.WebSettings#getSerifFontFamily}.
     */
    CARAPI_(String) GetSerifFontFamily();

    /**
     * See {@link android.webkit.WebSettings#setCursiveFontFamily}.
     */
    CARAPI_(void) SetCursiveFontFamily(
        /* [in] */ const String& font);

    /**
     * See {@link android.webkit.WebSettings#getCursiveFontFamily}.
     */
    CARAPI_(String) GetCursiveFontFamily();

    /**
     * See {@link android.webkit.WebSettings#setFantasyFontFamily}.
     */
    CARAPI_(void) SetFantasyFontFamily(
        /* [in] */ const String& font);

    /**
     * See {@link android.webkit.WebSettings#getFantasyFontFamily}.
     */
    CARAPI_(String) GetFantasyFontFamily();

    /**
     * See {@link android.webkit.WebSettings#setMinimumFontSize}.
     */
    CARAPI_(void) SetMinimumFontSize(
        /* [in] */ Int32 size);

    /**
     * See {@link android.webkit.WebSettings#getMinimumFontSize}.
     */
    CARAPI_(Int32) GetMinimumFontSize();

    /**
     * See {@link android.webkit.WebSettings#setMinimumLogicalFontSize}.
     */
    CARAPI_(void) SetMinimumLogicalFontSize(
        /* [in] */ Int32 size);

    /**
     * See {@link android.webkit.WebSettings#getMinimumLogicalFontSize}.
     */
    CARAPI_(Int32) GetMinimumLogicalFontSize();

    /**
     * See {@link android.webkit.WebSettings#setDefaultFontSize}.
     */
    CARAPI_(void) SetDefaultFontSize(
        /* [in] */ Int32 size);

    /**
     * See {@link android.webkit.WebSettings#getDefaultFontSize}.
     */
    CARAPI_(Int32) GetDefaultFontSize();

    /**
     * See {@link android.webkit.WebSettings#setDefaultFixedFontSize}.
     */
    CARAPI_(void) SetDefaultFixedFontSize(
        /* [in] */ Int32 size);

    /**
     * See {@link android.webkit.WebSettings#getDefaultFixedFontSize}.
     */
    CARAPI_(Int32) GetDefaultFixedFontSize();

    /**
     * See {@link android.webkit.WebSettings#setJavaScriptEnabled}.
     */
    CARAPI_(void) SetJavaScriptEnabled(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#setAllowUniversalAccessFromFileURLs}.
     */
    CARAPI_(void) SetAllowUniversalAccessFromFileURLs(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#setAllowFileAccessFromFileURLs}.
     */
    CARAPI_(void) SetAllowFileAccessFromFileURLs(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#setLoadsImagesAutomatically}.
     */
    CARAPI_(void) SetLoadsImagesAutomatically(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#getLoadsImagesAutomatically}.
     */
    CARAPI_(Boolean) GetLoadsImagesAutomatically();

    /**
     * See {@link android.webkit.WebSettings#setImagesEnabled}.
     */
    CARAPI_(void) SetImagesEnabled(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#getImagesEnabled}.
     */
    CARAPI_(Boolean) GetImagesEnabled();

    /**
     * See {@link android.webkit.WebSettings#getJavaScriptEnabled}.
     */
    CARAPI_(Boolean) GetJavaScriptEnabled();

    /**
     * See {@link android.webkit.WebSettings#getAllowUniversalAccessFromFileURLs}.
     */
    CARAPI_(Boolean) GetAllowUniversalAccessFromFileURLs();

    /**
     * See {@link android.webkit.WebSettings#getAllowFileAccessFromFileURLs}.
     */
    CARAPI_(Boolean) GetAllowFileAccessFromFileURLs();

    /**
     * See {@link android.webkit.WebSettings#setPluginsEnabled}.
     */
    CARAPI_(void) SetPluginsEnabled(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#setPluginState}.
     */
    CARAPI_(void) SetPluginState(
        /* [in] */ PluginState state);

    /**
     * See {@link android.webkit.WebSettings#getPluginsEnabled}.
     */
    CARAPI_(Boolean) GetPluginsEnabled();

    /**
     * See {@link android.webkit.WebSettings#getPluginState}.
     */
    CARAPI_(PluginState) GetPluginState();

    /**
     * See {@link android.webkit.WebSettings#setJavaScriptCanOpenWindowsAutomatically}.
     */
    CARAPI_(void) SetJavaScriptCanOpenWindowsAutomatically(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#getJavaScriptCanOpenWindowsAutomatically}.
     */
    CARAPI_(Boolean) GetJavaScriptCanOpenWindowsAutomatically();

    /**
     * See {@link android.webkit.WebSettings#setLayoutAlgorithm}.
     */
    CARAPI_(void) SetLayoutAlgorithm(
        /* [in] */ LayoutAlgorithm l);

    /**
     * See {@link android.webkit.WebSettings#getLayoutAlgorithm}.
     */
    CARAPI_(LayoutAlgorithm) GetLayoutAlgorithm();

    /**
     * See {@link android.webkit.WebSettings#setSupportMultipleWindows}.
     */
    CARAPI_(void) SetSupportMultipleWindows(
        /* [in] */ Boolean support);

    /**
     * See {@link android.webkit.WebSettings#supportMultipleWindows}.
     */
    CARAPI_(Boolean) SupportMultipleWindows();

    /**
     * See {@link android.webkit.WebSettings#setUseWideViewPort}.
     */
    CARAPI_(void) SetUseWideViewPort(
        /* [in] */ Boolean use);

    /**
     * See {@link android.webkit.WebSettings#getUseWideViewPort}.
     */
    CARAPI_(Boolean) GetUseWideViewPort();

    CARAPI_(void) SetZeroLayoutHeightDisablesViewportQuirk(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) GetZeroLayoutHeightDisablesViewportQuirk();

    CARAPI_(void) SetForceZeroLayoutHeight(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) GetForceZeroLayoutHeight();

    /**
     * See {@link android.webkit.WebSettings#setAppCacheEnabled}.
     */
    CARAPI_(void) SetAppCacheEnabled(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#setAppCachePath}.
     */
    CARAPI_(void) SetAppCachePath(
        /* [in] */ const String& path);

    /**
     * See {@link android.webkit.WebSettings#setDomStorageEnabled}.
     */
    CARAPI_(void) SetDomStorageEnabled(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#getDomStorageEnabled}.
     */
    CARAPI_(Boolean) GetDomStorageEnabled();

    /**
     * See {@link android.webkit.WebSettings#setDatabaseEnabled}.
     */
    CARAPI_(void) SetDatabaseEnabled(
        /* [in] */ Boolean flag);

    /**
     * See {@link android.webkit.WebSettings#getDatabaseEnabled}.
     */
    CARAPI_(Boolean) GetDatabaseEnabled();

    /**
     * See {@link android.webkit.WebSettings#setDefaultTextEncodingName}.
     */
    CARAPI_(void) SetDefaultTextEncodingName(
        /* [in] */ const String& encoding);

    /**
     * See {@link android.webkit.WebSettings#getDefaultTextEncodingName}.
     */
    CARAPI_(String) GetDefaultTextEncodingName();

    /**
     * See {@link android.webkit.WebSettings#setMediaPlaybackRequiresUserGesture}.
     */
    CARAPI_(void) SetMediaPlaybackRequiresUserGesture(
        /* [in] */ Boolean require);

    /**
     * See {@link android.webkit.WebSettings#getMediaPlaybackRequiresUserGesture}.
     */
    CARAPI_(Boolean) GetMediaPlaybackRequiresUserGesture();

    /**
     * See {@link android.webkit.WebSettings#setDefaultVideoPosterURL}.
     */
    CARAPI_(void) SetDefaultVideoPosterURL(const String& url);

    /**
     * See {@link android.webkit.WebSettings#getDefaultVideoPosterURL}.
     */
    CARAPI_(String) GetDefaultVideoPosterURL();

    /**
     * See {@link android.webkit.WebSettings#setSupportZoom}.
     */
    CARAPI_(void) SetSupportZoom(
        /* [in] */ Boolean support);

    /**
     * See {@link android.webkit.WebSettings#supportZoom}.
     */
    CARAPI_(Boolean) SupportZoom();

    /**
     * See {@link android.webkit.WebSettings#setBuiltInZoomControls}.
     */
    CARAPI_(void) SetBuiltInZoomControls(
        /* [in] */ Boolean enabled);

    /**
     * See {@link android.webkit.WebSettings#getBuiltInZoomControls}.
     */
    CARAPI_(Boolean) GetBuiltInZoomControls();

    /**
     * See {@link android.webkit.WebSettings#setDisplayZoomControls}.
     */
    CARAPI_(void) SetDisplayZoomControls(
        /* [in] */ Boolean enabled);

    /**
     * See {@link android.webkit.WebSettings#getDisplayZoomControls}.
     */
    CARAPI_(Boolean) GetDisplayZoomControls();

    CARAPI_(void) SetMixedContentMode(
        /* [in] */ Int32 mode);

    CARAPI_(Int32) GetMixedContentMode();

    /**
     * Sets whether to use the video overlay for the embedded video.
     * @param flag whether to enable the video overlay for the embedded video.
     */
    CARAPI_(void) SetVideoOverlayForEmbeddedVideoEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Gets whether to use the video overlay for the embedded video.
     * @return true if the WebView enables the video overlay for the embedded video.
     */
    CARAPI_(Boolean) GetVideoOverlayForEmbeddedVideoEnabled();

    CARAPI_(Boolean) SupportsMultiTouchZoom();

    CARAPI_(Boolean) ShouldDisplayZoomControls();

private:
    //@CalledByNative
    CARAPI_(void) NativeAwSettingsGone(
        /* [in] */ Handle64 nativeAwSettings);

    //@CalledByNative
    CARAPI_(Double) GetDIPScaleLocked();

    CARAPI_(void) UpdateEverythingLocked();

    //@CalledByNative
    CARAPI_(Float) GetInitialPageScalePercentLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetSpatialNavigationLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetEnableSupportedHardwareAcceleratedFeaturesLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetFullscreenSupportedLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetSaveFormDataLocked();

    //@CalledByNative
    CARAPI_(String) GetUserAgentLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetLoadWithOverviewModeLocked();

    //@CalledByNative
    CARAPI_(Int32) GetTextSizePercentLocked();

    //@CalledByNative
    CARAPI_(String) GetStandardFontFamilyLocked();

    //@CalledByNative
    CARAPI_(String) GetFixedFontFamilyLocked();

    //@CalledByNative
    CARAPI_(String) GetSansSerifFontFamilyLocked();

    //@CalledByNative
    CARAPI_(String) GetSerifFontFamilyLocked();

    //@CalledByNative
    CARAPI_(String) GetCursiveFontFamilyLocked();

    //@CalledByNative
    CARAPI_(String) GetFantasyFontFamilyLocked();

    //@CalledByNative
    CARAPI_(Int32) GetMinimumFontSizeLocked();

    //@CalledByNative
    CARAPI_(Int32) GetMinimumLogicalFontSizeLocked();

    //@CalledByNative
    CARAPI_(Int32) GetDefaultFontSizeLocked();

    //@CalledByNative
    CARAPI_(Int32) GetDefaultFixedFontSizeLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetLoadsImagesAutomaticallyLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetImagesEnabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetJavaScriptEnabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetAllowUniversalAccessFromFileURLsLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetAllowFileAccessFromFileURLsLocked();

    /**
     * Return true if plugins are disabled.
     * @return True if plugins are disabled.
     */
    //@CalledByNative
    CARAPI_(Boolean) GetPluginsDisabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetJavaScriptCanOpenWindowsAutomaticallyLocked();

    /**
     * Gets whether Text Auto-sizing layout algorithm is enabled.
     *
     * @return true if Text Auto-sizing layout algorithm is enabled
     */
    //@CalledByNative
    CARAPI_(Boolean) GetTextAutosizingEnabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetSupportMultipleWindowsLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetSupportLegacyQuirksLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetUseWideViewportLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetZeroLayoutHeightDisablesViewportQuirkLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetForceZeroLayoutHeightLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetPasswordEchoEnabledLocked();

    /**
     * Gets whether Application Cache is enabled.
     *
     * @return true if Application Cache is enabled
     */
    //@CalledByNative
    CARAPI_(Boolean) GetAppCacheEnabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetDomStorageEnabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetDatabaseEnabledLocked();

    //@CalledByNative
    CARAPI_(String) GetDefaultTextEncodingLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetMediaPlaybackRequiresUserGestureLocked();

    //@CalledByNative
    CARAPI_(String) GetDefaultVideoPosterURLLocked();

    CARAPI_(void) OnGestureZoomSupportChanged(
        /* [in] */ Boolean supportsDoubleTapZoom,
        /* [in] */ Boolean supportsMultiTouchZoom);

    //@CalledByNative
    CARAPI_(Boolean) GetAllowRunningInsecureContentLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetAllowDisplayingInsecureContentLocked();

    //@CalledByNative
    CARAPI_(Boolean) GetVideoOverlayForEmbeddedVideoEnabledLocked();

    //@CalledByNative
    CARAPI_(Boolean) SupportsDoubleTapZoomLocked();

    CARAPI_(Boolean) SupportsMultiTouchZoomLocked();

    CARAPI_(Int32) ClipFontSize(
        /* [in] */ Int32 size);

    //@CalledByNative
    CARAPI_(void) UpdateEverything();

    //@CalledByNative
    CARAPI_(void) PopulateWebPreferences(
        /* [in] */ Handle64 webPrefsPtr);

    CARAPI_(void) UpdateWebkitPreferencesOnUiThreadLocked();

    CARAPI_(Handle64) NativeInit(
        /* [in] */ Handle64 webContentsPtr);

    CARAPI_(void) NativeDestroy(
        /* [in] */ Handle64 nativeAwSettings);

    CARAPI_(void) NativePopulateWebPreferencesLocked(
        /* [in] */ Handle64 nativeAwSettings,
        /* [in] */ Handle64 webPrefsPtr);

    CARAPI_(void) NativeResetScrollAndScaleState(
        /* [in] */ Handle64 nativeAwSettings);

    CARAPI_(void) NativeUpdateEverythingLocked(
        /* [in] */ Handle64 nativeAwSettings);

    CARAPI_(void) NativeUpdateInitialPageScaleLocked(
        /* [in] */ Handle64 nativeAwSettings);

    CARAPI_(void) NativeUpdateUserAgentLocked(
        /* [in] */ Handle64 nativeAwSettings);

    CARAPI_(void) NativeUpdateWebkitPreferencesLocked(
        /* [in] */ Handle64 nativeAwSettings);

    static CARAPI_(String) NativeGetDefaultUserAgent();

    CARAPI_(void) NativeUpdateFormDataPreferencesLocked(
        /* [in] */ Handle64 nativeAwSettings);

    CARAPI_(void) NativeUpdateRendererPreferencesLocked(
        /* [in] */ Handle64 nativeAwSettings);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwSettingsCallback_Init();

private:
    static CARAPI_(void) NativeAwSettingsGone(
        /* [in] */ IInterface* obj,
        /* [in] */ Handle64 nativeAwSettings);

    static CARAPI_(Double) GetDIPScaleLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetInitialPageScalePercentLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetSpatialNavigationLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetEnableSupportedHardwareAcceleratedFeaturesLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetFullscreenSupportedLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetSaveFormDataLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetUserAgentLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetLoadWithOverviewModeLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetTextSizePercentLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetStandardFontFamilyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetFixedFontFamilyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetSansSerifFontFamilyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetSerifFontFamilyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetCursiveFontFamilyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetFantasyFontFamilyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetMinimumFontSizeLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetMinimumLogicalFontSizeLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetDefaultFontSizeLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetDefaultFixedFontSizeLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetLoadsImagesAutomaticallyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetImagesEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetJavaScriptEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetAllowUniversalAccessFromFileURLsLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetAllowFileAccessFromFileURLsLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetPluginsDisabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetJavaScriptCanOpenWindowsAutomaticallyLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetTextAutosizingEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetSupportMultipleWindowsLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetSupportLegacyQuirksLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetUseWideViewportLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetZeroLayoutHeightDisablesViewportQuirkLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetForceZeroLayoutHeightLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetPasswordEchoEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetAppCacheEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetDomStorageEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetDatabaseEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetDefaultTextEncodingLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetMediaPlaybackRequiresUserGestureLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(String) GetDefaultVideoPosterURLLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetAllowRunningInsecureContentLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetAllowDisplayingInsecureContentLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetVideoOverlayForEmbeddedVideoEnabledLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) SupportsDoubleTapZoomLocked(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) UpdateEverything(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) PopulateWebPreferences(
        /* [in] */ IInterface* obj,
        /* [in] */ Handle64 webPrefsPtr);

private:
    static const String TAG;

    // This class must be created on the UI thread. Afterwards, it can be
    // used from any thread. Internally, the class uses a message queue
    // to call native code on the UI thread only.

    // Values passed in on construction.
    Boolean mHasInternetPermission;

    AutoPtr<ZoomSupportChangeListener> mZoomChangeListener;
    Double mDIPScale;

    // Lock to protect all settings.
    Object mAwSettingsLock;

    LayoutAlgorithm mLayoutAlgorithm;
    Int32 mTextSizePercent;
    String mStandardFontFamily;
    String mFixedFontFamily;
    String mSansSerifFontFamily;
    String mSerifFontFamily;
    String mCursiveFontFamily;
    String mFantasyFontFamily;
    String mDefaultTextEncoding;
    String mUserAgent;
    Int32 mMinimumFontSize;
    Int32 mMinimumLogicalFontSize;
    Int32 mDefaultFontSize;
    Int32 mDefaultFixedFontSize;
    Boolean mLoadsImagesAutomatically;
    Boolean mImagesEnabled;
    Boolean mJavaScriptEnabled;
    Boolean mAllowUniversalAccessFromFileURLs;
    Boolean mAllowFileAccessFromFileURLs;
    Boolean mJavaScriptCanOpenWindowsAutomatically;
    Boolean mSupportMultipleWindows;
    PluginState mPluginState;
    Boolean mAppCacheEnabled;
    Boolean mDomStorageEnabled;
    Boolean mDatabaseEnabled;
    Boolean mUseWideViewport;
    Boolean mZeroLayoutHeightDisablesViewportQuirk;
    Boolean mForceZeroLayoutHeight;
    Boolean mLoadWithOverviewMode;
    Boolean mMediaPlaybackRequiresUserGesture;
    String mDefaultVideoPosterURL;
    Float mInitialPageScalePercent;
    Boolean mSpatialNavigationEnabled;  // Default depends on device features.
    Boolean mEnableSupportedHardwareAcceleratedFeatures;
    Int32 mMixedContentMode;
    Boolean mVideoOverlayForEmbeddedVideoEnabled;

    // Although this bit is stored on AwSettings it is actually controlled via the CookieManager.
    Boolean mAcceptThirdPartyCookies;

    Boolean mSupportLegacyQuirks;

    Boolean mPasswordEchoEnabled;

    // Not accessed by the native side.
    Boolean mBlockNetworkLoads;  // Default depends on permission of embedding APK.
    Boolean mAllowContentUrlAccess;
    Boolean mAllowFileUrlAccess;
    Int32 mCacheMode;
    Boolean mShouldFocusFirstNode;
    Boolean mGeolocationEnabled;
    Boolean mAutoCompleteEnabled;
    Boolean mFullscreenSupported;
    Boolean mSupportZoom;
    Boolean mBuiltInZoomControls;
    Boolean mDisplayZoomControls;

    // Protects access to settings global fields.
    static Object sGlobalContentSettingsLock;
    // For compatibility with the legacy WebView, we can only enable AppCache when the path is
    // provided. However, we don't use the path, so we just check if we have received it from the
    // client.
    static Boolean sAppCachePathIsSet;

    // The native side of this object. It's lifetime is bounded by the WebContent it is attached to.
    Handle64 mNativeAwSettings;

    // Custom handler that queues messages to call native code on the UI thread.
    AutoPtr<EventHandler> mEventHandler;

    static const Int32 MINIMUM_FONT_SIZE = 1;
    static const Int32 MAXIMUM_FONT_SIZE = 72;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWSETTINGS_H__
