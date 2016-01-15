/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_CONTENTSETTINGSADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_CONTENTSETTINGSADAPTER_H__

// package com.android.webview.chromium;

// import android.util.Log;
// import android.webkit.DebugFlags;
// import android.webkit.WebSettings.LayoutAlgorithm;
// import android.webkit.WebSettings.PluginState;
// import android.webkit.WebSettings.RenderPriority;
// import android.webkit.WebSettings.TextSize;
// import android.webkit.WebSettings.ZoomDensity;

// import org.chromium.android_webview.AwSettings;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

class ContentSettingsAdapter
    : public Object
    , public WebSettings
{
public:
    ContentSettingsAdapter(
        /* [in] */ AwSettings* awSettings);

    CARAPI_(AutoPtr<AwSettings>) GetAwSettings();

    //@Override
    //@Deprecated
    CARAPI_(void) SetNavDump(
        /* [in] */ Boolean enabled);

    //@Override
    //@Deprecated
    CARAPI_(Boolean) GetNavDump();

    //@Override
    CARAPI_(void) SetSupportZoom(
        /* [in] */ Boolean support);

    //@Override
    CARAPI_(Boolean) SupportZoom();

    //@Override
    CARAPI_(void) SetBuiltInZoomControls(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI_(Boolean) GetBuiltInZoomControls();

    //@Override
    CARAPI_(void) SetDisplayZoomControls(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI_(Boolean) GetDisplayZoomControls();

    //@Override
    CARAPI_(void) SetAllowFileAccess(
        /* [in] */ Boolean allow);

    //@Override
    CARAPI_(Boolean) GetAllowFileAccess();

    //@Override
    CARAPI_(void) SetAllowContentAccess(
        /* [in] */ Boolean allow);

    //@Override
    CARAPI_(Boolean) GetAllowContentAccess();

    //@Override
    CARAPI_(void) SetLoadWithOverviewMode(
        /* [in] */ Boolean overview);

    //@Override
    CARAPI_(Boolean) GetLoadWithOverviewMode();

    //@Override
    CARAPI_(void) SetAcceptThirdPartyCookies(
        /* [in] */ Boolean accept);

    //@Override
    CARAPI_(Boolean) GetAcceptThirdPartyCookies();

    //@Override
    CARAPI_(void) SetEnableSmoothTransition(
        /* [in] */ Boolean enable);

    //@Override
    CARAPI_(Boolean) EnableSmoothTransition();

    //@Override
    CARAPI_(void) SetUseWebViewBackgroundForOverscrollBackground(
        /* [in] */ Boolean view);

    //@Override
    CARAPI_(Boolean) GetUseWebViewBackgroundForOverscrollBackground();

    //@Override
    CARAPI_(void) SetSaveFormData(
        /* [in] */ Boolean save);

    //@Override
    CARAPI_(Boolean) GetSaveFormData();

    //@Override
    CARAPI_(void) SetSavePassword(
        /* [in] */ Boolean save);

    //@Override
    CARAPI_(Boolean) GetSavePassword();

    //@Override
    //synchronized
    CARAPI_(void) SetTextZoom(
        /* [in] */ Int32 textZoom);

    //@Override
    //synchronized
    CARAPI_(Int32) GetTextZoom();

    //@Override
    CARAPI_(void) SetDefaultZoom(
        /* [in] */ ZoomDensity zoom);

    //@Override
    CARAPI_(ZoomDensity) GetDefaultZoom();

    //@Override
    CARAPI_(void) SetLightTouchEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI_(Boolean) GetLightTouchEnabled();

    //@Override
    //synchronized
    CARAPI_(void) SetUserAgent(
        /* [in] */ Int32 ua);

    //@Override
    //synchronized
    CARAPI_(Int32) GetUserAgent();

    //@Override
    //synchronized
    CARAPI_(void) SetUseWideViewPort(
        /* [in] */ Boolean use);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetUseWideViewPort();

    //@Override
    //synchronized
    CARAPI_(void) SetSupportMultipleWindows(
        /* [in] */ Boolean support);

    //@Override
    //synchronized
    CARAPI_(Boolean) SupportMultipleWindows();

    //@Override
    //synchronized
    CARAPI_(void) SetLayoutAlgorithm(
        /* [in] */ LayoutAlgorithm l);

    //@Override
    //synchronized
    CARAPI_(LayoutAlgorithm) GetLayoutAlgorithm();

    //@Override
    //synchronized
    CARAPI_(void) SetStandardFontFamily(
        /* [in] */ const String& font);

    //@Override
    //synchronized
    CARAPI_(String) GetStandardFontFamily();

    //@Override
    //synchronized
    CARAPI_(void) SetFixedFontFamily(
        /* [in] */ const String& font);

    //@Override
    //synchronized
    CARAPI_(String) GetFixedFontFamily();

    //@Override
    //synchronized
    CARAPI_(void) SetSansSerifFontFamily(
        /* [in] */ const String& font);

    //@Override
    //synchronized
    CARAPI_(String) GetSansSerifFontFamily();

    //@Override
    //synchronized
    CARAPI_(void) SetSerifFontFamily(
        /* [in] */ const String& font);

    //@Override
    //synchronized
    CARAPI_(String) GetSerifFontFamily();

    //@Override
    //synchronized
    CARAPI_(void) SetCursiveFontFamily(
        /* [in] */ const String& font);

    //@Override
    //synchronized
    CARAPI_(String) GetCursiveFontFamily();

    //@Override
    //synchronized
    CARAPI_(void) SetFantasyFontFamily(
        /* [in] */ const String& font);

    //@Override
    //synchronized
    CARAPI_(String) GetFantasyFontFamily();

    //@Override
    //synchronized
    CARAPI_(void) SetMinimumFontSize(
        /* [in] */ Int32 size);

    //@Override
    //synchronized
    CARAPI_(Int32) GetMinimumFontSize();

    //@Override
    //synchronized
    CARAPI_(void) SetMinimumLogicalFontSize(
        /* [in] */ Int32 size);

    //@Override
    //synchronized
    CARAPI_(Int32) GetMinimumLogicalFontSize();

    //@Override
    //synchronized
    CARAPI_(void) SetDefaultFontSize(
        /* [in] */ Int32 size);

    //@Override
    //synchronized
    CARAPI_(Int32) GetDefaultFontSize();

    //@Override
    //synchronized
    CARAPI_(void) SetDefaultFixedFontSize(
        /* [in] */ Int32 size);

    //@Override
    //synchronized
    CARAPI_(Int32) GetDefaultFixedFontSize();

    //@Override
    //synchronized
    CARAPI_(void) SetLoadsImagesAutomatically(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetLoadsImagesAutomatically();

    //@Override
    //synchronized
    CARAPI_(void) SetBlockNetworkImage(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetBlockNetworkImage();

    //@Override
    //synchronized
    CARAPI_(void) SetBlockNetworkLoads(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetBlockNetworkLoads();

    //@Override
    //synchronized
    CARAPI_(void) SetJavaScriptEnabled(
        /* [in] */ Boolean flag);

    //@Override
    CARAPI_(void) SetAllowUniversalAccessFromFileURLs(
        /* [in] */ Boolean flag);

    //@Override
    CARAPI_(void) SetAllowFileAccessFromFileURLs(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(void) SetPluginsEnabled(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(void) SetPluginState(
        /* [in] */ PluginState state);

    //@Override
    //synchronized
    CARAPI_(void) SetDatabasePath(
        /* [in] */ const String& databasePath);

    //@Override
    //synchronized
    CARAPI_(void) SetGeolocationDatabasePath(
        /* [in] */ const String& databasePath);

    //@Override
    //synchronized
    CARAPI_(void) SetAppCacheEnabled(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(void) SetAppCachePath(
        /* [in] */ const String& appCachePath);

    //@Override
    //synchronized
    CARAPI_(void) SetAppCacheMaxSize(
        /* [in] */ Int64 appCacheMaxSize);

    //@Override
    //synchronized
    CARAPI_(void) SetDatabaseEnabled(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(void) SetDomStorageEnabled(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetDomStorageEnabled();

    //@Override
    //synchronized
    CARAPI_(String) GetDatabasePath();

    //@Override
    //synchronized
    CARAPI_(Boolean) GetDatabaseEnabled();

    //@Override
    //synchronized
    CARAPI_(void) SetGeolocationEnabled(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetJavaScriptEnabled();

    //@Override
    CARAPI_(Boolean) GetAllowUniversalAccessFromFileURLs();

    //@Override
    CARAPI_(Boolean) GetAllowFileAccessFromFileURLs();

    //@Override
    //synchronized
    CARAPI_(Boolean) GetPluginsEnabled();

    //@Override
    //synchronized
    CARAPI_(PluginState) GetPluginState();

    //@Override
    //synchronized
    CARAPI_(void) SetJavaScriptCanOpenWindowsAutomatically(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(Boolean) GetJavaScriptCanOpenWindowsAutomatically();

    //@Override
    //synchronized
    CARAPI_(void) SetDefaultTextEncodingName(
        /* [in] */ const String& encoding);

    //@Override
    //synchronized
    CARAPI_(String) GetDefaultTextEncodingName();

    //@Override
    //synchronized
    CARAPI_(void) SetUserAgentString(
        /* [in] */ const String& ua);

    //@Override
    //synchronized
    CARAPI_(String) GetUserAgentString();

    //@Override
    CARAPI_(void) SetNeedInitialFocus(
        /* [in] */ Boolean flag);

    //@Override
    //synchronized
    CARAPI_(void) SetRenderPriority(
        /* [in] */ RenderPriority priority);

    //@Override
    CARAPI_(void) SetCacheMode(
        /* [in] */ Int32 mode);

    //@Override
    CARAPI_(Int32) GetCacheMode();

    //@Override
    CARAPI_(void) SetMediaPlaybackRequiresUserGesture(
        /* [in] */ Boolean require);

    //@Override
    CARAPI_(Boolean) GetMediaPlaybackRequiresUserGesture();

    //@Override
    CARAPI_(void) SetMixedContentMode(
        /* [in] */ Int32 mode);

    //@Override
    CARAPI_(Int32) GetMixedContentMode();

    //@Override
    CARAPI_(void) SetVideoOverlayForEmbeddedEncryptedVideoEnabled(
        /* [in] */ Boolean flag);

    //@Override
    CARAPI_(Boolean) GetVideoOverlayForEmbeddedEncryptedVideoEnabled();

private:
    static const String LOGTAG;
    AutoPtr<AwSettings> mAwSettings;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_CONTENTSETTINGSADAPTER_H__
