
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/webkit/CookieSyncManager.h"
#include "elastos/droid/webkit/CPluginList.h"
#include "elastos/droid/webkit/DebugFlags.h"
#include "elastos/droid/webkit/URLUtil.h"
#include "elastos/droid/webkit/WebView.h"
#include "elastos/droid/webkit/WebViewFactory.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {

//=================================================================
//     WebView::WebViewTransport
//=================================================================

CAR_INTERFACE_IMPL(WebView::WebViewTransport, Object, IWebViewTransport);

ECode WebView::WebViewTransport::constructor()
{
    return NOERROR;
}

ECode WebView::WebViewTransport::SetWebView(
    /* [in] */ IWebView* webview)
{
    AutoLock lock(this);
    mWebview = webview;
    return NOERROR;
}

ECode WebView::WebViewTransport::GetWebView(
    /* [out] */ IWebView** webView)
{
    AutoLock lock(this);
    *webView = mWebview;
    REFCOUNT_ADD(*webView);
    return NOERROR;
}

ECode WebView::WebViewTransport::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebView::WebViewTransport";
    return NOERROR;
}

//=================================================================
//     WebView::PrivateAccess
//=================================================================

CAR_INTERFACE_IMPL(WebView::PrivateAccess, Object, IWebViewPrivateAccess)

// ---- Access to super-class methods ----
ECode WebView::PrivateAccess::Super_getScrollBarStyle(
    /* [out] */ Int32* style)
{
    VALIDATE_NOT_NULL(style);
    mHost->AbsoluteLayout::GetScrollBarStyle(style);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_scrollTo(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY)
{
    mHost->AbsoluteLayout::ScrollTo(scrollX, scrollY);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_computeScroll()
{
    mHost->AbsoluteLayout::ComputeScroll();
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_onHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->AbsoluteLayout::OnHoverEvent(event, result);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_performAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mHost->AbsoluteLayout::PerformAccessibilityAction(action, arguments, result);
}

ECode WebView::PrivateAccess::Super_performLongClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->AbsoluteLayout::PerformLongClick(result);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_setFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost->AbsoluteLayout::SetFrame(left, top, right, bottom);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_dispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->AbsoluteLayout::DispatchKeyEvent(event, result);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_onGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->AbsoluteLayout::OnGenericMotionEvent(event, result);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_requestFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->AbsoluteLayout::RequestFocus(direction, previouslyFocusedRect, result);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_setLayoutParams(
    /* [in] */ IViewGroupLayoutParams* params)
{
    mHost->AbsoluteLayout::SetLayoutParams(params);
    return NOERROR;
}

// ---- Access to non-public methods ----
ECode WebView::PrivateAccess::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Int32 maxOverScrollX,
    /* [in] */ Int32 maxOverScrollY,
    /* [in] */ Boolean isTouchEvent)
{
    mHost->OverScrollBy(deltaX, deltaY, scrollX, scrollY, scrollRangeX, scrollRangeY,
                     maxOverScrollX, maxOverScrollY, isTouchEvent);
    return NOERROR;
}

ECode WebView::PrivateAccess::AwakenScrollBars(
    /* [in] */ Int32 duration)
{
    mHost->AwakenScrollBars(duration);
    return NOERROR;
}

ECode WebView::PrivateAccess::AwakenScrollBars(
    /* [in] */ Int32 duration,
    /* [in] */ Boolean invalidate)
{
    mHost->AwakenScrollBars(duration, invalidate);
    return NOERROR;
}

ECode WebView::PrivateAccess::GetVerticalScrollFactor(
    /* [out] */ Float* factor)
{
    VALIDATE_NOT_NULL(factor);
    mHost->GetVerticalScrollFactor(factor);
    return NOERROR;
}

ECode WebView::PrivateAccess::GetHorizontalScrollFactor(
    /* [out] */ Float* factor)
{
    VALIDATE_NOT_NULL(factor);
    mHost->GetHorizontalScrollFactor(factor);
    return NOERROR;
}

ECode WebView::PrivateAccess::SetMeasuredDimension(
    /* [in] */ Int32 measuredWidth,
    /* [in] */ Int32 measuredHeight)
{
    mHost->SetMeasuredDimension(measuredWidth, measuredHeight);
    return NOERROR;
}

ECode WebView::PrivateAccess::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    mHost->OnScrollChanged(l, t, oldl, oldt);
    return NOERROR;
}

ECode WebView::PrivateAccess::GetHorizontalScrollbarHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    mHost->GetHorizontalScrollbarHeight(height);
    return NOERROR;
}

ECode WebView::PrivateAccess::Super_onDrawVerticalScrollBar(
            /* [in] */ ICanvas* canvas,
            /* [in] */ IDrawable* scrollBar,
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b)
{
    mHost->AbsoluteLayout::OnDrawVerticalScrollBar(canvas, scrollBar, l, t, r, b);
    return NOERROR;
}

// ---- Access to (non-public) fields ----
/** Raw setter for the scroll X value, without invoking onScrollChanged handlers etc. */
ECode WebView::PrivateAccess::SetScrollXRaw(
    /* [in] */ Int32 scrollX)
{
    mHost->mScrollX = scrollX;
    return NOERROR;
}

/** Raw setter for the scroll Y value, without invoking onScrollChanged handlers etc. */
ECode WebView::PrivateAccess::SetScrollYRaw(
    /* [in] */ Int32 scrollY)
{
    mHost->mScrollY = scrollY;
    return NOERROR;
}

ECode WebView::PrivateAccess::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebView::PrivateAccess";
    return NOERROR;
}

//=================================================================
//                 WebView::FindListenerDistributor
//=================================================================

CAR_INTERFACE_IMPL(WebView::FindListenerDistributor, Object, IWebViewFindListener)

WebView::FindListenerDistributor::FindListenerDistributor(
    /* [in] */ WebView* owner)
    : mOwner(owner)
{
}

ECode WebView::FindListenerDistributor::OnFindResultReceived(
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
    if (mFindDialogFindListener != NULL) {
        mFindDialogFindListener->OnFindResultReceived(activeMatchOrdinal, numberOfMatches,
                isDoneCounting);
    }

    if (mUserFindListener != NULL) {
        mUserFindListener->OnFindResultReceived(activeMatchOrdinal, numberOfMatches,
                isDoneCounting);
    }

    return NOERROR;
}

//=================================================================
//                           WebView
//=================================================================

const String WebView::LOGTAG("WebView_proxy");

volatile Boolean WebView::sEnforceThreadChecking = FALSE;

Object WebView::sLock;

CAR_INTERFACE_IMPL(WebView, Object, IWebView);

WebView::WebView()
{
    mWebViewThread = Looper::GetMyLooper();
}

ECode WebView::constructor(
    /* [in] */ IContext * context)
{
    return constructor(context, NULL);
}

ECode WebView::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IAttributeSet * attrs)
{
    return constructor(context, attrs, R::attr::webViewStyle);
}

ECode WebView::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IAttributeSet * attrs,
    /* [in] */ Int32 defStyle)
{
    return constructor(context, attrs, defStyle, FALSE);
}

ECode WebView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return constructor(context, attrs, defStyleAttr, defStyleRes, NULL, FALSE);
}

ECode WebView::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IAttributeSet * attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Boolean privateBrowsing)
{
    return constructor(context, attrs, defStyleAttr, 0, NULL, privateBrowsing);
}

ECode WebView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle,
    /* [in] */ IMap* javaScriptInterfaces,
    /* [in] */ Boolean privateBrowsing)
{
    if (context == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(AbsoluteLayout::constructor(context, attrs, defStyle));
    FAIL_RETURN(CheckThread());

    EnsureProviderCreated();
    return mProvider->Init(javaScriptInterfaces, privateBrowsing);
}

ECode WebView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [in] */ IMap* javaScriptInterfaces,
    /* [in] */ Boolean privateBrowsing)
{
    AbsoluteLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    if (context == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IApplicationInfo> appInfo;
    context->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 sdkVersion;
    appInfo->GetTargetSdkVersion(&sdkVersion);
    sEnforceThreadChecking = sdkVersion >=
            Build::VERSION_CODES::JELLY_BEAN_MR2;
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "WebView<init>");
    }

    EnsureProviderCreated();
    mProvider->Init(javaScriptInterfaces, privateBrowsing);
    // Post condition of creating a webview is the CookieSyncManager.getInstance() is allowed.
    CookieSyncManager::SetGetInstanceIsAllowed();

    return NOERROR;
}

