
#ifndef __ELASTOS_DROID_WEBKIT_WEBSETTINGS_H__
#define __ELASTOS_DROID_WEBKIT_WEBSETTINGS_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Manages settings state for a WebView. When a WebView is first created, it
 * obtains a set of default settings. These default settings will be returned
 * from any getter call. A WebSettings object obtained from
 * WebView.getSettings() is tied to the life of the WebView. If a WebView has
 * been destroyed, any method call on WebSettings will throw an
 * IllegalStateException.
 */
// This is an abstract base class: concrete WebViewProviders must
// create a class derived from this, and return an instance of it in the
// WebViewProvider.getWebSettingsProvider() method implementation.
class ECO_PUBLIC WebSettings
    : public Object
    , public IWebSettings
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Enables dumping the pages navigation cache to a text file. The default
     * is false.
     *
     * @deprecated This method is now obsolete.
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    CARAPI SetNavDump(
        /* [in] */ Boolean enabled);

    /**
     * Gets whether dumping the navigation cache is enabled.
     *
     * @return whether dumping the navigation cache is enabled
     * @see #setNavDump
     * @deprecated This method is now obsolete.
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    CARAPI GetNavDump(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView should support zooming using its on-screen zoom
     * controls and gestures. The particular zoom mechanisms that should be used
     * can be set with {@link #setBuiltInZoomControls}. This setting does not
     * affect zooming performed using the {@link WebView#zoomIn()} and
     * {@link WebView#zoomOut()} methods. The default is true.
     *
     * @param support whether the WebView should support zoom
     */
    CARAPI SetSupportZoom(
        /* [in] */ Boolean support);

    /**
     * Gets whether the WebView supports zoom.
     *
     * @return true if the WebView supports zoom
     * @see #setSupportZoom
     */
    CARAPI SupportZoom(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView requires a user gesture to play media.
     * The default is true.
     *
     * @param require whether the WebView requires a user gesture to play media
     */
    CARAPI SetMediaPlaybackRequiresUserGesture(
        /* [in] */ Boolean require);

    /**
     * Gets whether the WebView requires a user gesture to play media.
     *
     * @return true if the WebView requires a user gesture to play media
     * @see #setMediaPlaybackRequiresUserGesture
     */
    CARAPI GetMediaPlaybackRequiresUserGesture(
        /* [out] */ Boolean* result);

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
    CARAPI SetBuiltInZoomControls(
        /* [in] */ Boolean enabled);

    /**
     * Gets whether the zoom mechanisms built into WebView are being used.
     *
     * @return true if the zoom mechanisms built into WebView are being used
     * @see #setBuiltInZoomControls
     */
    CARAPI GetBuiltInZoomControls(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView should display on-screen zoom controls when
     * using the built-in zoom mechanisms. See {@link #setBuiltInZoomControls}.
     * The default is true.
     *
     * @param enabled whether the WebView should display on-screen zoom controls
     */
    CARAPI SetDisplayZoomControls(
        /* [in] */ Boolean enabled);

    /**
     * Gets whether the WebView displays on-screen zoom controls when using
     * the built-in zoom mechanisms.
     *
     * @return true if the WebView displays on-screen zoom controls when using
     *         the built-in zoom mechanisms
     * @see #setDisplayZoomControls
     */
    CARAPI GetDisplayZoomControls(
        /* [out] */ Boolean* result);

    /**
     * Enables or disables file access within WebView. File access is enabled by
     * default.  Note that this enables or disables file system access only.
     * Assets and resources are still accessible using file:///android_asset and
     * file:///android_res.
     */
    CARAPI SetAllowFileAccess(
        /* [in] */ Boolean allow);

    /**
     * Gets whether this WebView supports file access.
     *
     * @see #setAllowFileAccess
     */
    CARAPI GetAllowFileAccess(
        /* [out] */ Boolean* result);

    /**
     * Enables or disables content URL access within WebView.  Content URL
     * access allows WebView to load content from a content provider installed
     * in the system. The default is enabled.
     */
    CARAPI SetAllowContentAccess(
        /* [in] */ Boolean allow);

    /**
     * Gets whether this WebView supports content URL access.
     *
     * @see #setAllowContentAccess
     */
    CARAPI GetAllowContentAccess(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView loads pages in overview mode. The default is
     * false.
     */
    CARAPI SetLoadWithOverviewMode(
        /* [in] */ Boolean overview);

    /**
     * Gets whether this WebView loads pages in overview mode.
     *
     * @return whether this WebView loads pages in overview mode
     * @see #setLoadWithOverviewMode
     */
    CARAPI GetLoadWithOverviewMode(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView will enable smooth transition while panning or
     * zooming or while the window hosting the WebView does not have focus.
     * If it is true, WebView will choose a solution to maximize the performance.
     * e.g. the WebView's content may not be updated during the transition.
     * If it is false, WebView will keep its fidelity. The default value is false.
     *
     * @deprecated This method is now obsolete, and will become a no-op in future.
     */
    CARAPI SetEnableSmoothTransition(
        /* [in] */ Boolean enable);

    /**
     * Gets whether the WebView enables smooth transition while panning or
     * zooming.
     *
     * @see #setEnableSmoothTransition
     *
     * @deprecated This method is now obsolete, and will become a no-op in future.
     */
    CARAPI EnableSmoothTransition(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView uses its background for over scroll background.
     * If true, it will use the WebView's background. If false, it will use an
     * internal pattern. Default is true.
     *
     * @deprecated This method is now obsolete.
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    CARAPI SetUseWebViewBackgroundForOverscrollBackground(
        /* [in] */ Boolean view);

    /**
     * Gets whether this WebView uses WebView's background instead of
     * internal pattern for over scroll background.
     *
     * @see #setUseWebViewBackgroundForOverscrollBackground
     * @deprecated This method is now obsolete.
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    CARAPI GetUseWebViewBackgroundForOverscrollBackground(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView should save form data. The default is true,
     * unless in private browsing mode, when the value is always false.
     */
    CARAPI SetSaveFormData(
        /* [in] */ Boolean save);

    /**
     * Gets whether the WebView saves form data. Always false in private
     * browsing mode.
     *
     * @return whether the WebView saves form data
     * @see #setSaveFormData
     */
    CARAPI GetSaveFormData(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView should save passwords. The default is true.
     */
    CARAPI SetSavePassword(
        /* [in] */ Boolean save);

    /**
     * Gets whether the WebView saves passwords.
     *
     * @return whether the WebView saves passwords
     * @see #setSavePassword
     */
    CARAPI GetSavePassword(
        /* [out] */ Boolean* result);

    /**
     * Sets the text zoom of the page in percent. The default is 100.
     *
     * @param textZoom the text zoom in percent
     */
    CARAPI SetTextZoom(
        /* [in] */ Int32 textZoom);

    /**
     * Gets the text zoom of the page in percent.
     *
     * @return the text zoom of the page in percent
     * @see #setTextZoom
     */
    CARAPI GetTextZoom(
        /* [out] */ Int32* textZoom);

    /**
     * Sets policy for third party cookies.
     * Developers should access this via {@link CookieManager#setShouldAcceptThirdPartyCookies}.
     * @hide Internal API.
     */
    CARAPI SetAcceptThirdPartyCookies(
        /* [in] */ Boolean accept);

    /**
     * Gets policy for third party cookies.
     * Developers should access this via {@link CookieManager#getShouldAcceptThirdPartyCookies}.
     * @hide Internal API
     */
    CARAPI GetAcceptThirdPartyCookies(
        /* [out] */ Boolean* result);

    /**
     * Sets the text size of the page. The default is {@link TextSize#NORMAL}.
     *
     * @param t the text size as a {@link TextSize} value
     * @deprecated Use {@link #setTextZoom} instead.
     */
    CARAPI SetTextSize(
        /* [in] */ TextSize t);

    /**
     * Gets the text size of the page. If the text size was previously specified
     * in percent using {@link #setTextZoom}, this will return the closest
     * matching {@link TextSize}.
     *
     * @return the text size as a {@link TextSize} value
     * @see #setTextSize
     * @deprecated Use {@link #getTextZoom} instead.
     */
    CARAPI GetTextSize(
        /* [out] */ TextSize* size);

    /**
     * Sets the default zoom density of the page. This must be called from the UI
     * thread. The default is {@link ZoomDensity#MEDIUM}.
     *
     * @param zoom the zoom density
     */
    CARAPI SetDefaultZoom(
        /* [in] */ ZoomDensity zoom);

    /**
     * Gets the default zoom density of the page. This should be called from
     * the UI thread.
     *
     * @return the zoom density
     * @see #setDefaultZoom
     */
    CARAPI GetDefaultZoom(
        /* [out] */ ZoomDensity* zoom);

    /**
     * Enables using light touches to make a selection and activate mouseovers.
     * The default is false.
     */
    CARAPI SetLightTouchEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Gets whether light touches are enabled.
     *
     * @return whether light touches are enabled
     * @see #setLightTouchEnabled
     */
    CARAPI GetLightTouchEnabled(
        /* [out] */ Boolean* result);

    /**
     * Controlled a rendering optimization that is no longer present. Setting
     * it now has no effect.
     *
     * @deprecated This setting now has no effect.
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    CARAPI SetUseDoubleTree(
        /* [in] */ Boolean use);

    /**
     * Controlled a rendering optimization that is no longer present. Setting
     * it now has no effect.
     *
     * @deprecated This setting now has no effect.
     * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
     */
    CARAPI GetUseDoubleTree(
        /* [out] */ Boolean* result);

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
    CARAPI SetUserAgent(
        /* [in] */ Int32 ua);

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
    CARAPI GetUserAgent(
        /* [out] */ Int32* ua);

    /**
     * Tells the WebView to use a wide viewport. The default is false.
     *
     * @param use whether to use a wide viewport
     */
    CARAPI SetUseWideViewPort(
        /* [in] */ Boolean use);

    /**
     * Gets whether the WebView is using a wide viewport.
     *
     * @return true if the WebView is using a wide viewport
     * @see #setUseWideViewPort
     */
    CARAPI GetUseWideViewPort(
        /* [out] */ Boolean* result);

    /**
     * Sets whether the WebView whether supports multiple windows. If set to
     * true, {@link WebChromeClient#onCreateWindow} must be implemented by the
     * host application. The default is false.
     *
     * @param support whether to suport multiple windows
     */
    CARAPI SetSupportMultipleWindows(
        /* [in] */ Boolean support);

    /**
     * Gets whether the WebView supports multiple windows.
     *
     * @return true if the WebView supports multiple windows
     * @see #setSupportMultipleWindows
     */
    CARAPI SupportMultipleWindows(
        /* [out] */ Boolean* result);

    /**
     * Sets the underlying layout algorithm. This will cause a relayout of the
     * WebView. The default is {@link LayoutAlgorithm#NARROW_COLUMNS}.
     *
     * @param l the layout algorithm to use, as a {@link LayoutAlgorithm} value
     */
    CARAPI SetLayoutAlgorithm(
        /* [in] */ LayoutAlgorithm l);

    /**
     * Gets the current layout algorithm.
     *
     * @return the layout algorithm in use, as a {@link LayoutAlgorithm} value
     * @see #setLayoutAlgorithm
     */
    CARAPI GetLayoutAlgorithm(
        /* [out] */ LayoutAlgorithm* thm);

    /**
     * Sets the standard font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    CARAPI SetStandardFontFamily(
        /* [in] */ const String& font);

    /**
     * Gets the standard font family name.
     *
     * @return the standard font family name as a string
     * @see #setStandardFontFamily
     */
    CARAPI GetStandardFontFamily(
        /* [out] */ String* family);

    /**
     * Sets the fixed font family name. The default is "monospace".
     *
     * @param font a font family name
     */
    CARAPI SetFixedFontFamily(
        /* [in] */ const String& font);

    /**
     * Gets the fixed font family name.
     *
     * @return the fixed font family name as a string
     * @see #setFixedFontFamily
     */
    CARAPI GetFixedFontFamily(
        /* [out] */ String* family);

    /**
     * Sets the sans-serif font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    CARAPI SetSansSerifFontFamily(
        /* [in] */ const String& font);

    /**
     * Gets the sans-serif font family name.
     *
     * @return the sans-serif font family name as a string
     * @see #setSansSerifFontFamily
     */
    CARAPI GetSansSerifFontFamily(
        /* [out] */ String* family);

    /**
     * Sets the serif font family name. The default is "sans-serif".
     *
     * @param font a font family name
     */
    CARAPI SetSerifFontFamily(
        /* [in] */ const String& font);

    /**
     * Gets the serif font family name. The default is "serif".
     *
     * @return the serif font family name as a string
     * @see #setSerifFontFamily
     */
    CARAPI GetSerifFontFamily(
        /* [out] */ String* family);

    /**
     * Sets the cursive font family name. The default is "cursive".
     *
     * @param font a font family name
     */
    CARAPI SetCursiveFontFamily(
        /* [in] */ const String& font);

    /**
     * Gets the cursive font family name.
     *
     * @return the cursive font family name as a string
     * @see #setCursiveFontFamily
     */
    CARAPI GetCursiveFontFamily(
        /* [out] */ String* family);

    /**
     * Sets the fantasy font family name. The default is "fantasy".
     *
     * @param font a font family name
     */
    CARAPI SetFantasyFontFamily(
        /* [in] */ const String& font);

    /**
     * Gets the fantasy font family name.
     *
     * @return the fantasy font family name as a string
     * @see #setFantasyFontFamily
     */
    CARAPI GetFantasyFontFamily(
        /* [out] */ String* family);

    /**
     * Sets the minimum font size. The default is 8.
     *
     * @param size a non-negative integer between 1 and 72. Any number outside
     *             the specified range will be pinned.
     */
    CARAPI SetMinimumFontSize(
        /* [in] */ Int32 size);

    /**
     * Gets the minimum font size.
     *
     * @return a non-negative integer between 1 and 72
     * @see #setMinimumFontSize
     */
    CARAPI GetMinimumFontSize(
        /* [out] */ Int32* size);

    /**
     * Sets the minimum logical font size. The default is 8.
     *
     * @param size a non-negative integer between 1 and 72. Any number outside
     *             the specified range will be pinned.
     */
    CARAPI SetMinimumLogicalFontSize(
        /* [in] */ Int32 size);

    /**
     * Gets the minimum logical font size.
     *
     * @return a non-negative integer between 1 and 72
     * @see #setMinimumLogicalFontSize
     */
    CARAPI GetMinimumLogicalFontSize(
        /* [out] */ Int32* size);

    /**
     * Sets the default font size. The default is 16.
     *
     * @param size a non-negative integer between 1 and 72. Any number outside
     *             the specified range will be pinned.
     */
    CARAPI SetDefaultFontSize(
        /* [in] */ Int32 size);

    /**
     * Gets the default font size.
     *
     * @return a non-negative integer between 1 and 72
     * @see #setDefaultFontSize
     */
    CARAPI GetDefaultFontSize(
        /* [out] */ Int32* size);

    /**
     * Sets the default fixed font size. The default is 16.
     *
     * @param size a non-negative integer between 1 and 72. Any number outside
     *             the specified range will be pinned.
     */
    CARAPI SetDefaultFixedFontSize(
        /* [in] */ Int32 size);

    /**
     * Gets the default fixed font size.
     *
     * @return a non-negative integer between 1 and 72
     * @see #setDefaultFixedFontSize
     */
    CARAPI GetDefaultFixedFontSize(
        /* [out] */ Int32* size);

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
    CARAPI SetLoadsImagesAutomatically(
        /* [in] */ Boolean flag);

    /**
     * Gets whether the WebView loads image resources. This includes
     * images embedded using the data URI scheme.
     *
     * @return true if the WebView loads image resources
     * @see #setLoadsImagesAutomatically
     */
    CARAPI GetLoadsImagesAutomatically(
        /* [out] */ Boolean* result);

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
    CARAPI SetBlockNetworkImage(
        /* [in] */ Boolean flag);

    /**
     * Gets whether the WebView does not load image resources from the network.
     *
     * @return true if the WebView does not load image resources from the network
     * @see #setBlockNetworkImage
     */
    CARAPI GetBlockNetworkImage(
        /* [out] */ Boolean* result);

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
    CARAPI SetBlockNetworkLoads(
        /* [in] */ Boolean flag);

    /**
     * Gets whether the WebView does not load any resources from the network.
     *
     * @return true if the WebView does not load any resources from the network
     * @see #setBlockNetworkLoads
     */
    CARAPI GetBlockNetworkLoads(
        /* [out] */ Boolean* result);

    /**
     * Tells the WebView to enable JavaScript execution.
     * <b>The default is false.</b>
     *
     * @param flag true if the WebView should execute JavaScript
     */
    CARAPI SetJavaScriptEnabled(
        /* [in] */ Boolean flag);

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
    CARAPI SetAllowUniversalAccessFromFileURLs(
        /* [in] */ Boolean flag);

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
    CARAPI SetAllowFileAccessFromFileURLs(
        /* [in] */ Boolean flag);

    /**
     * Sets whether the WebView should enable plugins. The default is false.
     *
     * @param flag true if plugins should be enabled
     * @deprecated This method has been deprecated in favor of
     *             {@link #setPluginState}
     */
    CARAPI SetPluginsEnabled(
        /* [in] */ Boolean flag);

    /**
     * Tells the WebView to enable, disable, or have plugins on demand. On
     * demand mode means that if a plugin exists that can handle the embedded
     * content, a placeholder icon will be shown instead of the plugin. When
     * the placeholder is clicked, the plugin will be enabled. The default is
     * {@link PluginState#OFF}.
     *
     * @param state a PluginState value
     */
    CARAPI SetPluginState(
        /* [in] */ PluginState state);

    /**
     * Sets a custom path to plugins used by the WebView. This method is
     * obsolete since each plugin is now loaded from its own package.
     *
     * @param pluginsPath a String path to the directory containing plugins
     * @deprecated This method is no longer used as plugins are loaded from
     *             their own APK via the system's package manager.
     */
    CARAPI SetPluginsPath(
        /* [in] */ const String& pluginsPath);

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
    CARAPI SetDatabasePath(
        /* [in] */ const String& databasePath);

    /**
     * Sets the path where the Geolocation databases should be saved. In order
     * for Geolocation permissions and cached positions to be persisted, this
     * method must be called with a path to which the application can write.
     *
     * @param databasePath a path to the directory where databases should be
     *                     saved.
     */
    // This will update WebCore when the Sync runs in the C++ side.
    CARAPI SetGeolocationDatabasePath(
        /* [in] */ const String& databasePath);

    /**
     * Sets whether the Application Caches API should be enabled. The default
     * is false. Note that in order for the Application Caches API to be
     * enabled, a valid database path must also be supplied to
     * {@link #setAppCachePath}.
     *
     * @param flag true if the WebView should enable Application Caches
     */
    CARAPI SetAppCacheEnabled(
        /* [in] */ Boolean flag);

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
    CARAPI SetAppCachePath(
        /* [in] */ const String& appCachePath);

    /**
     * Sets the maximum size for the Application Cache content. The passed size
     * will be rounded to the nearest value that the database can support, so
     * this should be viewed as a guide, not a hard limit. Setting the
     * size to a value less than current database size does not cause the
     * database to be trimmed. The default size is {@link Long#MAX_VALUE}.
     *
     * @param appCacheMaxSize the maximum size in bytes
     */
    CARAPI SetAppCacheMaxSize(
        /* [in] */ Int64 appCacheMaxSize);

    /**
     * Sets whether the database storage API is enabled. The default value is
     * false. See also {@link #setDatabasePath} for how to correctly set up the
     * database storage API.
     *
     * @param flag true if the WebView should use the database storage API
     */
    CARAPI SetDatabaseEnabled(
        /* [in] */ Boolean flag);

    /**
     * Sets whether the DOM storage API is enabled. The default value is false.
     *
     * @param flag true if the WebView should use the DOM storage API
     */
    CARAPI SetDomStorageEnabled(
        /* [in] */ Boolean flag);

    /**
     * Gets whether the DOM Storage APIs are enabled.
     *
     * @return true if the DOM Storage APIs are enabled
     * @see #setDomStorageEnabled
     */
    CARAPI GetDomStorageEnabled(
        /* [out] */ Boolean* result);

    /**
     * Gets the path to where database storage API databases are saved.
     *
     * @return the String path to the database storage API databases
     * @see #setDatabasePath
     */
    CARAPI GetDatabasePath(
        /* [out] */ String* param);

    /**
     * Gets whether the database storage API is enabled.
     *
     * @return true if the database storage API is enabled
     * @see #setDatabaseEnabled
     */
    CARAPI GetDatabaseEnabled(
        /* [out] */ Boolean* result);

    /**
     * Sets whether Geolocation is enabled. The default is true. See also
     * {@link #setGeolocationDatabasePath} for how to correctly set up
     * Geolocation.
     *
     * @param flag whether Geolocation should be enabled
     */
    CARAPI SetGeolocationEnabled(
        /* [in] */ Boolean flag);

    /**
     * Gets whether JavaScript is enabled.
     *
     * @return true if JavaScript is enabled
     * @see #setJavaScriptEnabled
     */
    CARAPI GetJavaScriptEnabled(
        /* [out] */ Boolean* result);

    /**
     * Gets whether JavaScript running in the context of a file scheme URL can
     * access content from any origin. This includes access to content from
     * other file scheme URLs.
     *
     * @return whether JavaScript running in the context of a file scheme URL
     *         can access content from any origin
     * @see #setAllowUniversalAccessFromFileURLs
     */
    CARAPI GetAllowUniversalAccessFromFileURLs(
        /* [out] */ Boolean* result);

    /**
     * Gets whether JavaScript running in the context of a file scheme URL can
     * access content from other file scheme URLs.
     *
     * @return whether JavaScript running in the context of a file scheme URL
     *         can access content from other file scheme URLs
     * @see #setAllowFileAccessFromFileURLs
     */
    CARAPI GetAllowFileAccessFromFileURLs(
        /* [out] */ Boolean* result);

    /**
     * Gets whether plugins are enabled.
     *
     * @return true if plugins are enabled
     * @see #setPluginsEnabled
     * @deprecated This method has been replaced by {@link #getPluginState}
     */
    CARAPI GetPluginsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Gets the current state regarding whether plugins are enabled.
     *
     * @return the plugin state as a {@link PluginState} value
     * @see #setPluginState
     */
    CARAPI GetPluginState(
        /* [out] */ PluginState* state);

    /**
     * Gets the directory that contains the plugin libraries. This method is
     * obsolete since each plugin is now loaded from its own package.
     *
     * @return an empty string
     * @deprecated This method is no longer used as plugins are loaded from
     * their own APK via the system's package manager.
     */
    CARAPI GetPluginsPath(
        /* [out] */ String* path);

    /**
     * Tells JavaScript to open windows automatically. This applies to the
     * JavaScript function window.open(). The default is false.
     *
     * @param flag true if JavaScript can open windows automatically
     */
    CARAPI SetJavaScriptCanOpenWindowsAutomatically(
        /* [in] */ Boolean flag);

    /**
     * Gets whether JavaScript can open windows automatically.
     *
     * @return true if JavaScript can open windows automatically during
     *         window.open()
     * @see #setJavaScriptCanOpenWindowsAutomatically
     */
    CARAPI GetJavaScriptCanOpenWindowsAutomatically(
        /* [out] */ Boolean* result);

    /**
     * Sets the default text encoding name to use when decoding html pages.
     * The default is "Latin-1".
     *
     * @param encoding the text encoding name
     */
    CARAPI SetDefaultTextEncodingName(
        /* [in] */ const String& encoding);

    /**
     * Gets the default text encoding name.
     *
     * @return the default text encoding name as a string
     * @see #setDefaultTextEncodingName
     */
    CARAPI GetDefaultTextEncodingName(
        /* [out] */ String* name);

    /**
     * Sets the WebView's user-agent string. If the string is null or empty,
     * the system default value will be used.
     */
    CARAPI SetUserAgentString(
        /* [in] */ const String& ua);

    /**
     * Gets the WebView's user-agent string.
     *
     * @return the WebView's user-agent string
     * @see #setUserAgentString
     */
    CARAPI GetUserAgentString(
        /* [out] */ String* ua);

    /**
     * Returns the default User-Agent used by a WebView.
     * An instance of WebView could use a different User-Agent if a call
     * is made to {@link WebSettings#setUserAgentString(String)}.
     *
     * @param context a Context object used to access application assets
     */
    static CARAPI_(String) GetDefaultUserAgent(
        /* [in] */ IContext* context);

    /**
     * Tells the WebView whether it needs to set a node to have focus when
     * {@link WebView#requestFocus(int, android.graphics.Rect)} is called. The
     * default value is true.
     *
     * @param flag whether the WebView needs to set a node
     */
    CARAPI SetNeedInitialFocus(
        /* [in] */ Boolean flag);

    /**
     * Sets the priority of the Render thread. Unlike the other settings, this
     * one only needs to be called once per process. The default value is
     * {@link RenderPriority#NORMAL}.
     *
     * @param priority the priority
     */
    CARAPI SetRenderPriority(
        /* [in] */ RenderPriority priority);

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
    CARAPI SetCacheMode(
        /* [in] */ Int32 mode);

    /**
     * Gets the current setting for overriding the cache mode.
     *
     * @return the current setting for overriding the cache mode
     * @see #setCacheMode
     */
    CARAPI GetCacheMode(
        /* [out] */ Int32* mode);

protected:
    WebSettings();

    virtual ~WebSettings();
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBSETTINGS_H__
