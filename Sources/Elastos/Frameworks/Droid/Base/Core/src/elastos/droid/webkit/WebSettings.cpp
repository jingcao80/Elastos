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

#include "elastos/droid/webkit/WebSettings.h"
#include "elastos/droid/webkit/WebViewFactory.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(WebSettings, Object, IWebSettings);

WebSettings::WebSettings()
{
}

WebSettings::~WebSettings()
{
}

String WebSettings::GetDefaultUserAgent(
    /* [in] */ IContext* context)
{
    AutoPtr<IWebViewFactoryProviderStatics> stat;
    WebViewFactory::GetProvider()->GetStatics((IWebViewFactoryProviderStatics**)&stat);
    String userAgent;
    stat->GetDefaultUserAgent(context, &userAgent);
    return userAgent;
}


/**
 * Enables dumping the pages navigation cache to a text file. The default
 * is false.
 *
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::SetNavDump(
    /* [in] */ Boolean enabled)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether dumping the navigation cache is enabled.
 *
 * @return whether dumping the navigation cache is enabled
 * @see #setNavDump
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::GetNavDump(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should support zooming using its on-screen zoom
 * controls and gestures. The particular zoom mechanisms that should be used
 * can be set with {@link #setBuiltInZoomControls}. This setting does not
 * affect zooming performed using the {@link WebView#zoomIn()} and
 * {@link WebView#zoomOut()} methods. The default is true.
 *
 * @param support whether the WebView should support zoom
 */
ECode WebSettings::SetSupportZoom(
    /* [in] */ Boolean support)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView supports zoom.
 *
 * @return true if the WebView supports zoom
 * @see #setSupportZoom
 */
ECode WebSettings::SupportZoom(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView requires a user gesture to play media.
 * The default is true.
 *
 * @param require whether the WebView requires a user gesture to play media
 */
ECode WebSettings::SetMediaPlaybackRequiresUserGesture(
    /* [in] */ Boolean require)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView requires a user gesture to play media.
 *
 * @return true if the WebView requires a user gesture to play media
 * @see #setMediaPlaybackRequiresUserGesture
 */
ECode WebSettings::GetMediaPlaybackRequiresUserGesture(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should use its built-in zoom mechanisms. The
 * built-in zoom mechanisms comprise on-screen zoom controls, which are
 * displayed over the WebView's content, and the use of a pinch gesture to
 * control zooming. Whether or not these on-screen controls are displayed
 * can be set with {@link #setDisplayZoomControls}. The default is false.
 * <p>
 * The built-in mechanisms are the only currently supported zoom
 * mechanisms, so it is recommended that this setting is always enabled.
 *
 * @param enabled whether the WebView should use its built-in zoom mechanisms
 */
// This method was intended to select between the built-in zoom mechanisms
// and the separate zoom controls. The latter were obtained using
// {@link WebView#getZoomControls}, which is now hidden.
ECode WebSettings::SetBuiltInZoomControls(
    /* [in] */ Boolean enabled)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the zoom mechanisms built into WebView are being used.
 *
 * @return true if the zoom mechanisms built into WebView are being used
 * @see #setBuiltInZoomControls
 */
ECode WebSettings::GetBuiltInZoomControls(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should display on-screen zoom controls when
 * using the built-in zoom mechanisms. See {@link #setBuiltInZoomControls}.
 * The default is true.
 *
 * @param enabled whether the WebView should display on-screen zoom controls
 */
ECode WebSettings::SetDisplayZoomControls(
    /* [in] */ Boolean enabled)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView displays on-screen zoom controls when using
 * the built-in zoom mechanisms.
 *
 * @return true if the WebView displays on-screen zoom controls when using
 *         the built-in zoom mechanisms
 * @see #setDisplayZoomControls
 */
ECode WebSettings::GetDisplayZoomControls(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Enables or disables file access within WebView. File access is enabled by
 * default.  Note that this enables or disables file system access only.
 * Assets and resources are still accessible using file:///android_asset and
 * file:///android_res.
 */
ECode WebSettings::SetAllowFileAccess(
    /* [in] */ Boolean allow)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether this WebView supports file access.
 *
 * @see #setAllowFileAccess
 */
ECode WebSettings::GetAllowFileAccess(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Enables or disables content URL access within WebView.  Content URL
 * access allows WebView to load content from a content provider installed
 * in the system. The default is enabled.
 */
ECode WebSettings::SetAllowContentAccess(
    /* [in] */ Boolean allow)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether this WebView supports content URL access.
 *
 * @see #setAllowContentAccess
 */
ECode WebSettings::GetAllowContentAccess(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView loads pages in overview mode. The default is
 * false.
 */
ECode WebSettings::SetLoadWithOverviewMode(
    /* [in] */ Boolean overview)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether this WebView loads pages in overview mode.
 *
 * @return whether this WebView loads pages in overview mode
 * @see #setLoadWithOverviewMode
 */
ECode WebSettings::GetLoadWithOverviewMode(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView will enable smooth transition while panning or
 * zooming or while the window hosting the WebView does not have focus.
 * If it is true, WebView will choose a solution to maximize the performance.
 * e.g. the WebView's content may not be updated during the transition.
 * If it is false, WebView will keep its fidelity. The default value is false.
 *
 * @deprecated This method is now obsolete, and will become a no-op in future.
 */
ECode WebSettings::SetEnableSmoothTransition(
    /* [in] */ Boolean enable)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView enables smooth transition while panning or
 * zooming.
 *
 * @see #setEnableSmoothTransition
 *
 * @deprecated This method is now obsolete, and will become a no-op in future.
 */
ECode WebSettings::EnableSmoothTransition(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView uses its background for over scroll background.
 * If true, it will use the WebView's background. If false, it will use an
 * internal pattern. Default is true.
 *
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::SetUseWebViewBackgroundForOverscrollBackground(
    /* [in] */ Boolean view)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether this WebView uses WebView's background instead of
 * internal pattern for over scroll background.
 *
 * @see #setUseWebViewBackgroundForOverscrollBackground
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::GetUseWebViewBackgroundForOverscrollBackground(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should save form data. The default is true,
 * unless in private browsing mode, when the value is always false.
 */
ECode WebSettings::SetSaveFormData(
    /* [in] */ Boolean save)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView saves form data. Always false in private
 * browsing mode.
 *
 * @return whether the WebView saves form data
 * @see #setSaveFormData
 */
ECode WebSettings::GetSaveFormData(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should save passwords. The default is true.
 */
ECode WebSettings::SetSavePassword(
    /* [in] */ Boolean save)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView saves passwords.
 *
 * @return whether the WebView saves passwords
 * @see #setSavePassword
 */
ECode WebSettings::GetSavePassword(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the text zoom of the page in percent. The default is 100.
 *
 * @param textZoom the text zoom in percent
 */
ECode WebSettings::SetTextZoom(
    /* [in] */ Int32 textZoom)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the text zoom of the page in percent.
 *
 * @return the text zoom of the page in percent
 * @see #setTextZoom
 */
ECode WebSettings::GetTextZoom(
    /* [out] */ Int32* textZoom)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets policy for third party cookies.
 * Developers should access this via {@link CookieManager#setShouldAcceptThirdPartyCookies}.
 * @hide Internal API.
 */
ECode WebSettings::SetAcceptThirdPartyCookies(
    /* [in] */ Boolean accept)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets policy for third party cookies.
 * Developers should access this via {@link CookieManager#getShouldAcceptThirdPartyCookies}.
 * @hide Internal API
 */
ECode WebSettings::GetAcceptThirdPartyCookies(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the text size of the page. The default is {@link TextSize#NORMAL}.
 *
 * @param t the text size as a {@link TextSize} value
 * @deprecated Use {@link #setTextZoom} instead.
 */
ECode WebSettings::SetTextSize(
    /* [in] */ TextSize t)
{
    return SetTextZoom((Int32)t);
}

/**
 * Gets the text size of the page. If the text size was previously specified
 * in percent using {@link #setTextZoom}, this will return the closest
 * matching {@link TextSize}.
 *
 * @return the text size as a {@link TextSize} value
 * @see #setTextSize
 * @deprecated Use {@link #getTextZoom} instead.
 */
ECode WebSettings::GetTextSize(
    /* [out] */ TextSize* sizeOut)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(sizeOut);
    TextSize closestSize = -1;
    Int32 smallestDelta = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 textSize;
    GetTextZoom(&textSize);
    Int32 TEXT_SZIE[5] = { TextSize_SMALLEST, TextSize_SMALLER, TextSize_NORMAL,
            TextSize_LARGER, TextSize_LARGEST };
    for (Int32 i = 0; i < 5; i++) {
        TextSize size = TEXT_SZIE[i];
        Int32 delta = Elastos::Core::Math::Abs(textSize - size);
        if (delta == 0) {
            *sizeOut = size;
            return NOERROR;
        }
        if (delta < smallestDelta) {
            smallestDelta = delta;
            closestSize = size;
        }
    }

    *sizeOut = closestSize != -1 ? closestSize : TextSize_NORMAL;

    return NOERROR;
}


/**
 * Sets the default zoom density of the page. This must be called from the UI
 * thread. The default is {@link ZoomDensity#MEDIUM}.
 *
 * @param zoom the zoom density
 */
ECode WebSettings::SetDefaultZoom(
    /* [in] */ ZoomDensity zoom)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the default zoom density of the page. This should be called from
 * the UI thread.
 *
 * @return the zoom density
 * @see #setDefaultZoom
 */
ECode WebSettings::GetDefaultZoom(
    /* [out] */ ZoomDensity* zoom)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Enables using light touches to make a selection and activate mouseovers.
 * The default is false.
 */
ECode WebSettings::SetLightTouchEnabled(
    /* [in] */ Boolean enabled)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether light touches are enabled.
 *
 * @return whether light touches are enabled
 * @see #setLightTouchEnabled
 */
ECode WebSettings::GetLightTouchEnabled(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Controlled a rendering optimization that is no longer present. Setting
 * it now has no effect.
 *
 * @deprecated This setting now has no effect.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::SetUseDoubleTree(
    /* [in] */ Boolean use)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Controlled a rendering optimization that is no longer present. Setting
 * it now has no effect.
 *
 * @deprecated This setting now has no effect.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::GetUseDoubleTree(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Returns false unconditionally, so no need for derived classes to override.
    *result = FALSE;
    return NOERROR;
}

/**
 * Sets the user-agent string using an integer code.
 * <ul>
 *   <li>0 means the WebView should use an Android user-agent string</li>
 *   <li>1 means the WebView should use a desktop user-agent string</li>
 * </ul>
 * Other values are ignored. The default is an Android user-agent string,
 * i.e. code value 0.
 *
 * @param ua the integer code for the user-agent string
 * @deprecated Please use {@link #setUserAgentString} instead.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::SetUserAgent(
    /* [in] */ Int32 ua)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the user-agent as an integer code.
 * <ul>
 *   <li>-1 means the WebView is using a custom user-agent string set with
 *   {@link #setUserAgentString}</li>
 *   <li>0 means the WebView should use an Android user-agent string</li>
 *   <li>1 means the WebView should use a desktop user-agent string</li>
 * </ul>
 *
 * @return the integer code for the user-agent string
 * @see #setUserAgent
 * @deprecated Please use {@link #getUserAgentString} instead.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebSettings::GetUserAgent(
    /* [out] */ Int32* ua)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Tells the WebView to use a wide viewport. The default is false.
 *
 * @param use whether to use a wide viewport
 */
ECode WebSettings::SetUseWideViewPort(
    /* [in] */ Boolean use)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView is using a wide viewport.
 *
 * @return true if the WebView is using a wide viewport
 * @see #setUseWideViewPort
 */
ECode WebSettings::GetUseWideViewPort(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView whether supports multiple windows. If set to
 * true, {@link WebChromeClient#onCreateWindow} must be implemented by the
 * host application. The default is false.
 *
 * @param support whether to suport multiple windows
 */
ECode WebSettings::SetSupportMultipleWindows(
    /* [in] */ Boolean support)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView supports multiple windows.
 *
 * @return true if the WebView supports multiple windows
 * @see #setSupportMultipleWindows
 */
ECode WebSettings::SupportMultipleWindows(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the underlying layout algorithm. This will cause a relayout of the
 * WebView. The default is {@link LayoutAlgorithm#NARROW_COLUMNS}.
 *
 * @param l the layout algorithm to use, as a {@link LayoutAlgorithm} value
 */
ECode WebSettings::SetLayoutAlgorithm(
    /* [in] */ LayoutAlgorithm l)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the current layout algorithm.
 *
 * @return the layout algorithm in use, as a {@link LayoutAlgorithm} value
 * @see #setLayoutAlgorithm
 */
ECode WebSettings::GetLayoutAlgorithm(
    /* [out] */ LayoutAlgorithm* thm)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the standard font family name. The default is "sans-serif".
 *
 * @param font a font family name
 */
ECode WebSettings::SetStandardFontFamily(
    /* [in] */ const String& font)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the standard font family name.
 *
 * @return the standard font family name as a string
 * @see #setStandardFontFamily
 */
ECode WebSettings::GetStandardFontFamily(
    /* [out] */ String* family)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the fixed font family name. The default is "monospace".
 *
 * @param font a font family name
 */
ECode WebSettings::SetFixedFontFamily(
    /* [in] */ const String& font)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the fixed font family name.
 *
 * @return the fixed font family name as a string
 * @see #setFixedFontFamily
 */
ECode WebSettings::GetFixedFontFamily(
    /* [out] */ String* family)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the sans-serif font family name. The default is "sans-serif".
 *
 * @param font a font family name
 */
ECode WebSettings::SetSansSerifFontFamily(
    /* [in] */ const String& font)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the sans-serif font family name.
 *
 * @return the sans-serif font family name as a string
 * @see #setSansSerifFontFamily
 */
ECode WebSettings::GetSansSerifFontFamily(
    /* [out] */ String* family)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the serif font family name. The default is "sans-serif".
 *
 * @param font a font family name
 */
ECode WebSettings::SetSerifFontFamily(
    /* [in] */ const String& font)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the serif font family name. The default is "serif".
 *
 * @return the serif font family name as a string
 * @see #setSerifFontFamily
 */
ECode WebSettings::GetSerifFontFamily(
    /* [out] */ String* family)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the cursive font family name. The default is "cursive".
 *
 * @param font a font family name
 */
ECode WebSettings::SetCursiveFontFamily(
    /* [in] */ const String& font)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the cursive font family name.
 *
 * @return the cursive font family name as a string
 * @see #setCursiveFontFamily
 */
ECode WebSettings::GetCursiveFontFamily(
    /* [out] */ String* family)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the fantasy font family name. The default is "fantasy".
 *
 * @param font a font family name
 */
ECode WebSettings::SetFantasyFontFamily(
    /* [in] */ const String& font)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the fantasy font family name.
 *
 * @return the fantasy font family name as a string
 * @see #setFantasyFontFamily
 */
ECode WebSettings::GetFantasyFontFamily(
    /* [out] */ String* family)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the minimum font size. The default is 8.
 *
 * @param size a non-negative integer between 1 and 72. Any number outside
 *             the specified range will be pinned.
 */
ECode WebSettings::SetMinimumFontSize(
    /* [in] */ Int32 size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the minimum font size.
 *
 * @return a non-negative integer between 1 and 72
 * @see #setMinimumFontSize
 */
ECode WebSettings::GetMinimumFontSize(
    /* [out] */ Int32* size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the minimum logical font size. The default is 8.
 *
 * @param size a non-negative integer between 1 and 72. Any number outside
 *             the specified range will be pinned.
 */
ECode WebSettings::SetMinimumLogicalFontSize(
    /* [in] */ Int32 size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the minimum logical font size.
 *
 * @return a non-negative integer between 1 and 72
 * @see #setMinimumLogicalFontSize
 */
ECode WebSettings::GetMinimumLogicalFontSize(
    /* [out] */ Int32* size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the default font size. The default is 16.
 *
 * @param size a non-negative integer between 1 and 72. Any number outside
 *             the specified range will be pinned.
 */
ECode WebSettings::SetDefaultFontSize(
    /* [in] */ Int32 size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the default font size.
 *
 * @return a non-negative integer between 1 and 72
 * @see #setDefaultFontSize
 */
ECode WebSettings::GetDefaultFontSize(
    /* [out] */ Int32* size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the default fixed font size. The default is 16.
 *
 * @param size a non-negative integer between 1 and 72. Any number outside
 *             the specified range will be pinned.
 */
ECode WebSettings::SetDefaultFixedFontSize(
    /* [in] */ Int32 size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the default fixed font size.
 *
 * @return a non-negative integer between 1 and 72
 * @see #setDefaultFixedFontSize
 */
ECode WebSettings::GetDefaultFixedFontSize(
    /* [out] */ Int32* size)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should load image resources. Note that this method
 * controls loading of all images, including those embedded using the data
 * URI scheme. Use {@link #setBlockNetworkImage} to control loading only
 * of images specified using network URI schemes. Note that if the value of this
 * setting is changed from false to true, all images resources referenced
 * by content currently displayed by the WebView are loaded automatically.
 * The default is true.
 *
 * @param flag whether the WebView should load image resources
 */
ECode WebSettings::SetLoadsImagesAutomatically(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView loads image resources. This includes
 * images embedded using the data URI scheme.
 *
 * @return true if the WebView loads image resources
 * @see #setLoadsImagesAutomatically
 */
ECode WebSettings::GetLoadsImagesAutomatically(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should not load image resources from the
 * network (resources accessed via http and https URI schemes).  Note
 * that this method has no effect unless
 * {@link #getLoadsImagesAutomatically} returns true. Also note that
 * disabling all network loads using {@link #setBlockNetworkLoads}
 * will also prevent network images from loading, even if this flag is set
 * to false. When the value of this setting is changed from true to false,
 * network images resources referenced by content currently displayed by
 * the WebView are fetched automatically. The default is false.
 *
 * @param flag whether the WebView should not load image resources from the
 *             network
 * @see #setBlockNetworkLoads
 */
ECode WebSettings::SetBlockNetworkImage(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView does not load image resources from the network.
 *
 * @return true if the WebView does not load image resources from the network
 * @see #setBlockNetworkImage
 */
ECode WebSettings::GetBlockNetworkImage(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should not load resources from the network.
 * Use {@link #setBlockNetworkImage} to only avoid loading
 * image resources. Note that if the value of this setting is
 * changed from true to false, network resources referenced by content
 * currently displayed by the WebView are not fetched until
 * {@link android.webkit.WebView#reload} is called.
 * If the application does not have the
 * {@link android.Manifest.permission#INTERNET} permission, attempts to set
 * a value of false will cause a {@link java.lang.SecurityException}
 * to be thrown. The default value is false if the application has the
 * {@link android.Manifest.permission#INTERNET} permission, otherwise it is
 * true.
 *
 * @param flag whether the WebView should not load any resources from the
 *             network
 * @see android.webkit.WebView#reload
 */
ECode WebSettings::SetBlockNetworkLoads(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the WebView does not load any resources from the network.
 *
 * @return true if the WebView does not load any resources from the network
 * @see #setBlockNetworkLoads
 */
ECode WebSettings::GetBlockNetworkLoads(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Tells the WebView to enable JavaScript execution.
 * <b>The default is false.</b>
 *
 * @param flag true if the WebView should execute JavaScript
 */
ECode WebSettings::SetJavaScriptEnabled(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether JavaScript running in the context of a file scheme URL
 * should be allowed to access content from any origin. This includes
 * access to content from other file scheme URLs. See
 * {@link #setAllowFileAccessFromFileURLs}. To enable the most restrictive,
 * and therefore secure policy, this setting should be disabled.
 * <p>
 * The default value is true for API level
 * {@link android.os.Build.VERSION_CODES#ICE_CREAM_SANDWICH_MR1} and below,
 * and false for API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN}
 * and above.
 *
 * @param flag whether JavaScript running in the context of a file scheme
 *             URL should be allowed to access content from any origin
 */
ECode WebSettings::SetAllowUniversalAccessFromFileURLs(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether JavaScript running in the context of a file scheme URL
 * should be allowed to access content from other file scheme URLs. To
 * enable the most restrictive, and therefore secure policy, this setting
 * should be disabled. Note that the value of this setting is ignored if
 * the value of {@link #getAllowUniversalAccessFromFileURLs} is true.
 * <p>
 * The default value is true for API level
 * {@link android.os.Build.VERSION_CODES#ICE_CREAM_SANDWICH_MR1} and below,
 * and false for API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN}
 * and above.
 *
 * @param flag whether JavaScript running in the context of a file scheme
 *             URL should be allowed to access content from other file
 *             scheme URLs
 */
ECode WebSettings::SetAllowFileAccessFromFileURLs(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the WebView should enable plugins. The default is false.
 *
 * @param flag true if plugins should be enabled
 * @deprecated This method has been deprecated in favor of
 *             {@link #setPluginState}
 */
ECode WebSettings::SetPluginsEnabled(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Tells the WebView to enable, disable, or have plugins on demand. On
 * demand mode means that if a plugin exists that can handle the embedded
 * content, a placeholder icon will be shown instead of the plugin. When
 * the placeholder is clicked, the plugin will be enabled. The default is
 * {@link PluginState#OFF}.
 *
 * @param state a PluginState value
 */
ECode WebSettings::SetPluginState(
    /* [in] */ PluginState state)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets a custom path to plugins used by the WebView. This method is
 * obsolete since each plugin is now loaded from its own package.
 *
 * @param pluginsPath a String path to the directory containing plugins
 * @deprecated This method is no longer used as plugins are loaded from
 *             their own APK via the system's package manager.
 */
ECode WebSettings::SetPluginsPath(
    /* [in] */ const String& pluginsPath)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the path to where database storage API databases should be saved.
 * In order for the database storage API to function correctly, this method
 * must be called with a path to which the application can write. This
 * method should only be called once: repeated calls are ignored.
 *
 * @param databasePath a path to the directory where databases should be
 *                     saved.
 */
// This will update WebCore when the Sync runs in the C++ side.
// Note that the WebCore Database Tracker only allows the path to be set
// once.
ECode WebSettings::SetDatabasePath(
    /* [in] */ const String& databasePath)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the path where the Geolocation databases should be saved. In order
 * for Geolocation permissions and cached positions to be persisted, this
 * method must be called with a path to which the application can write.
 *
 * @param databasePath a path to the directory where databases should be
 *                     saved.
 */
// This will update WebCore when the Sync runs in the C++ side.
ECode WebSettings::SetGeolocationDatabasePath(
    /* [in] */ const String& databasePath)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the Application Caches API should be enabled. The default
 * is false. Note that in order for the Application Caches API to be
 * enabled, a valid database path must also be supplied to
 * {@link #setAppCachePath}.
 *
 * @param flag true if the WebView should enable Application Caches
 */
ECode WebSettings::SetAppCacheEnabled(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the path to the Application Caches files. In order for the
 * Application Caches API to be enabled, this method must be called with a
 * path to which the application can write. This method should only be
 * called once: repeated calls are ignored.
 *
 * @param appCachePath a String path to the directory containing
 *                     Application Caches files.
 * @see setAppCacheEnabled
 */
ECode WebSettings::SetAppCachePath(
    /* [in] */ const String& appCachePath)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the maximum size for the Application Cache content. The passed size
 * will be rounded to the nearest value that the database can support, so
 * this should be viewed as a guide, not a hard limit. Setting the
 * size to a value less than current database size does not cause the
 * database to be trimmed. The default size is {@link Long#MAX_VALUE}.
 *
 * @param appCacheMaxSize the maximum size in bytes
 */
ECode WebSettings::SetAppCacheMaxSize(
    /* [in] */ Int64 appCacheMaxSize)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the database storage API is enabled. The default value is
 * false. See also {@link #setDatabasePath} for how to correctly set up the
 * database storage API.
 *
 * @param flag true if the WebView should use the database storage API
 */
ECode WebSettings::SetDatabaseEnabled(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether the DOM storage API is enabled. The default value is false.
 *
 * @param flag true if the WebView should use the DOM storage API
 */
ECode WebSettings::SetDomStorageEnabled(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the DOM Storage APIs are enabled.
 *
 * @return true if the DOM Storage APIs are enabled
 * @see #setDomStorageEnabled
 */
ECode WebSettings::GetDomStorageEnabled(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the path to where database storage API databases are saved.
 *
 * @return the String path to the database storage API databases
 * @see #setDatabasePath
 */
ECode WebSettings::GetDatabasePath(
    /* [out] */ String* param)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether the database storage API is enabled.
 *
 * @return true if the database storage API is enabled
 * @see #setDatabaseEnabled
 */
ECode WebSettings::GetDatabaseEnabled(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets whether Geolocation is enabled. The default is true. See also
 * {@link #setGeolocationDatabasePath} for how to correctly set up
 * Geolocation.
 *
 * @param flag whether Geolocation should be enabled
 */
ECode WebSettings::SetGeolocationEnabled(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether JavaScript is enabled.
 *
 * @return true if JavaScript is enabled
 * @see #setJavaScriptEnabled
 */
ECode WebSettings::GetJavaScriptEnabled(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether JavaScript running in the context of a file scheme URL can
 * access content from any origin. This includes access to content from
 * other file scheme URLs.
 *
 * @return whether JavaScript running in the context of a file scheme URL
 *         can access content from any origin
 * @see #setAllowUniversalAccessFromFileURLs
 */
ECode WebSettings::GetAllowUniversalAccessFromFileURLs(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether JavaScript running in the context of a file scheme URL can
 * access content from other file scheme URLs.
 *
 * @return whether JavaScript running in the context of a file scheme URL
 *         can access content from other file scheme URLs
 * @see #setAllowFileAccessFromFileURLs
 */
ECode WebSettings::GetAllowFileAccessFromFileURLs(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether plugins are enabled.
 *
 * @return true if plugins are enabled
 * @see #setPluginsEnabled
 * @deprecated This method has been replaced by {@link #getPluginState}
 */
ECode WebSettings::GetPluginsEnabled(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the current state regarding whether plugins are enabled.
 *
 * @return the plugin state as a {@link PluginState} value
 * @see #setPluginState
 */
ECode WebSettings::GetPluginState(
    /* [out] */ PluginState* state)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the directory that contains the plugin libraries. This method is
 * obsolete since each plugin is now loaded from its own package.
 *
 * @return an empty string
 * @deprecated This method is no longer used as plugins are loaded from
 * their own APK via the system's package manager.
 */
ECode WebSettings::GetPluginsPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    // Unconditionally returns empty string, so no need for derived classes to override.
    *path = "";
    return NOERROR;
}

/**
 * Tells JavaScript to open windows automatically. This applies to the
 * JavaScript function window.open(). The default is false.
 *
 * @param flag true if JavaScript can open windows automatically
 */
ECode WebSettings::SetJavaScriptCanOpenWindowsAutomatically(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets whether JavaScript can open windows automatically.
 *
 * @return true if JavaScript can open windows automatically during
 *         window.open()
 * @see #setJavaScriptCanOpenWindowsAutomatically
 */
ECode WebSettings::GetJavaScriptCanOpenWindowsAutomatically(
    /* [out] */ Boolean* result)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the default text encoding name to use when decoding html pages.
 * The default is "Latin-1".
 *
 * @param encoding the text encoding name
 */
ECode WebSettings::SetDefaultTextEncodingName(
    /* [in] */ const String& encoding)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the default text encoding name.
 *
 * @return the default text encoding name as a string
 * @see #setDefaultTextEncodingName
 */
ECode WebSettings::GetDefaultTextEncodingName(
    /* [out] */ String* name)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the WebView's user-agent string. If the string is null or empty,
 * the system default value will be used.
 */
ECode WebSettings::SetUserAgentString(
    /* [in] */ const String& ua)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the WebView's user-agent string.
 *
 * @return the WebView's user-agent string
 * @see #setUserAgentString
 */
ECode WebSettings::GetUserAgentString(
    /* [out] */ String* ua)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}


/**
 * Tells the WebView whether it needs to set a node to have focus when
 * {@link WebView#requestFocus(int, android.graphics.Rect)} is called. The
 * default value is true.
 *
 * @param flag whether the WebView needs to set a node
 */
ECode WebSettings::SetNeedInitialFocus(
    /* [in] */ Boolean flag)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Sets the priority of the Render thread. Unlike the other settings, this
 * one only needs to be called once per process. The default value is
 * {@link RenderPriority#NORMAL}.
 *
 * @param priority the priority
 */
ECode WebSettings::SetRenderPriority(
    /* [in] */ RenderPriority priority)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Overrides the way the cache is used. The way the cache is used is based
 * on the navigation type. For a normal page load, the cache is checked
 * and content is re-validated as needed. When navigating back, content is
 * not revalidated, instead the content is just retrieved from the cache.
 * This method allows the client to override this behavior by specifying
 * one of {@link #LOAD_DEFAULT}, {@link #LOAD_NORMAL},
 * {@link #LOAD_CACHE_ELSE_NETWORK}, {@link #LOAD_NO_CACHE} or
 * {@link #LOAD_CACHE_ONLY}. The default value is {@link #LOAD_DEFAULT}.
 *
 * @param mode the mode to use
 */
ECode WebSettings::SetCacheMode(
    /* [in] */ Int32 mode)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

/**
 * Gets the current setting for overriding the cache mode.
 *
 * @return the current setting for overriding the cache mode
 * @see #setCacheMode
 */
ECode WebSettings::GetCacheMode(
    /* [out] */ Int32* mode)
{
    return E_MUST_OVERRIDE_EXCEPTION;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