/**
 * Specifies whether the horizontal scrollbar has overlay style.
 *
 * @param overlay true if horizontal scrollbar should have overlay style
 */
ECode WebView::SetHorizontalScrollbarOverlay(
    /* [in] */ Boolean overlay)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setHorizontalScrollbarOverlay=" + overlay);
    }

    return mProvider->SetHorizontalScrollbarOverlay(overlay);
}

/**
 * Specifies whether the vertical scrollbar has overlay style.
 *
 * @param overlay true if vertical scrollbar should have overlay style
 */
ECode WebView::SetVerticalScrollbarOverlay(
    /* [in] */ Boolean overlay)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setVerticalScrollbarOverlay=" + overlay);
    }
    return mProvider->SetVerticalScrollbarOverlay(overlay);
}

/**
 * Gets whether horizontal scrollbar has overlay style.
 *
 * @return true if horizontal scrollbar has overlay style
 */
ECode WebView::OverlayHorizontalScrollbar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->OverlayHorizontalScrollbar(result);
}

/**
 * Gets whether vertical scrollbar has overlay style.
 *
 * @return true if vertical scrollbar has overlay style
 */
ECode WebView::OverlayVerticalScrollbar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->OverlayVerticalScrollbar(result);
}

/**
 * Gets the visible height (in pixels) of the embedded title bar (if any).
 *
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebView::GetVisibleTitleHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    FAIL_RETURN(CheckThread());
    return mProvider->GetVisibleTitleHeight(height);
}

/**
 * Gets the SSL certificate for the main top-level page or null if there is
 * no certificate (the site is not secure).
 *
 * @return the SSL certificate for the main top-level page
 */
ECode WebView::GetCertificate(
    /* [out] */ ISslCertificate** cert)
{
    VALIDATE_NOT_NULL(cert);
    FAIL_RETURN(CheckThread());
    return mProvider->GetCertificate(cert);
}

/**
 * Sets the SSL certificate for the main top-level page.
 *
 * @deprecated Calling this function has no useful effect, and will be
 * ignored in future releases.
 */
//@Deprecated
ECode WebView::SetCertificate(
    /* [in] */ ISslCertificate* certificate)
{
    FAIL_RETURN(CheckThread());
     if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setCertificate=%p", certificate);
    }
    return mProvider->SetCertificate(certificate);
}

//-------------------------------------------------------------------------
// Methods called by activity
//-------------------------------------------------------------------------

/**
 * Sets a username and password pair for the specified host. This data is
 * used by the WebView to autocomplete username and password fields in web
 * forms. Note that this is unrelated to the credentials used for HTTP
 * authentication.
 *
 * @param host the host that required the credentials
 * @param username the username for the given host
 * @param password the password for the given host
 * @see WebViewDatabase#clearUsernamePassword
 * @see WebViewDatabase#hasUsernamePassword
 */
ECode WebView::SavePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "savePassword=%s", host.string());
    }
    return mProvider->SavePassword(host, username, password);
}

/**
 * Stores HTTP authentication credentials for a given host and realm. This
 * method is intended to be used with
 * {@link WebViewClient#onReceivedHttpAuthRequest}.
 *
 * @param host the host to which the credentials apply
 * @param realm the realm to which the credentials apply
 * @param username the username
 * @param password the password
 * @see getHttpAuthUsernamePassword
 * @see WebViewDatabase#hasHttpAuthUsernamePassword
 * @see WebViewDatabase#clearHttpAuthUsernamePassword
 */
ECode WebView::SetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setHttpAuthUsernamePassword=%s", host.string());
    }
    return mProvider->SetHttpAuthUsernamePassword(host, realm, username, password);
}

/**
 * Retrieves HTTP authentication credentials for a given host and realm.
 * This method is intended to be used with
 * {@link WebViewClient#onReceivedHttpAuthRequest}.
 *
 * @param host the host to which the credentials apply
 * @param realm the realm to which the credentials apply
 * @return the credentials as a String array, if found. The first element
 *         is the username and the second element is the password. Null if
 *         no credentials are found.
 * @see setHttpAuthUsernamePassword
 * @see WebViewDatabase#hasHttpAuthUsernamePassword
 * @see WebViewDatabase#clearHttpAuthUsernamePassword
 */
ECode WebView::GetHttpAuthUsernamePassword(
    /* [in] */ const String& host,
    /* [in] */ const String& realm,
    /* [out, callee] */ ArrayOf<String>** up)
{
    VALIDATE_NOT_NULL(up);
    FAIL_RETURN(CheckThread());
    return mProvider->GetHttpAuthUsernamePassword(host, realm, up);
}

/**
 * Destroys the internal state of this WebView. This method should be called
 * after this WebView has been removed from the view system. No other
 * methods may be called on this WebView after destroy.
 */
ECode WebView::Destroy()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "destroy");
    }
    return mProvider->Destroy();
}

/**
 * Enables platform notifications of data state and proxy changes.
 * Notifications are enabled by default.
 *
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
void WebView::EnablePlatformNotifications()
{
    // noop
}

/**
 * Disables platform notifications of data state and proxy changes.
 * Notifications are enabled by default.
 *
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
void WebView::DisablePlatformNotifications()
{
    // noop
}

/**
 * Used only by internal tests to free up memory.
 *
 * @hide
 */
void WebView::FreeMemoryForTests()
{
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    GetFactory()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    statics->FreeMemoryForTests();
}

/**
 * Informs WebView of the network state. This is used to set
 * the JavaScript property window.navigator.isOnline and
 * generates the online/offline event as specified in HTML5, sec. 5.7.7
 *
 * @param networkUp a boolean indicating if network is available
 */
ECode WebView::SetNetworkAvailable(
    /* [in] */ Boolean networkUp)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setNetworkAvailable=%d", networkUp);
    }
    return mProvider->SetNetworkAvailable(networkUp);
}

/**
 * Saves the state of this WebView used in
 * {@link android.app.Activity#onSaveInstanceState}. Please note that this
 * method no longer stores the display data for this WebView. The previous
 * behavior could potentially leak files if {@link #restoreState} was never
 * called.
 *
 * @param outState the Bundle to store this WebView's state
 * @return the same copy of the back/forward list used to save the state. If
 *         saveState fails, the returned list will be null.
 */
ECode WebView::SaveState(
    /* [in] */ IBundle* outState,
    /* [out] */ IWebBackForwardList** wfl)
{
    VALIDATE_NOT_NULL(wfl);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "saveState");
    }
    return mProvider->SaveState(outState, wfl);
}

/**
 * Saves the current display data to the Bundle given. Used in conjunction
 * with {@link #saveState}.
 * @param b a Bundle to store the display data
 * @param dest the file to store the serialized picture data. Will be
 *             overwritten with this WebView's picture data.
 * @return true if the picture was successfully saved
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebView::SavePicture(
    /* [in] */ IBundle* b,
    /* [in] */ IFile* dest,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        String name;
        dest->GetName(&name);
        Logger::D(LOGTAG, "savePicture=%s", name.string());
    }
    return mProvider->SavePicture(b, dest, result);
}

/**
 * Restores the display data that was saved in {@link #savePicture}. Used in
 * conjunction with {@link #restoreState}. Note that this will not work if
 * this WebView is hardware accelerated.
 *
 * @param b a Bundle containing the saved display data
 * @param src the file where the picture data was stored
 * @return true if the picture was successfully restored
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebView::RestorePicture(
    /* [in] */ IBundle* b,
    /* [in] */ IFile* src,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        String name;
        src->GetName(&name);
        Logger::D(LOGTAG, "restorePicture=%s", name.string());
    }
    return mProvider->RestorePicture(b, src, result);
}

/**
 * Restores the state of this WebView from the given Bundle. This method is
 * intended for use in {@link android.app.Activity#onRestoreInstanceState}
 * and should be called to restore the state of this WebView. If
 * it is called after this WebView has had a chance to build state (load
 * pages, create a back/forward list, etc.) there may be undesirable
 * side-effects. Please note that this method no longer restores the
 * display data for this WebView.
 *
 * @param inState the incoming Bundle of state
 * @return the restored back/forward list or null if restoreState failed
 */
ECode WebView::RestoreState(
    /* [in] */ IBundle* inState,
    /* [out] */ IWebBackForwardList** wfl)
{
    VALIDATE_NOT_NULL(wfl);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "restoreState");
    }
    return mProvider->RestoreState(inState, wfl);
}

