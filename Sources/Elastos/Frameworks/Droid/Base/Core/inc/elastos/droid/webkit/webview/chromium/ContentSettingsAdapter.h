
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENTSETTINGSADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENTSETTINGSADAPTER_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwSettings.h"
#include "elastos/droid/webkit/WebSettings.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwSettings;
using Elastos::Droid::Webkit::LayoutAlgorithm;
using Elastos::Droid::Webkit::PluginState;
using Elastos::Droid::Webkit::RenderPriority;
using Elastos::Droid::Webkit::WebSettings;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class ContentSettingsAdapter
    : public WebSettings
{
public:
    ContentSettingsAdapter();

    ContentSettingsAdapter(
        /* [in] */ AwSettings* awSettings);

    virtual CARAPI GetAwSettings(
        /* [out] */ AwSettings** awSettings);

    // @Override
    // @Deprecated
    CARAPI SetNavDump(
        /* [in] */ Boolean enabled);

    // @Override
    // @Deprecated
    CARAPI GetNavDump(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetSupportZoom(
        /* [in] */ Boolean support);

    // @Override
    CARAPI SupportZoom(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetBuiltInZoomControls(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI GetBuiltInZoomControls(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetDisplayZoomControls(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI GetDisplayZoomControls(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetAllowFileAccess(
        /* [in] */ Boolean allow);

    // @Override
    CARAPI GetAllowFileAccess(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetAllowContentAccess(
        /* [in] */ Boolean allow);

    // @Override
    CARAPI GetAllowContentAccess(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetLoadWithOverviewMode(
        /* [in] */ Boolean overview);

    // @Override
    CARAPI GetLoadWithOverviewMode(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetAcceptThirdPartyCookies(
        /* [in] */ Boolean accept);

    // @Override
    CARAPI GetAcceptThirdPartyCookies(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetEnableSmoothTransition(
        /* [in] */ Boolean enable);

    // @Override
    CARAPI EnableSmoothTransition(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetUseWebViewBackgroundForOverscrollBackground(
        /* [in] */ Boolean view);

    // @Override
    CARAPI GetUseWebViewBackgroundForOverscrollBackground(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetSaveFormData(
        /* [in] */ Boolean save);

    // @Override
    CARAPI GetSaveFormData(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetSavePassword(
        /* [in] */ Boolean save);

    // @Override
    CARAPI GetSavePassword(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetTextZoom(
        /* [in] */ Int32 textZoom);

    // @Override
    // synchronized
    CARAPI GetTextZoom(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetDefaultZoom(
         /* [in] */ ZoomDensity zoom);

    // @Override
    CARAPI GetDefaultZoom(
        /* [out] */ ZoomDensity* result);

    // @Override
    CARAPI SetLightTouchEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI GetLightTouchEnabled(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetUserAgent(
        /* [in] */ Int32 ua);

    // @Override
    // synchronized
    CARAPI GetUserAgent(
        /* [out] */ Int32* result);

    // @Override
    // synchronized
    CARAPI SetUseWideViewPort(
        /* [in] */ Boolean use);

    // @Override
    // synchronized
    CARAPI GetUseWideViewPort(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetSupportMultipleWindows(
        /* [in] */ Boolean support);

    // @Override
    // synchronized
    CARAPI SupportMultipleWindows(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetLayoutAlgorithm(
        /* [in] */ LayoutAlgorithm la);

    // @Override
    CARAPI GetLayoutAlgorithm(
        /* [out] */ LayoutAlgorithm* result);

    // @Override
    // synchronized
    CARAPI SetStandardFontFamily(
        /* [in] */ const String& font);

    // @Override
    // synchronized
    CARAPI GetStandardFontFamily(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetFixedFontFamily(
        /* [in] */ const String& font);

    // @Override
    // synchronized
    CARAPI GetFixedFontFamily(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetSansSerifFontFamily(
        /* [in] */ const String& font);

    // @Override
    // synchronized
    CARAPI GetSansSerifFontFamily(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetSerifFontFamily(
        /* [in] */ const String& font);

    // @Override
    // synchronized
    CARAPI GetSerifFontFamily(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetCursiveFontFamily(
        /* [in] */ const String& font);

    // @Override
    // synchronized
    CARAPI GetCursiveFontFamily(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetFantasyFontFamily(
        /* [in] */ const String& font);

    // @Override
    // synchronized
    CARAPI GetFantasyFontFamily(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetMinimumFontSize(
        /* [in] */ Int32 size);

    // @Override
    // synchronized
    CARAPI GetMinimumFontSize(
        /* [out] */ Int32* result);

    // @Override
    // synchronized
    CARAPI SetMinimumLogicalFontSize(
        /* [in] */ Int32 size);

    // @Override
    // synchronized
    CARAPI GetMinimumLogicalFontSize(
        /* [out] */ Int32* result);

    // @Override
    // synchronized
    CARAPI SetDefaultFontSize(
        /* [in] */ Int32 size);

    // @Override
    // synchronized
    CARAPI GetDefaultFontSize(
        /* [out] */ Int32* result);

    // @Override
    // synchronized
    CARAPI SetDefaultFixedFontSize(
        /* [in] */ Int32 size);

    // @Override
    // synchronized
    CARAPI GetDefaultFixedFontSize(
        /* [out] */ Int32* result);

    // @Override
    // synchronized
    CARAPI SetLoadsImagesAutomatically(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI GetLoadsImagesAutomatically(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetBlockNetworkImage(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI GetBlockNetworkImage(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetBlockNetworkLoads(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI GetBlockNetworkLoads(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetJavaScriptEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI SetAllowUniversalAccessFromFileURLs(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI SetAllowFileAccessFromFileURLs(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI SetPluginsEnabled(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI SetPluginState(
        /* [in] */ PluginState state);

    // @Override
    // synchronized
    CARAPI SetDatabasePath(
        /* [in] */ const String& databasePath);

    // @Override
    // synchronized
    CARAPI SetGeolocationDatabasePath(
        /* [in] */ const String& databasePath);

    // @Override
    // synchronized
    CARAPI SetAppCacheEnabled(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI SetAppCachePath(
        /* [in] */ const String& appCachePath);

    // @Override
    // synchronized
    CARAPI SetAppCacheMaxSize(
        /* [in] */ Int64 appCacheMaxSize);

    // @Override
    // synchronized
    CARAPI SetDatabaseEnabled(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI SetDomStorageEnabled(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI GetDomStorageEnabled(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI GetDatabasePath(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI GetDatabaseEnabled(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetGeolocationEnabled(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI GetJavaScriptEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetAllowUniversalAccessFromFileURLs(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetAllowFileAccessFromFileURLs(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI GetPluginsEnabled(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI GetPluginState(
        /* [out] */ PluginState* result);

    // @Override
    // synchronized
    CARAPI SetJavaScriptCanOpenWindowsAutomatically(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI GetJavaScriptCanOpenWindowsAutomatically(
        /* [out] */ Boolean* result);

    // @Override
    // synchronized
    CARAPI SetDefaultTextEncodingName(
        /* [in] */ const String& encoding);

    // @Override
    // synchronized
    CARAPI GetDefaultTextEncodingName(
        /* [out] */ String* result);

    // @Override
    // synchronized
    CARAPI SetUserAgentString(
        /* [in] */ const String& ua);

    // @Override
    // synchronized
    CARAPI GetUserAgentString(
        /* [out] */ String* result);

    // @Override
    CARAPI SetNeedInitialFocus(
        /* [in] */ Boolean flag);

    // @Override
    // synchronized
    CARAPI SetRenderPriority(
        /* [in] */ RenderPriority priority);

    // @Override
    CARAPI SetCacheMode(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI GetCacheMode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetMediaPlaybackRequiresUserGesture(
        /* [in] */ Boolean require);

    // @Override
    CARAPI GetMediaPlaybackRequiresUserGesture(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetMixedContentMode(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI GetMixedContentMode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetVideoOverlayForEmbeddedEncryptedVideoEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI GetVideoOverlayForEmbeddedEncryptedVideoEnabled(
        /* [out] */ Boolean* result);

private:
    static const String LOGTAG;
    AutoPtr<AwSettings> mAwSettings;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENTSETTINGSADAPTER_H_

