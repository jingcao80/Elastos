
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

const String ContentSettingsAdapter::LOGTAG;// = ContentSettingsAdapter.class.getSimpleName();

ContentSettingsAdapter::ContentSettingsAdapter(
    /* [in] */ AwSettings* awSettings)
    : mAwSettings(awSettings)
{
}

AutoPtr<AwSettings> ContentSettingsAdapter::GetAwSettings()
{
    return mAwSettings;
}

//@Override
//@Deprecated
void ContentSettingsAdapter::SetNavDump(
    /* [in] */ Boolean enabled)
{
    // Intentional no-op.
}

//@Override
//@Deprecated
Boolean ContentSettingsAdapter::GetNavDump()
{
    // Intentional no-op.
    return FALSE;
}

//@Override
void ContentSettingsAdapter::SetSupportZoom(
    /* [in] */ Boolean support)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setSupportZoom=" + support);
    }

    mAwSettings->SetSupportZoom(support);
}

//@Override
Boolean ContentSettingsAdapter::SupportZoom()
{
    return mAwSettings->SupportZoom();
}

//@Override
void ContentSettingsAdapter::SetBuiltInZoomControls(
    /* [in] */ Boolean enabled)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setBuiltInZoomControls=" + enabled);
    }

    mAwSettings->SetBuiltInZoomControls(enabled);
}

//@Override
Boolean ContentSettingsAdapter::GetBuiltInZoomControls()
{
    return mAwSettings->GetBuiltInZoomControls();
}

//@Override
void ContentSettingsAdapter::SetDisplayZoomControls(
    /* [in] */ Boolean enabled)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setDisplayZoomControls=" + enabled);
    }

    mAwSettings->SetDisplayZoomControls(enabled);
}

//@Override
Boolean ContentSettingsAdapter::GetDisplayZoomControls()
{
    return mAwSettings->GetDisplayZoomControls();
}

//@Override
void ContentSettingsAdapter::SetAllowFileAccess(
    /* [in] */ Boolean allow)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAllowFileAccess=" + allow);
    }

    mAwSettings->SetAllowFileAccess(allow);
}

//@Override
Boolean ContentSettingsAdapter::GetAllowFileAccess()
{
    return mAwSettings->GetAllowFileAccess();
}

//@Override
void ContentSettingsAdapter::SetAllowContentAccess(
    /* [in] */ Boolean allow)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAllowContentAccess=" + allow);
    }

    mAwSettings->SetAllowContentAccess(allow);
}

//@Override
Boolean ContentSettingsAdapter::GetAllowContentAccess()
{
    return mAwSettings->GetAllowContentAccess();
}

//@Override
void ContentSettingsAdapter::SetLoadWithOverviewMode(
    /* [in] */ Boolean overview)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setLoadWithOverviewMode=" + overview);
    }

    mAwSettings->SetLoadWithOverviewMode(overview);
}

//@Override
Boolean ContentSettingsAdapter::GetLoadWithOverviewMode()
{
    return mAwSettings->GetLoadWithOverviewMode();
}

//@Override
void ContentSettingsAdapter::SetAcceptThirdPartyCookies(
    /* [in] */ Boolean accept)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAcceptThirdPartyCookies=" + accept);
    }

    mAwSettings->SetAcceptThirdPartyCookies(accept);
}

//@Override
Boolean ContentSettingsAdapter::GetAcceptThirdPartyCookies()
{
    return mAwSettings->GetAcceptThirdPartyCookies();
}

//@Override
void ContentSettingsAdapter::SetEnableSmoothTransition(
    /* [in] */ Boolean enable)
{
    // Intentional no-op.
}

//@Override
Boolean ContentSettingsAdapter::EnableSmoothTransition()
{
    // Intentional no-op.
    return FALSE;
}

//@Override
void ContentSettingsAdapter::SetUseWebViewBackgroundForOverscrollBackground(
    /* [in] */ Boolean view)
{
    // Intentional no-op.
}

//@Override
Boolean ContentSettingsAdapter::GetUseWebViewBackgroundForOverscrollBackground()
{
    // Intentional no-op.
    return FALSE;
}

//@Override
void ContentSettingsAdapter::SetSaveFormData(
    /* [in] */ Boolean save)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setSaveFormData=" + save);
    }

    mAwSettings->SetSaveFormData(save);
}

//@Override
Boolean ContentSettingsAdapter::GetSaveFormData()
{
    return mAwSettings->GetSaveFormData();
}

//@Override
void ContentSettingsAdapter::SetSavePassword(
    /* [in] */ Boolean save)
{
    // Intentional no-op.
}

//@Override
Boolean ContentSettingsAdapter::GetSavePassword()
{
    // Intentional no-op.
    return FALSE;
}

//@Override
//synchronized
void ContentSettingsAdapter::SetTextZoom(
    /* [in] */ Int32 textZoom)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setTextZoom=" + textZoom);
    }

    mAwSettings->SetTextZoom(textZoom);
}

//@Override
//synchronized
Int32 ContentSettingsAdapter::GetTextZoom()
{
    Object::Autolock lock(this);
    return mAwSettings->GetTextZoom();
}

//@Override
void ContentSettingsAdapter::SetDefaultZoom(
    /* [in] */ ZoomDensity zoom)
{
    if (zoom != ZoomDensity.MEDIUM) {
        Log.w(LOGTAG, "setDefaultZoom not supported, zoom=" + zoom);
    }
}

//@Override
ZoomDensity ContentSettingsAdapter::GetDefaultZoom()
{
    // Intentional no-op.
    return ZoomDensity.MEDIUM;
}

//@Override
void ContentSettingsAdapter::SetLightTouchEnabled(
    /* [in] */ Boolean enabled)
{
    // Intentional no-op.
}

//@Override
Boolean ContentSettingsAdapter::GetLightTouchEnabled()
{
    // Intentional no-op.
    return FALSE;
}

//@Override
//synchronized
void ContentSettingsAdapter::SetUserAgent(
    /* [in] */ Int32 ua)
{
    Object::Autolock lock(this);

    // Minimal implementation for backwards compatibility: just supports resetting to default.
    if (ua == 0) {
        SetUserAgentString(NULL);
    }
    else {
        Log.w(LOGTAG, "setUserAgent not supported, ua=" + ua);
    }
}

//@Override
//synchronized
Int32 ContentSettingsAdapter::GetUserAgent()
{
    Object::Autolock lock(this);

    // Minimal implementation for backwards compatibility: just identifies default vs custom.
    return AwSettings::GetDefaultUserAgent()->Equals(GetUserAgentString()) ? 0 : -1;
}