/**
 * Loads the given URL with the specified additional HTTP headers.
 *
 * @param url the URL of the resource to load
 * @param additionalHttpHeaders the additional headers to be used in the
 *            HTTP request for this URL, specified as a map from name to
 *            value. Note that if this map contains any of the headers
 *            that are set by default by this WebView, such as those
 *            controlling caching, accept types or the User-Agent, their
 *            values may be overriden by this WebView's defaults.
 */
ECode WebView::LoadUrl(
    /* [in] */ const String& url,
    /* [in] */ IMap* additionalHttpHeaders)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        StringBuilder headers;
        if (additionalHttpHeaders != NULL) {
            AutoPtr<ISet> entrySet;
            additionalHttpHeaders->GetEntrySet((ISet**)&entrySet);
            AutoPtr<IIterator> iter;
            entrySet->GetIterator((IIterator**)&iter);
            Boolean bNext = FALSE;
            iter->HasNext(&bNext);
            for (; bNext; iter->HasNext(&bNext)) {
                AutoPtr<IMapEntry> entry;
                iter->GetNext((IInterface**)&entry);
                AutoPtr<ICharSequence> iKey, iValue;
                entry->GetKey((IInterface**)&iKey);
                entry->GetValue((IInterface**)&iValue);
                String key, value;
                iKey->ToString(&key);
                iValue->ToString(&value);
                headers.Append(key);
                headers.Append(":");
                headers.Append(value);
                headers.Append("\n");
            }
        }
        String headersStr;
        headers.ToString(&headersStr);
        Logger::D(LOGTAG, "loadUrl(extra headers)=%s%s%s", url.string(), "\n", headersStr.string());
    }
    return mProvider->LoadUrl(url, additionalHttpHeaders);
}

/**
 * Loads the given URL.
 *
 * @param url the URL of the resource to load
 */
ECode WebView::LoadUrl(
    /* [in] */ const String& url)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "loadUrl=%s", url.string());
    }
    return mProvider->LoadUrl(url);
}

/**
 * Loads the URL with postData using "POST" method into this WebView. If url
 * is not a network URL, it will be loaded with {link
 * {@link #loadUrl(String)} instead.
 *
 * @param url the URL of the resource to load
 * @param postData the data will be passed to "POST" request
 */
ECode WebView::PostUrl(
    /* [in] */ const String& url,
    /* [in] */ ArrayOf<Byte>* postData)
{
    FAIL_RETURN(CheckThread());

    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "postUrl=%s", url.string());
    }

    if (URLUtil::IsNetworkUrl(url)) {
        return mProvider->PostUrl(url, postData);
    }
    else {
        return mProvider->LoadUrl(url);
    }
}

/**
 * Loads the given data into this WebView using a 'data' scheme URL.
 * <p>
 * Note that JavaScript's same origin policy means that script running in a
 * page loaded using this method will be unable to access content loaded
 * using any scheme other than 'data', including 'http(s)'. To avoid this
 * restriction, use {@link
 * #loadDataWithBaseURL(String,String,String,String,String)
 * loadDataWithBaseURL()} with an appropriate base URL.
 * <p>
 * The encoding parameter specifies whether the data is base64 or URL
 * encoded. If the data is base64 encoded, the value of the encoding
 * parameter must be 'base64'. For all other values of the parameter,
 * including null, it is assumed that the data uses ASCII encoding for
 * octets inside the range of safe URL characters and use the standard %xx
 * hex encoding of URLs for octets outside that range. For example, '#',
 * '%', '\', '?' should be replaced by %23, %25, %27, %3f respectively.
 * <p>
 * The 'data' scheme URL formed by this method uses the default US-ASCII
 * charset. If you need need to set a different charset, you should form a
 * 'data' scheme URL which explicitly specifies a charset parameter in the
 * mediatype portion of the URL and call {@link #loadUrl(String)} instead.
 * Note that the charset obtained from the mediatype portion of a data URL
 * always overrides that specified in the HTML or XML document itself.
 *
 * @param data a String of data in the given encoding
 * @param mimeType the MIME type of the data, e.g. 'text/html'
 * @param encoding the encoding of the data
 */
ECode WebView::LoadData(
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "loadData");
    }
    return mProvider->LoadData(data, mimeType, encoding);
}

/**
 * Loads the given data into this WebView, using baseUrl as the base URL for
 * the content. The base URL is used both to resolve relative URLs and when
 * applying JavaScript's same origin policy. The historyUrl is used for the
 * history entry.
 * <p>
 * Note that content specified in this way can access local device files
 * (via 'file' scheme URLs) only if baseUrl specifies a scheme other than
 * 'http', 'https', 'ftp', 'ftps', 'about' or 'javascript'.
 * <p>
 * If the base URL uses the data scheme, this method is equivalent to
 * calling {@link #loadData(String,String,String) loadData()} and the
 * historyUrl is ignored.
 *
 * @param baseUrl the URL to use as the page's base URL. If null defaults to
 *                'about:blank'.
 * @param data a String of data in the given encoding
 * @param mimeType the MIMEType of the data, e.g. 'text/html'. If null,
 *                 defaults to 'text/html'.
 * @param encoding the encoding of the data
 * @param historyUrl the URL to use as the history entry. If null defaults
 *                   to 'about:blank'.
 */
ECode WebView::LoadDataWithBaseURL(
    /* [in] */ const String& baseUrl,
    /* [in] */ const String& data,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& encoding,
    /* [in] */ const String& historyUrl)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "loadDataWithBaseURL=%s", baseUrl.string());
    }
    return mProvider->LoadDataWithBaseURL(baseUrl, data, mimeType, encoding, historyUrl);
}

ECode WebView::EvaluateJavascript(
    /* [in] */ const String& script,
    /* [in] */ IValueCallback* resultCallback)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "evaluateJavascript=%s", script.string());
    }
    return mProvider->EvaluateJavaScript(script, resultCallback);
}

/**
 * Saves the current view as a web archive.
 *
 * @param filename the filename where the archive should be placed
 */
ECode WebView::SaveWebArchive(
    /* [in] */ const String& filename)
{
    FAIL_RETURN(CheckThread());
    return mProvider->SaveWebArchive(filename);
}

/**
 * Saves the current view as a web archive.
 *
 * @param basename the filename where the archive should be placed
 * @param autoname if false, takes basename to be a file. If true, basename
 *                 is assumed to be a directory in which a filename will be
 *                 chosen according to the URL of the current page.
 * @param callback called after the web archive has been saved. The
 *                 parameter for onReceiveValue will either be the filename
 *                 under which the file was saved, or null if saving the
 *                 file failed.
 */
ECode WebView::SaveWebArchive(
    /* [in] */ const String& basename,
    /* [in] */ Boolean autoname,
    /* [in] */ IValueCallback* callback)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "saveWebArchive(auto)=%s", basename.string());
    }
    return mProvider->SaveWebArchive(basename, autoname, callback);
}

/**
 * Stops the current load.
 */
ECode WebView::StopLoading()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "stopLoading");
    }
    return mProvider->StopLoading();
}

/**
 * Reloads the current URL.
 */
ECode WebView::Reload()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "reload");
    }
    return mProvider->Reload();
}

/**
 * Gets whether this WebView has a back history item.
 *
 * @return true iff this WebView has a back history item
 */
ECode WebView::CanGoBack(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->CanGoBack(result);
}

/**
 * Goes back in the history of this WebView.
 */
ECode WebView::GoBack()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "goBack");
    }
    return mProvider->GoBack();
}

/**
 * Gets whether this WebView has a forward history item.
 *
 * @return true iff this WebView has a forward history item
 */
ECode WebView::CanGoForward(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->CanGoForward(result);
}

/**
 * Goes forward in the history of this WebView.
 */
ECode WebView::GoForward()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "goForward");
    }
    return mProvider->GoForward();
}

/**
 * Gets whether the page can go back or forward the given
 * number of steps.
 *
 * @param steps the negative or positive number of steps to move the
 *              history
 */
ECode WebView::CanGoBackOrForward(
    /* [in] */ Int32 steps,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->CanGoBackOrForward(steps, result);
}

/**
 * Goes to the history item that is the number of steps away from
 * the current item. Steps is negative if backward and positive
 * if forward.
 *
 * @param steps the number of steps to take back or forward in the back
 *              forward list
 */
ECode WebView::GoBackOrForward(
    /* [in] */ Int32 steps)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "goBackOrForwad=%d", steps);
    }
    return mProvider->GoBackOrForward(steps);
}

/**
 * Gets whether private browsing is enabled in this WebView.
 */
ECode WebView::IsPrivateBrowsingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->IsPrivateBrowsingEnabled(result);
}

/**
 * Scrolls the contents of this WebView up by half the view size.
 *
 * @param top true to jump to the top of the page
 * @return true if the page was scrolled
 */
ECode WebView::PageUp(
    /* [in] */ Boolean top,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "pageUp");
    }
    return mProvider->PageUp(top, result);
}

/**
 * Scrolls the contents of this WebView down by half the page size.
 *
 * @param bottom true to jump to bottom of page
 * @return true if the page was scrolled
 */
ECode WebView::PageDown(
    /* [in] */ Boolean bottom,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "pageDown");
    }
    return mProvider->PageDown(bottom, result);
}

/**
 * Clears this WebView so that onDraw() will draw nothing but white background,
 * and onMeasure() will return 0 if MeasureSpec is not MeasureSpec.EXACTLY.
 */
ECode WebView::ClearView()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "clearView");
    }
    return mProvider->ClearView();
}

/**
 * Gets a new picture that captures the current contents of this WebView.
 * The picture is of the entire document being displayed, and is not
 * limited to the area currently displayed by this WebView. Also, the
 * picture is a static copy and is unaffected by later changes to the
 * content being displayed.
 * <p>
 * Note that due to internal changes, for API levels between
 * {@link android.os.Build.VERSION_CODES#HONEYCOMB} and
 * {@link android.os.Build.VERSION_CODES#ICE_CREAM_SANDWICH} inclusive, the
 * picture does not include fixed position elements or scrollable divs.
 *
 * @return a picture that captures the current contents of this WebView
 */
ECode WebView::CapturePicture(
    /* [out] */ IPicture** picture)
{
    VALIDATE_NOT_NULL(picture);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "capturePicture");
    }
    return mProvider->CapturePicture(picture);
}

// TODO
// ECode WebView::CreatePrintDocumentAdapter(
//     /* [out] */ IPrintDocumentAdapter** adapter)
// {
//     FAIL_RETURN(CheckThread());
//     if (DebugFlags::TRACE_API) {
//         Logger::D(LOGTAG, "createPrintDocumentAdapter");
//     }
//     return mProvider->CreatePrintDocumentAdapter("default", adapter);
// }

// TODO
// ECode WebView::CreatePrintDocumentAdapter(
//     /* [in] */ const String& documentName,
//     /* [out] */ IPrintDocumentAdapter** adpter)
// {
//     FAIL_RETURN(CheckThread());
//     if (DebugFlags::TRACE_API) {
//         Log::D(LOGTAG, "createPrintDocumentAdapter");
//     }
//     return mProvider->CreatePrintDocumentAdapter(documentName, adpter);
// }

/**
 * Gets the current scale of this WebView.
 *
 * @return the current scale
 *
 * @deprecated This method is prone to inaccuracy due to race conditions
 * between the web rendering and UI threads; prefer
 * {@link WebViewClient#onScaleChanged}.
 */
ECode WebView::GetScale(
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);
    FAIL_RETURN(CheckThread());
    return mProvider->GetScale(scale);
}

/**
 * Sets the initial scale for this WebView. 0 means default. If
 * {@link WebSettings#getUseWideViewPort()} is true, it zooms out all the
 * way. Otherwise it starts with 100%. If initial scale is greater than 0,
 * WebView starts with this value as initial scale.
 * Please note that unlike the scale properties in the viewport meta tag,
 * this method doesn't take the screen density into account.
 *
 * @param scaleInPercent the initial scale in percent
 */
ECode WebView::SetInitialScale(
    /* [in] */ Int32 scaleInPercent)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setInitialScale=%d", scaleInPercent);
    }
    return mProvider->SetInitialScale(scaleInPercent);
}

/**
 * Invokes the graphical zoom picker widget for this WebView. This will
 * result in the zoom widget appearing on the screen to control the zoom
 * level of this WebView.
 */
ECode WebView::InvokeZoomPicker()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "invokeZoomPicker");
    }
    return mProvider->InvokeZoomPicker();
}

/**
 * Gets a HitTestResult based on the current cursor node. If a HTML::a
 * tag is found and the anchor has a non-JavaScript URL, the HitTestResult
 * type is set to SRC_ANCHOR_TYPE and the URL is set in the "extra" field.
 * If the anchor does not have a URL or if it is a JavaScript URL, the type
 * will be UNKNOWN_TYPE and the URL has to be retrieved through
 * {@link #requestFocusNodeHref} asynchronously. If a HTML::img tag is
 * found, the HitTestResult type is set to IMAGE_TYPE and the URL is set in
 * the "extra" field. A type of
 * SRC_IMAGE_ANCHOR_TYPE indicates an anchor with a URL that has an image as
 * a child node. If a phone number is found, the HitTestResult type is set
 * to PHONE_TYPE and the phone number is set in the "extra" field of
 * HitTestResult. If a map address is found, the HitTestResult type is set
 * to GEO_TYPE and the address is set in the "extra" field of HitTestResult.
 * If an email address is found, the HitTestResult type is set to EMAIL_TYPE
 * and the email is set in the "extra" field of HitTestResult. Otherwise,
 * HitTestResult type is set to UNKNOWN_TYPE.
 */
ECode WebView::GetHitTestResult(
    /* [out] */ IWebViewHitTestResult** result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "getHitTestResult");
    }
    return mProvider->GetHitTestResult(result);
}

/**
 * Requests the anchor or image element URL at the last tapped point.
 * If hrefMsg is null, this method returns immediately and does not
 * dispatch hrefMsg to its target. If the tapped point hits an image,
 * an anchor, or an image in an anchor, the message associates
 * strings in named keys in its data. The value paired with the key
 * may be an empty string.
 *
 * @param hrefMsg the message to be dispatched with the result of the
 *                request. The message data contains three keys. "url"
 *                returns the anchor's href attribute. "title" returns the
 *                anchor's text. "src" returns the image's src attribute.
 */
ECode WebView::RequestFocusNodeHref(
    /* [in] */ IMessage* hrefMsg)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "requestFocusNodeHref");
    }
    return mProvider->RequestFocusNodeHref(hrefMsg);
}

/**
 * Requests the URL of the image last touched by the user. msg will be sent
 * to its target with a String representing the URL as its object.
 *
 * @param msg the message to be dispatched with the result of the request
 *            as the data member with "url" as key. The result can be null.
 */
ECode WebView::RequestImageRef(
    /* [in] */ IMessage* msg)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "requestImageRef");
    }
    return mProvider->RequestImageRef(msg);
}

/**
 * Gets the URL for the current page. This is not always the same as the URL
 * passed to WebViewClient.onPageStarted because although the load for
 * that URL has begun, the current page may not have changed.
 *
 * @return the URL for the current page
 */
ECode WebView::GetUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    FAIL_RETURN(CheckThread());
    return mProvider->GetUrl(url);
}

/**
 * Gets the original URL for the current page. This is not always the same
 * as the URL passed to WebViewClient.onPageStarted because although the
 * load for that URL has begun, the current page may not have changed.
 * Also, there may have been redirects resulting in a different URL to that
 * originally requested.
 *
 * @return the URL that was originally requested for the current page
 */
ECode WebView::GetOriginalUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    FAIL_RETURN(CheckThread());
    return mProvider->GetOriginalUrl(url);
}

/**
 * Gets the title for the current page. This is the title of the current page
 * until WebViewClient.onReceivedTitle is called.
 *
 * @return the title for the current page
 */
ECode WebView::GetTitle(
    /* [out] */ String* title)
{
    VALIDATE_NOT_NULL(title);
    FAIL_RETURN(CheckThread());
    return mProvider->GetTitle(title);
}

/**
 * Gets the favicon for the current page. This is the favicon of the current
 * page until WebViewClient.onReceivedIcon is called.
 *
 * @return the favicon for the current page
 */
ECode WebView::GetFavicon(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    FAIL_RETURN(CheckThread());
    return mProvider->GetFavicon(bitmap);
}

/**
 * Gets the touch icon URL for the apple-touch-icon <link> element, or
 * a URL on this site's server pointing to the standard location of a
 * touch icon.
 *
 * @hide
 */