//@Override
//synchronized
void ContentSettingsAdapter::SetUseWideViewPort(
    /* [in] */ Boolean use)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setUseWideViewPort=" + use);
    }

    mAwSettings->SetUseWideViewPort(use);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetUseWideViewPort()
{
    Object::Autolock lock(this);
    return mAwSettings->GetUseWideViewPort();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetSupportMultipleWindows(
    /* [in] */ Boolean support)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setSupportMultipleWindows=" + support);
    }

    mAwSettings->SetSupportMultipleWindows(support);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::SupportMultipleWindows()
{
    Object::Autolock lock(this);
    return mAwSettings->SupportMultipleWindows();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetLayoutAlgorithm(
    /* [in] */ LayoutAlgorithm l)
{
    Object::Autolock lock(this);

    // TODO: Remove the upstream enum and mapping once the new value is in the public API.
    final AwSettings.LayoutAlgorithm[] chromiumValues = {
        AwSettings.LayoutAlgorithm.NORMAL,
        AwSettings.LayoutAlgorithm.SINGLE_COLUMN,
        AwSettings.LayoutAlgorithm.NARROW_COLUMNS,
        AwSettings.LayoutAlgorithm.TEXT_AUTOSIZING
    };
    mAwSettings.setLayoutAlgorithm(chromiumValues[l.ordinal()]);
}

//@Override
//synchronized
LayoutAlgorithm ContentSettingsAdapter::GetLayoutAlgorithm()
{
    Object::Autolock lock(this);

    // TODO: Remove the upstream enum and mapping once the new value is in the public API.
    final LayoutAlgorithm[] webViewValues = {
        LayoutAlgorithm.NORMAL,
        LayoutAlgorithm.SINGLE_COLUMN,
        LayoutAlgorithm.NARROW_COLUMNS,
        LayoutAlgorithm.TEXT_AUTOSIZING
    };
    return webViewValues[mAwSettings.getLayoutAlgorithm().ordinal()];
}

//@Override
//synchronized
void ContentSettingsAdapter::SetStandardFontFamily(
    /* [in] */ const String& font)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setStandardFontFamily=" + font);
    }

    mAwSettings->SetStandardFontFamily(font);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetStandardFontFamily()
{
    Object::Autolock lock(this);
    return mAwSettings->GetStandardFontFamily();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetFixedFontFamily(
    /* [in] */ const String& font)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setFixedFontFamily=" + font);
    }

    mAwSettings->SetFixedFontFamily(font);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetFixedFontFamily()
{
    Object::Autolock lock(this);
    return mAwSettings->GetFixedFontFamily();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetSansSerifFontFamily(
    /* [in] */ const String& font)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setSansSerifFontFamily=" + font);
    }

    mAwSettings->SetSansSerifFontFamily(font);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetSansSerifFontFamily()
{
    Object::Autolock lock(this);
    return mAwSettings->GetSansSerifFontFamily();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetSerifFontFamily(
    /* [in] */ const String& font)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setSerifFontFamily=" + font);
    }

    mAwSettings->SetSerifFontFamily(font);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetSerifFontFamily()
{
    Object::Autolock lock(this);
    return mAwSettings->GetSerifFontFamily();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetCursiveFontFamily(
    /* [in] */ const String& font)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setCursiveFontFamily=" + font);
    }

    mAwSettings->SetCursiveFontFamily(font);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetCursiveFontFamily()
{
    Object::Autolock lock(this);
    return mAwSettings->GetCursiveFontFamily();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetFantasyFontFamily(
    /* [in] */ const String& font)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setFantasyFontFamily=" + font);
    }

    mAwSettings->SetFantasyFontFamily(font);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetFantasyFontFamily()
{
    Object::Autolock lock(this);
    return mAwSettings->GetFantasyFontFamily();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetMinimumFontSize(
    /* [in] */ Int32 size)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setMinimumFontSize=" + size);
    }

    mAwSettings->SetMinimumFontSize(size);
}

//@Override
//synchronized
Int32 ContentSettingsAdapter::GetMinimumFontSize()
{
    Object::Autolock lock(this);
    return mAwSettings->GetMinimumFontSize();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetMinimumLogicalFontSize(
    /* [in] */ Int32 size)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setMinimumLogicalFontSize=" + size);
    }

    mAwSettings->SetMinimumLogicalFontSize(size);
}

//@Override
//synchronized
Int32 ContentSettingsAdapter::GetMinimumLogicalFontSize()
{
    Object::Autolock lock(this);
    return mAwSettings->GetMinimumLogicalFontSize();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetDefaultFontSize(
    /* [in] */ Int32 size)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setDefaultFontSize=" + size);
    }

    mAwSettings->SetDefaultFontSize(size);
}

//@Override
//synchronized
Int32 ContentSettingsAdapter::GetDefaultFontSize()
{
    Object::Autolock lock(this);
    return mAwSettings->GetDefaultFontSize();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetDefaultFixedFontSize(
    /* [in] */ Int32 size)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setDefaultFixedFontSize=" + size);
    }

    mAwSettings->SetDefaultFixedFontSize(size);
}