ECode WebView::GetTouchIconUrl(
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    return mProvider->GetTouchIconUrl(url);
}

/**
 * Gets the progress for the current page.
 *
 * @return the progress for the current page between 0 and 100
 */
ECode WebView::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress);
    FAIL_RETURN(CheckThread());
    return mProvider->GetProgress(progress);
}

/**
 * Gets the height of the HTML content.
 *
 * @return the height of the HTML content
 */
ECode WebView::GetContentHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    FAIL_RETURN(CheckThread());
    return mProvider->GetContentHeight(height);
}

/**
 * Gets the width of the HTML content.
 *
 * @return the width of the HTML content
 * @hide
 */
ECode WebView::GetContentWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    return mProvider->GetContentWidth(width);
}

/**
 * Pauses all layout, parsing, and JavaScript timers for all WebViews. This
 * is a global requests, not restricted to just this WebView. This can be
 * useful if the application has been paused.
 */
ECode WebView::PauseTimers()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "pauseTimers");
    }
    return mProvider->PauseTimers();
}

/**
 * Resumes all layout, parsing, and JavaScript timers for all WebViews.
 * This will resume dispatching all timers.
 */
ECode WebView::ResumeTimers()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "resumeTimers");
    }
    return mProvider->ResumeTimers();
}

/**
 * Pauses any extra processing associated with this WebView and its
 * associated DOM, plugins, JavaScript etc. For example, if this WebView is
 * taken offscreen, this could be called to reduce unnecessary CPU or
 * network traffic. When this WebView is again "active", call onResume().
 * Note that this differs from pauseTimers(), which affects all WebViews.
 */
ECode WebView::OnPause()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "onPause");
    }
    return mProvider->OnPause();
}

/**
 * Resumes a WebView after a previous call to onPause().
 */
ECode WebView::OnResume()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "onResume");
    }
    return mProvider->OnResume();
}

/**
 * Gets whether this WebView is paused, meaning onPause() was called.
 * Calling onResume() sets the paused state back to false.
 *
 * @hide
 */
ECode WebView::IsPaused(
    /* [out] */ Boolean* isPaused)
{
    VALIDATE_NOT_NULL(isPaused);
    return mProvider->IsPaused(isPaused);
}

/**
 * Informs this WebView that memory is low so that it can free any available
 * memory.
 */
ECode WebView::FreeMemory()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "freeMemory");
    }
    return mProvider->FreeMemory();
}

/**
 * Clears the resource cache. Note that the cache is per-application, so
 * this will clear the cache for all WebViews used.
 *
 * @param includeDiskFiles if false, only the RAM cache is cleared
 */
ECode WebView::ClearCache(
    /* [in] */ Boolean includeDiskFiles)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "clearCache");
    }
    return mProvider->ClearCache(includeDiskFiles);
}

/**
 * Removes the autocomplete popup from the currently focused form field, if
 * present. Note this only affects the display of the autocomplete popup,
 * it does not remove any saved form data from this WebView's store. To do
 * that, use {@link WebViewDatabase#clearFormData}.
 */
ECode WebView::ClearFormData()
{
    FAIL_RETURN(CheckThread());
    return mProvider->ClearFormData();
}

/**
 * Tells this WebView to clear its internal back/forward list.
 */
ECode WebView::ClearHistory()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "clearFormData");
    }
    return mProvider->ClearHistory();
}

/**
 * Clears the SSL preferences table stored in response to proceeding with
 * SSL certificate errors.
 */
ECode WebView::ClearSslPreferences()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "clearSslPreferences");
    }
    return mProvider->ClearSslPreferences();
}

/**
 * Clears the client certificate preferences stored in response
 * to proceeding/cancelling client cert requests. Note that Webview
 * automatically clears these preferences when it receives a
 * {@link KeyChain#ACTION_STORAGE_CHANGED} intent. The preferences are
 * shared by all the webviews that are created by the embedder application.
 *
 * @param onCleared  A runnable to be invoked when client certs are cleared.
 *                   The embedder can pass null if not interested in the
 *                   callback. The runnable will be called in UI thread.
 */
ECode WebView::ClearClientCertPreferences(
    /* [in] */ IRunnable* onCleared)
{
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "clearClientCertPreferences");
    }
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    GetFactory()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    return statics->ClearClientCertPreferences(onCleared);
}

/**
 * Gets the WebBackForwardList for this WebView. This contains the
 * back/forward list for use in querying each item in the history stack.
 * This is a copy of the private WebBackForwardList so it contains only a
 * snapshot of the current state. Multiple calls to this method may return
 * different objects. The object returned from this method will not be
 * updated to reflect any new state.
 */
ECode WebView::CopyBackForwardList(
    /* [out] */ IWebBackForwardList** wfl)
{
    VALIDATE_NOT_NULL(wfl);
    FAIL_RETURN(CheckThread());
    return mProvider->CopyBackForwardList(wfl);
}

/**
 * Registers the listener to be notified as find-on-page operations
 * progress. This will replace the current listener.
 *
 * @param listener an implementation of {@link FindListener}
 */
ECode WebView::SetFindListener(
    /* [in] */ IWebViewFindListener* listener)
{
    FAIL_RETURN(CheckThread());
    SetupFindListenerIfNeeded();
    mFindListener->mUserFindListener = listener;
    return NOERROR;
}

/**
 * Highlights and scrolls to the next match found by
 * {@link #findAllAsync}, wrapping around page boundaries as necessary.
 * Notifies any registered {@link FindListener}. If {@link #findAllAsync(String)}
 * has not been called yet, or if {@link #clearMatches} has been called since the
 * last find operation, this function does nothing.
 *
 * @param forward the direction to search
 * @see #setFindListener
 */
ECode WebView::FindNext(
    /* [in] */ Boolean forward)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "findNext");
    }
    return mProvider->FindNext(forward);
}

/**
 * Finds all instances of find on the page and highlights them.
 * Notifies any registered {@link FindListener}.
 *
 * @param find the string to find
 * @return the number of occurances of the String "find" that were found
 * @deprecated {@link #findAllAsync} is preferred.
 * @see #setFindListener
 */
ECode WebView::FindAll(
    /* [in] */ const String& find,
    /* [out] */ Int32* all)
{
    VALIDATE_NOT_NULL(all);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "findAll");
    }
    AutoPtr<IStrictMode> sm;
    CStrictMode::AcquireSingleton((IStrictMode**)&sm);
    sm->NoteSlowCall(String("findAll blocks UI: prefer findAllAsync"));
    return mProvider->FindAll(find, all);
}

/**
 * Finds all instances of find on the page and highlights them,
 * asynchronously. Notifies any registered {@link FindListener}.
 * Successive calls to this will cancel any pending searches.
 *
 * @param find the string to find.
 * @see #setFindListener
 */
ECode WebView::FindAllAsync(
    /* [in] */ const String& find)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "findAllAsync");
    }
    return mProvider->FindAllAsync(find);
}

/**
 * Starts an ActionMode for finding text in this WebView.  Only works if this
 * WebView is attached to the view system.
 *
 * @param text if non-null, will be the initial text to search for.
 *             Otherwise, the last String searched for in this WebView will
 *             be used to start.
 * @param showIme if true, show the IME, assuming the user will begin typing.
 *                If false and text is non-null, perform a find all.
 * @return true if the find dialog is shown, false otherwise
 */
ECode WebView::ShowFindDialog(
    /* [in] */ const String& text,
    /* [in] */ Boolean showIme,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "showFindDialog");
    }
    return mProvider->ShowFindDialog(text, showIme, result);
}

/**
 * Gets the first substring consisting of the address of a physical
 * location. Currently, only addresses in the United States are detected,
 * and consist of:
 * <ul>
 *   <li>a house number</li>
 *   <li>a street name</li>
 *   <li>a street type (Road, Circle, etc), either spelled out or
 *       abbreviated</li>
 *   <li>a city name</li>
 *   <li>a state or territory, either spelled out or two-letter abbr</li>
 *   <li>an optional 5 digit or 9 digit zip code</li>
 * </ul>
 * All names must be correctly capitalized, and the zip code, if present,
 * must be valid for the state. The street type must be a standard USPS
 * spelling or abbreviation. The state or territory must also be spelled
 * or abbreviated using USPS standards. The house number may not exceed
 * five digits.
 *
 * @param addr the string to search for addresses
 * @return the address, or if no address is found, null
 */
String WebView::FindAddress(
    /* [in] */ const String& addr)
{
    // TODO: Rewrite this in Java so it is not needed to start up chromium
    // Could also be deprecated
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    GetFactory()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    String str;
    statics->FindAddress(addr, &str);
    return str;
}

/**
 * For apps targeting the L release, WebView has a new default behavior that reduces
 * memory footprint and increases performance by intelligently choosing
 * the portion of the HTML document that needs to be drawn. These
 * optimizations are transparent to the developers. However, under certain
 * circumstances, an App developer may want to disable them:
 * 1. When an app uses {@link #onDraw} to do own drawing and accesses portions
 * of the page that is way outside the visible portion of the page.
 * 2. When an app uses {@link #capturePicture} to capture a very large HTML document.
 * Note that capturePicture is a deprecated API.
 *
 * Enabling drawing the entire HTML document has a significant performance
 * cost. This method should be called before any WebViews are created.
 */
ECode WebView::EnableSlowWholeDocumentDraw()
{
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    GetFactory()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    return statics->EnableSlowWholeDocumentDraw();
}

/**
 * Clears the highlighting surrounding text matches created by
 * {@link #findAllAsync}.
 */
ECode WebView::ClearMatches()
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "clearMatches");
    }
    return mProvider->ClearMatches();
}

/**
 * Queries the document to see if it contains any image references. The
 * message object will be dispatched with arg1 being set to 1 if images
 * were found and 0 if the document does not reference any images.
 *
 * @param response the message that will be dispatched with the result
 */
ECode WebView::DocumentHasImages(
   /* [in] */ IMessage* response)
{
    FAIL_RETURN(CheckThread());
    return mProvider->DocumentHasImages(response);
}

/**
 * Sets the WebViewClient that will receive various notifications and
 * requests. This will replace the current handler.
 *
 * @param client an implementation of WebViewClient
 */
ECode WebView::SetWebViewClient(
    /* [in] */ IWebViewClient* client)
{
    FAIL_RETURN(CheckThread());
    return mProvider->SetWebViewClient(client);
}

/**
 * Registers the interface to be used when content can not be handled by
 * the rendering engine, and should be downloaded instead. This will replace
 * the current handler.
 *
 * @param listener an implementation of DownloadListener
 */
ECode WebView::SetDownloadListener(
    /* [in] */ IDownloadListener* listener)
{
    FAIL_RETURN(CheckThread());
    return mProvider->SetDownloadListener(listener);
}

/**
 * Sets the chrome handler. This is an implementation of WebChromeClient for
 * use in handling JavaScript dialogs, favicons, titles, and the progress.
 * This will replace the current handler.
 *
 * @param client an implementation of WebChromeClient
 */
ECode WebView::SetWebChromeClient(
    /* [in] */ IWebChromeClient* client)
{
    FAIL_RETURN(CheckThread());
    return mProvider->SetWebChromeClient(client);
}

/**
 * Sets the Picture listener. This is an interface used to receive
 * notifications of a new Picture.
 *
 * @param listener an implementation of WebView.PictureListener
 * @deprecated This method is now obsolete.
 */
ECode WebView::SetPictureListener(
    /* [in] */ IWebViewPictureListener* listener)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "setPictureListener=%p", listener);
    }
    return mProvider->SetPictureListener(listener);
}

/**
 * Injects the supplied Java object into this WebView. The object is
 * injected into the JavaScript context of the main frame, using the
 * supplied name. This allows the Java object's methods to be
 * accessed from JavaScript. For applications targeted to API
 * level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 * and above, only public methods that are annotated with
 * {@link android.webkit.JavascriptInterface} can be accessed from JavaScript.
 * For applications targeted to API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN} or below,
 * all public methods (including the inherited ones) can be accessed, see the
 * important security note below for implications.
 * <p> Note that injected objects will not
 * appear in JavaScript until the page is next (re)loaded. For example:
 * <pre>
 * class JsObject {
 *    {@literal @}JavascriptInterface
 *    String WebView::toString() { return "injectedObject"; }
 * }
 * WebView.addJavascriptInterface(new JsObject(), "injectedObject");
 * WebView.loadData("<!DOCTYPE html><title></title>", "text/html", null);
 * WebView.loadUrl("javascript:alert(injectedObject.toString())");</pre>
 * <p>
 * <strong>IMPORTANT:</strong>
 * <ul>
 * <li> This method can be used to allow JavaScript to control the host
 * application. This is a powerful feature, but also presents a security
 * risk for applications targeted to API level
 * {@link android.os.Build.VERSION_CODES#JELLY_BEAN} or below, because
 * JavaScript could use reflection to access an
 * injected object's public fields. Use of this method in a WebView
 * containing untrusted content could allow an attacker to manipulate the
 * host application in unintended ways, executing Java code with the
 * permissions of the host application. Use extreme care when using this
 * method in a WebView which could contain untrusted content.</li>
 * <li> JavaScript interacts with Java object on a private, background
 * thread of this WebView. Care is therefore required to maintain thread
 * safety.</li>
 * <li> The Java object's fields are not accessible.</li>
 * </ul>
 *
 * @param object the Java object to inject into this WebView's JavaScript
 *               context. Null values are ignored.
 * @param name the name used to expose the object in JavaScript
 */
ECode WebView::AddJavascriptInterface(
    /* [in] */ IInterface* object,
    /* [in] */ const String& name)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "addJavascriptInterface=%s", name.string());
    }
    return mProvider->AddJavascriptInterface(object, name);
}

/**
 * Removes a previously injected Java object from this WebView. Note that
 * the removal will not be reflected in JavaScript until the page is next
 * (re)loaded. See {@link #addJavascriptInterface}.
 *
 * @param name the name used to expose the object in JavaScript
 */
ECode WebView::RemoveJavascriptInterface(
    /* [in] */ const String& name)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "removeJavascriptInterface=%s", name.string());
    }
    return mProvider->RemoveJavascriptInterface(name);
}

/**
 * Gets the WebSettings object used to control the settings for this
 * WebView.
 *
 * @return a WebSettings object that can be used to control this WebView's
 *         settings
 */
ECode WebView::GetSettings(
    /* [out] */ IWebSettings** webSettings)
{
    VALIDATE_NOT_NULL(webSettings);
    FAIL_RETURN(CheckThread());
    return mProvider->GetSettings(webSettings);
}

/**
 * Enables debugging of web contents (HTML / CSS / JavaScript)
 * loaded into any WebViews of this application. This flag can be enabled
 * in order to facilitate debugging of web layouts and JavaScript
 * code running inside WebViews. Please refer to WebView documentation
 * for the debugging guide.
 *
 * The default is false.
 *
 * @param enabled whether to enable web contents debugging
 */
ECode WebView::SetWebContentsDebuggingEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    GetFactory()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    return statics->SetWebContentsDebuggingEnabled(enabled);
}

/**
 * Gets the list of currently loaded plugins.
 *
 * @return the list of currently loaded plugins
 * @deprecated This was used for Gears, which has been deprecated.
 * @hide
 */
AutoPtr<IPluginList> WebView::GetPluginList()
{
    AutoLock lock(sLock);
    AutoPtr<IPluginList> pl;
    CPluginList::New((IPluginList**)&pl);
    return pl;
}

/**
 * @deprecated This was used for Gears, which has been deprecated.
 * @hide
 */
ECode WebView::RefreshPlugins(
    /* [in] */ Boolean reloadOpenPages)
{
    FAIL_RETURN(CheckThread());
    return NOERROR;
}

/**
 * Puts this WebView into text selection mode. Do not rely on this
 * functionality; it will be deprecated in the future.
 *
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebView::EmulateShiftHeld()
{
    FAIL_RETURN(CheckThread());
    return NOERROR;
}

/**
 * @deprecated WebView no longer needs to implement
 * ViewGroup.OnHierarchyChangeListener.  This method does nothing now.
 */
// Cannot add @hide as this can always be accessed via the interface.
ECode WebView::OnChildViewAdded(
    /* [in] */ IView* parent,
    /* [in] */ IView* child)
{
    return NOERROR;
}

/**
 * @deprecated WebView no longer needs to implement
 * ViewGroup.OnHierarchyChangeListener.  This method does nothing now.
 */
// Cannot add @hide as this can always be accessed via the interface.
ECode WebView::OnChildViewRemoved(
    /* [in] */ IView* p,
    /* [in] */ IView* child)
{
    return NOERROR;
}