//@Override
//synchronized
Int32 ContentSettingsAdapter::GetDefaultFixedFontSize()
{
    Object::Autolock lock(this);
    return mAwSettings->GetDefaultFixedFontSize();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetLoadsImagesAutomatically(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setLoadsImagesAutomatically=" + flag);
    }

    mAwSettings->SetLoadsImagesAutomatically(flag);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetLoadsImagesAutomatically()
{
    Object::Autolock lock(this);
    return mAwSettings->GetLoadsImagesAutomatically();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetBlockNetworkImage(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setBlockNetworkImage=" + flag);
    }

    mAwSettings->SetImagesEnabled(!flag);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetBlockNetworkImage()
{
    Object::Autolock lock(this);
    return !mAwSettings->GetImagesEnabled();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetBlockNetworkLoads(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setBlockNetworkLoads=" + flag);
    }

    mAwSettings->SetBlockNetworkLoads(flag);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetBlockNetworkLoads()
{
    Object::Autolock lock(this);
    return mAwSettings->GetBlockNetworkLoads();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetJavaScriptEnabled(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setJavaScriptEnabled=" + flag);
    }

    mAwSettings->SetJavaScriptEnabled(flag);
}

//@Override
void ContentSettingsAdapter::SetAllowUniversalAccessFromFileURLs(
    /* [in] */ Boolean flag)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAllowUniversalAccessFromFileURLs=" + flag);
    }

    mAwSettings->SetAllowUniversalAccessFromFileURLs(flag);
}

//@Override
void ContentSettingsAdapter::SetAllowFileAccessFromFileURLs(
    /* [in] */ Boolean flag)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAllowFileAccessFromFileURLs=" + flag);
    }

    mAwSettings->SetAllowFileAccessFromFileURLs(flag);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetPluginsEnabled(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setPluginsEnabled=" + flag);
    }

    mAwSettings->SetPluginsEnabled(flag);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetPluginState(
    /* [in] */ PluginState state)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setPluginState=" + state);
    }

    mAwSettings->SetPluginState(state);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetDatabasePath(
    /* [in] */ const String& databasePath)
{
    Object::Autolock lock(this);
    // Intentional no-op.
}

//@Override
//synchronized
void ContentSettingsAdapter::SetGeolocationDatabasePath(
    /* [in] */ const String& databasePath)
{
    Object::Autolock lock(this);
    // Intentional no-op.
}

//@Override
//synchronized
void ContentSettingsAdapter::SetAppCacheEnabled(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAppCacheEnabled=" + flag);
    }

    mAwSettings->SetAppCacheEnabled(flag);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetAppCachePath(
    /* [in] */ const String& appCachePath)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setAppCachePath=" + appCachePath);
    }

    mAwSettings->SetAppCachePath(appCachePath);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetAppCacheMaxSize(
    /* [in] */ Int64 appCacheMaxSize)
{
    Object::Autolock lock(this);
    // Intentional no-op.
}

//@Override
//synchronized
void ContentSettingsAdapter::SetDatabaseEnabled(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setDatabaseEnabled=" + flag);
    }

    mAwSettings->SetDatabaseEnabled(flag);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetDomStorageEnabled(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setDomStorageEnabled=" + flag);
    }

    mAwSettings.setDomStorageEnabled(flag);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetDomStorageEnabled()
{
    Object::Autolock lock(this);
    return mAwSettings->GetDomStorageEnabled();
}