/**
 * @deprecated WebView should not have implemented
 * ViewTreeObserver.OnGlobalFocusChangeListener. This method does nothing now.
 */
// Cannot add @hide as this can always be accessed via the interface.
ECode WebView::OnGlobalFocusChanged(
    /* [in] */ IView* oldFocus,
    /* [in] */ IView* newFocus)
{
    return NOERROR;
}

/**
 * @deprecated Only the default case, true, will be supported in a future version.
 */
ECode WebView::SetMapTrackballToArrowKeys(
    /* [in] */ Boolean setMap)
{
    FAIL_RETURN(CheckThread());
    return mProvider->SetMapTrackballToArrowKeys(setMap);
}

ECode WebView::FlingScroll(
    /* [in] */ Int32 vx,
    /* [in] */ Int32 vy)
{
    FAIL_RETURN(CheckThread());
    if (DebugFlags::TRACE_API) {
        Logger::D(LOGTAG, "flingScroll");
    }
    return mProvider->FlingScroll(vx, vy);
}

/**
 * Gets the zoom controls for this WebView, as a separate View. The caller
 * is responsible for inserting this View into the layout hierarchy.
 * <p/>
 * API level {@link android.os.Build.VERSION_CODES#CUPCAKE} introduced
 * built-in zoom mechanisms for the WebView, as opposed to these separate
 * zoom controls. The built-in mechanisms are preferred and can be enabled
 * using {@link WebSettings#setBuiltInZoomControls}.
 *
 * @deprecated the built-in zoom mechanisms are preferred
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN}
 */
ECode WebView::GetZoomControls(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    FAIL_RETURN(CheckThread());
    return mProvider->GetZoomControls(view);
}

/**
 * Gets whether this WebView can be zoomed in.
 *
 * @return true if this WebView can be zoomed in
 *
 * @deprecated This method is prone to inaccuracy due to race conditions
 * between the web rendering and UI threads; prefer
 * {@link WebViewClient#onScaleChanged}.
 */
ECode WebView::CanZoomIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->CanZoomIn(result);
}

/**
 * Gets whether this WebView can be zoomed out.
 *
 * @return true if this WebView can be zoomed out
 *
 * @deprecated This method is prone to inaccuracy due to race conditions
 * between the web rendering and UI threads; prefer
 * {@link WebViewClient#onScaleChanged}.
 */
ECode WebView::CanZoomOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->CanZoomOut(result);
}

ECode WebView::ZoomBy(
        /* [in] */ Float zoomFactor)
{
    FAIL_RETURN(CheckThread());

    if (zoomFactor < 0.01) {
        //throw new IllegalArgumentException("zoomFactor must be greater than 0.01.");
        Logger::E("WebView::ZoomBy", "zoomFactor must be greater than 0.01.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (zoomFactor > 100.0) {
        //throw new IllegalArgumentException("zoomFactor must be less than 100.");
        Logger::E("WebView::ZoomBy", "zoomFactor must be less than 100.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean result;
    return mProvider->ZoomBy(zoomFactor, &result);
}

/**
 * Performs zoom in in this WebView.
 *
 * @return true if zoom in succeeds, false if no zoom changes
 */
ECode WebView::ZoomIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->ZoomIn(result);
}

/**
 * Performs zoom out in this WebView.
 *
 * @return true if zoom out succeeds, false if no zoom changes
 */
ECode WebView::ZoomOut(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(CheckThread());
    return mProvider->ZoomOut(result);
}

/**
 * @deprecated This method is now obsolete.
 * @hide Since API level {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR1}
 */
ECode WebView::DebugDump()
{
    FAIL_RETURN(CheckThread());
    return NOERROR;
}

/**
 * See {@link ViewDebug.HierarchyHandler#dumpViewHierarchyWithProperties(BufferedWriter, int)}
 * @hide
 */
ECode WebView::DumpViewHierarchyWithProperties(
    /* [in] */ IBufferedWriter* out,
    /* [in] */ Int32 level)
{
    return mProvider->DumpViewHierarchyWithProperties(out, level);
}

/**
 * See {@link ViewDebug.HierarchyHandler#findHierarchyView(String, int)}
 * @hide
 */
AutoPtr<IView> WebView::FindHierarchyView(
    /* [in] */ const String& className,
    /* [in] */ Int32 hashCode)
{
    AutoPtr<IView> view;
    mProvider->FindHierarchyView(className, hashCode, (IView**)&view);
    return view;
}

//-------------------------------------------------------------------------
// Interface for WebView providers
//-------------------------------------------------------------------------

/**
 * Gets the WebViewProvider. Used by providers to obtain the underlying
 * implementation, e.g. when the appliction responds to
 * WebViewClient.onCreateWindow() request.
 *
 * @hide WebViewProvider is not public API.
 */
ECode WebView::GetWebViewProvider(
    /* [out] */ IWebViewProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

//-------------------------------------------------------------------------
// Private internal stuff
//-------------------------------------------------------------------------

// Only used by android.webkit.FindActionModeCallback.
void WebView::SetFindDialogFindListener(
    /* [in] */ IWebViewFindListener* listener)
{
    CheckThread();
    SetupFindListenerIfNeeded();
    mFindListener->mFindDialogFindListener = listener;
}

// Only used by android.webkit.FindActionModeCallback.
void WebView::NotifyFindDialogDismissed()
{
    CheckThread();
    mProvider->NotifyFindDialogDismissed();
}

void WebView::SetupFindListenerIfNeeded()
{
    if (mFindListener == NULL) {
        mFindListener = new FindListenerDistributor(this);
        mProvider->SetFindListener(mFindListener);
    }
}

void WebView::EnsureProviderCreated()
{
    CheckThread();
    if (mProvider == NULL) {
        // As this can get called during the base class constructor chain, pass the minimum
        // number of dependencies here; the rest are deferred to init().
        AutoPtr<IWebViewPrivateAccess> pa = new PrivateAccess(this);
        IWebView* wv = this;
        assert(wv != NULL);
        GetFactory()->CreateWebView(wv, pa, (IWebViewProvider**)&mProvider);
    }
}

AutoPtr<IWebViewFactoryProvider> WebView::GetFactory()
{
    AutoLock lock(sLock);
    return WebViewFactory::GetProvider();
}

ECode WebView::CheckThread()
{
    // Ignore mWebViewThread == null because this can be called during in the super class
    // constructor, before this class's own constructor has even started.
    if (mWebViewThread != NULL && Looper::GetMyLooper() != mWebViewThread) {
        // Throwable throwable = new Throwable(
        //         "A WebView method was called on thread '" +
        //         Thread.currentThread().getName() + "'. " +
        //         "All WebView methods must be called on the same thread. " +
        //         "(Expected Looper " + mWebViewThread + " called on " + Looper.myLooper() +
        //         ", FYI main Looper is " + Looper.getMainLooper() + ")");
        // Log.w(LOGTAG, Log.getStackTraceString(throwable));
        // StrictMode.onWebViewMethodCalledOnWrongThread(throwable);
        Logger::E("WebView::CheckThread", "called on the wrong thread");
        AutoPtr<IStrictMode> sm;
        CStrictMode::AcquireSingleton((IStrictMode**)&sm);
        sm->OnWebViewMethodCalledOnWrongThread();

        if (sEnforceThreadChecking) {
            //throw new RuntimeException(throwable);
            Logger::E("WebView::CheckThread", "called on the wrong thread");
            return E_RUNTIME_EXCEPTION;
        }
    }

    return NOERROR;
}

//-------------------------------------------------------------------------
// Override View methods
//-------------------------------------------------------------------------

// TODO: Add a test that enumerates all methods in ViewDelegte & ScrollDelegate, and ensures
// there's a corresponding override (or better, caller) for each of them in here.

ECode WebView::OnAttachedToWindow()
{
    FAIL_RETURN(AbsoluteLayout::OnAttachedToWindow());
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    return viewDelegate->OnAttachedToWindow();
}

ECode WebView::OnDetachedFromWindowInternal()
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnDetachedFromWindow();
    return AbsoluteLayout::OnDetachedFromWindow();
}

ECode WebView::SetLayoutParams(
    /* [in] */ IViewGroupLayoutParams* params)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->SetLayoutParams(params);
    return NOERROR;
}

ECode WebView::SetOverScrollMode(
    /* [in] */ Int32 mode)
{
    FAIL_RETURN(AbsoluteLayout::SetOverScrollMode(mode));

    // This method may be called in the constructor chain, before the WebView provider is
    // created.
    EnsureProviderCreated();
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->SetOverScrollMode(mode);
    return NOERROR;
}

ECode WebView::SetScrollBarStyle(
    /* [in] */ Int32 style)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->SetScrollBarStyle(style);
    return AbsoluteLayout::SetScrollBarStyle(style);
}

Int32 WebView::ComputeHorizontalScrollRange()
{
    AutoPtr<IWebViewProviderScrollDelegate> scrollDelegate;
    mProvider->GetScrollDelegate((IWebViewProviderScrollDelegate**)&scrollDelegate);
    Int32 range;
    scrollDelegate->ComputeHorizontalScrollRange(&range);
    return range;
}

Int32 WebView::ComputeHorizontalScrollOffset()
{
    AutoPtr<IWebViewProviderScrollDelegate> scrollDelegate;
    mProvider->GetScrollDelegate((IWebViewProviderScrollDelegate**)&scrollDelegate);
    Int32 offset;
    scrollDelegate->ComputeHorizontalScrollOffset(&offset);
    return offset;
}

Int32 WebView::ComputeVerticalScrollRange()
{
    AutoPtr<IWebViewProviderScrollDelegate> scrollDelegate;
    mProvider->GetScrollDelegate((IWebViewProviderScrollDelegate**)&scrollDelegate);
    Int32 range;
    scrollDelegate->ComputeVerticalScrollRange(&range);
    return range;
}

Int32 WebView::ComputeVerticalScrollOffset()
{
    AutoPtr<IWebViewProviderScrollDelegate> scrollDelegate;
    mProvider->GetScrollDelegate((IWebViewProviderScrollDelegate**)&scrollDelegate);
    Int32 offset;
    scrollDelegate->ComputeVerticalScrollOffset(&offset);
    return offset;
}

Int32 WebView::ComputeVerticalScrollExtent()
{
    AutoPtr<IWebViewProviderScrollDelegate> scrollDelegate;
    mProvider->GetScrollDelegate((IWebViewProviderScrollDelegate**)&scrollDelegate);
    Int32 extent;
    scrollDelegate->ComputeVerticalScrollExtent(&extent);
    return extent;
}

ECode WebView::ComputeScroll()
{
    AutoPtr<IWebViewProviderScrollDelegate> scrollDelegate;
    mProvider->GetScrollDelegate((IWebViewProviderScrollDelegate**)&scrollDelegate);
    scrollDelegate->ComputeScroll();
    return NOERROR;
}

Boolean WebView::OnHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnHoverEvent(event, &result);
    return result;
}

Boolean WebView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnTouchEvent(event, &result);
    return result;
}

Boolean WebView::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnGenericMotionEvent(event, &result);
    return result;
}

Boolean WebView::OnTrackballEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnTrackballEvent(event, &result);
    return result;
}

Boolean WebView::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnKeyDown(keyCode, event, &result);
    return result;
}

Boolean WebView::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnKeyUp(keyCode, event, &result);
    return result;
}

Boolean WebView::OnKeyMultiple(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 repeatCount,
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->OnKeyMultiple(keyCode, repeatCount, event, &result);
    return result;
}

ECode WebView::GetAccessibilityNodeProvider(
    /* [out] */ IAccessibilityNodeProvider** provider)
{
    VALIDATE_NOT_NULL(provider);

    AutoPtr<IWebViewProviderViewDelegate> delegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&delegate);
    delegate->GetAccessibilityNodeProvider(provider);
    if (provider == NULL) {
        return AbsoluteLayout::GetAccessibilityNodeProvider(provider);
    }
    return NOERROR;
}

Boolean WebView::ShouldDelayChildPressedState()
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->ShouldDelayChildPressedState(&result);
    return result;
}

ECode WebView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FAIL_RETURN(AbsoluteLayout::OnInitializeAccessibilityNodeInfo(info));
    AutoPtr<ICharSequence> cs;
    CString::New(String("Elastos::Droid::Webkit::CWebView")/*WebView.class.getName()*/, (ICharSequence**)&cs);
    info->SetClassName(cs);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnInitializeAccessibilityNodeInfo(info);
    return NOERROR;
}

ECode WebView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FAIL_RETURN(AbsoluteLayout::OnInitializeAccessibilityEvent(event));
    AutoPtr<ICharSequence> cs;
    CString::New(String("Elastos::Droid::Webkit::CWebView")/*WebView.class.getName()*/, (ICharSequence**)&cs);
    IAccessibilityRecord::Probe(event)->SetClassName(cs);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnInitializeAccessibilityEvent(event);
    return NOERROR;
}

Boolean WebView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->PerformAccessibilityAction(action, arguments, &result);
    return result;
}

/** @hide */
void WebView::OnDrawVerticalScrollBar(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IDrawable* scrollBar,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnDrawVerticalScrollBar(canvas, scrollBar, l, t, r, b);
}

void WebView::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnOverScrolled(scrollX, scrollY, clampedX, clampedY);
}

void WebView::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    AbsoluteLayout::OnWindowVisibilityChanged(visibility);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnWindowVisibilityChanged(visibility);
}

void WebView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnDraw(canvas);
}

Boolean WebView::PerformLongClick()
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result = FALSE;
    viewDelegate->PerformLongClick(&result);
    return result;
}

void WebView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnConfigurationChanged(newConfig);
}

AutoPtr<IInputConnection> WebView::OnCreateInputConnection(
    /* [in] */ IEditorInfo* outAttrs)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    AutoPtr<IInputConnection> conn;
    viewDelegate->OnCreateInputConnection(outAttrs, (IInputConnection**)&conn);
    return conn;
}

ECode WebView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    AbsoluteLayout::OnVisibilityChanged(changedView, visibility);
    // This method may be called in the constructor chain, before the WebView provider is
    // created.
    EnsureProviderCreated();
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    return viewDelegate->OnVisibilityChanged(changedView, visibility);
}

ECode WebView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnWindowFocusChanged(hasWindowFocus);
    AbsoluteLayout::OnWindowFocusChanged(hasWindowFocus);
    return NOERROR;
}

void WebView::OnFocusChanged(
    /* [in] */ Boolean focused,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnFocusChanged(focused, direction, previouslyFocusedRect);
    AbsoluteLayout::OnFocusChanged(focused, direction, previouslyFocusedRect);
}

/** @hide */
Boolean WebView::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result = FALSE;
    viewDelegate->SetFrame(left, top, right, bottom, &result);
    return result;
}

void WebView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 ow,
    /* [in] */ Int32 oh)
{
    AbsoluteLayout::OnSizeChanged(w, h, ow, oh);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnSizeChanged(w, h, ow, oh);
}

void WebView::OnScrollChanged(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 oldl,
    /* [in] */ Int32 oldt)
{
    AbsoluteLayout::OnScrollChanged(l, t, oldl, oldt);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnScrollChanged(l, t, oldl, oldt);
}

Boolean WebView::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->DispatchKeyEvent(event, &result);
    return result;
}

Boolean WebView::RequestFocus(
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->RequestFocus(direction, previouslyFocusedRect, &result);
    return result;
}

void WebView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AbsoluteLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

ECode WebView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    viewDelegate->PreDispatchDraw(canvas);
    AbsoluteLayout::DispatchDraw(canvas);
    return NOERROR;
}

Boolean WebView::RequestChildRectangleOnScreen(
    /* [in] */ IView* child,
    /* [in] */ IRect* rect,
    /* [in] */ Boolean immediate)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    Boolean result;
    viewDelegate->RequestChildRectangleOnScreen(child, rect, immediate, &result);
    return result;
}

ECode WebView::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    return viewDelegate->SetBackgroundColor(color);
}

ECode WebView::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    AbsoluteLayout::SetLayerType(layerType, paint);
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    return viewDelegate->SetLayerType(layerType, paint);
}

ECode WebView::OnStartTemporaryDetach()
{
    AbsoluteLayout::OnStartTemporaryDetach();
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    return viewDelegate->OnStartTemporaryDetach();
}

ECode WebView::OnFinishTemporaryDetach()
{
    AbsoluteLayout::OnFinishTemporaryDetach();
    AutoPtr<IWebViewProviderViewDelegate> viewDelegate;
    mProvider->GetViewDelegate((IWebViewProviderViewDelegate**)&viewDelegate);
    return viewDelegate->OnFinishTemporaryDetach();
}

ECode WebView::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebView";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