//@Override
//synchronized
String ContentSettingsAdapter::GetDatabasePath()
{
    Object::Autolock lock(this);
    // Intentional no-op.
    return String("");
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetDatabaseEnabled()
{
    Object::Autolock lock(this);
    return mAwSettings->GetDatabaseEnabled();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetGeolocationEnabled(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setGeolocationEnabled=" + flag);
    }

    mAwSettings->SetGeolocationEnabled(flag);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetJavaScriptEnabled()
{
    Object::Autolock lock(this);
    return mAwSettings->GetJavaScriptEnabled();
}

//@Override
Boolean ContentSettingsAdapter::GetAllowUniversalAccessFromFileURLs()
{
    return mAwSettings->GetAllowUniversalAccessFromFileURLs();
}

//@Override
Boolean ContentSettingsAdapter::GetAllowFileAccessFromFileURLs()
{
    return mAwSettings->GetAllowFileAccessFromFileURLs();
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetPluginsEnabled()
{
    Object::Autolock lock(this);
    return mAwSettings->GetPluginsEnabled();
}

//@Override
//synchronized
PluginState ContentSettingsAdapter::GetPluginState()
{
    Object::Autolock lock(this);
    return mAwSettings->GetPluginState();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetJavaScriptCanOpenWindowsAutomatically(
    /* [in] */ Boolean flag)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setJavaScriptCanOpenWindowsAutomatically=" + flag);
    }

    mAwSettings->SetJavaScriptCanOpenWindowsAutomatically(flag);
}

//@Override
//synchronized
Boolean ContentSettingsAdapter::GetJavaScriptCanOpenWindowsAutomatically()
{
    Object::Autolock lock(this);
    return mAwSettings->GetJavaScriptCanOpenWindowsAutomatically();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetDefaultTextEncodingName(
    /* [in] */ const String& encoding)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setDefaultTextEncodingName=" + encoding);
    }

    mAwSettings->SetDefaultTextEncodingName(encoding);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetDefaultTextEncodingName()
{
    Object::Autolock lock(this);
    return mAwSettings->GetDefaultTextEncodingName();
}

//@Override
//synchronized
void ContentSettingsAdapter::SetUserAgentString(
    /* [in] */ const String& ua)
{
    Object::Autolock lock(this);

    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setUserAgentString=" + ua);
    }

    mAwSettings->SetUserAgentString(ua);
}

//@Override
//synchronized
String ContentSettingsAdapter::GetUserAgentString()
{
    Object::Autolock lock(this);
    return mAwSettings->GetUserAgentString();
}

//@Override
void ContentSettingsAdapter::SetNeedInitialFocus(
    /* [in] */ Boolean flag)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setNeedInitialFocus=" + flag);
    }

    mAwSettings->SetShouldFocusFirstNode(flag);
}

//@Override
//synchronized
void ContentSettingsAdapter::SetRenderPriority(
    /* [in] */ RenderPriority priority)
{
    Object::Autolock lock(this);
    // Intentional no-op.
}

//@Override
void ContentSettingsAdapter::SetCacheMode(
    /* [in] */ Int32 mode)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setCacheMode=" + mode);
    }

    mAwSettings->SetCacheMode(mode);
}

//@Override
Int32 ContentSettingsAdapter::GetCacheMode()
{
    return mAwSettings->GetCacheMode();
}

//@Override
void ContentSettingsAdapter::SetMediaPlaybackRequiresUserGesture(
    /* [in] */ Boolean require)
{
    if (DebugFlags::TRACE_API) {
        Log.d(LOGTAG, "setMediaPlaybackRequiresUserGesture=" + require);
    }

    mAwSettings->SetMediaPlaybackRequiresUserGesture(require);
}

//@Override
Boolean ContentSettingsAdapter::GetMediaPlaybackRequiresUserGesture()
{
    return mAwSettings->GetMediaPlaybackRequiresUserGesture();
}

//    @Override
void ContentSettingsAdapter::SetMixedContentMode(
    /* [in] */ Int32 mode)
{
    mAwSettings->SetMixedContentMode(mode);
}

//    @Override
Int32 ContentSettingsAdapter::GetMixedContentMode()
{
    return mAwSettings->GetMixedContentMode();
}

//    @Override
void ContentSettingsAdapter::SetVideoOverlayForEmbeddedEncryptedVideoEnabled(
    /* [in] */ Boolean flag)
{
    mAwSettings->SetVideoOverlayForEmbeddedVideoEnabled(flag);
}

//    @Override
Boolean ContentSettingsAdapter::GetVideoOverlayForEmbeddedEncryptedVideoEnabled()
{
    return mAwSettings->GetVideoOverlayForEmbeddedVideoEnabled();
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
