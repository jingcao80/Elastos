// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_WEBCONTENTSOBSERVERANDROID_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_WEBCONTENTSOBSERVERANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content_public/browser/WebContents.h"
#include <elastos/core/Object.h>

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;

using Elastos::Droid::Webkit::Content_Public::Browser::WebContents;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class ContentViewCore;

/**
  * This class receives callbacks that act as hooks for various a native web contents events related
  * to loading a url. A single web contents can have multiple WebContentObserverAndroids.
  */
// @JNINamespace("content")
class WebContentsObserverElastos
    : public Object
{
public:
    // TODO(yfriedman): Switch everyone to use the WebContents constructor.
    WebContentsObserverElastos(
        /* [in] */ ContentViewCore* contentViewCore);

    WebContentsObserverElastos(
        /* [in] */ WebContents* webContents);

    // @CalledByNative
    virtual CARAPI RenderProcessGone(
        /* [in] */ Boolean wasOomProtected);

    /**
      * Called when the a page starts loading.
      * @param url The validated url for the loading page.
      */
    // @CalledByNative
    virtual CARAPI DidStartLoading(
        /* [in] */ const String& url);

    /**
      * Called when the a page finishes loading.
      * @param url The validated url for the page.
      */
    // @CalledByNative
    virtual CARAPI DidStopLoading(
        /* [in] */ const String& url);

    /**
      * Called when an error occurs while loading a page and/or the page fails to load.
      * @param errorCode Error code for the occurring error.
      * @param description The description for the error.
      * @param failingUrl The url that was loading when the error occurred.
      */
    // @CalledByNative
    virtual CARAPI DidFailLoad(
        /* [in] */ Boolean isProvisionalLoad,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& description,
        /* [in] */ const String& failingUrl);

    /**
      * Called when the main frame of the page has committed.
      * @param url The validated url for the page.
      * @param baseUrl The validated base url for the page.
      * @param isNavigationToDifferentPage Whether the main frame navigated to a different page.
      * @param isFragmentNavigation Whether the main frame navigation did not cause changes to the
      *                             document (for example scrolling to a named anchor or PopState).
      */
    // @CalledByNative
    virtual CARAPI DidNavigateMainFrame(
        /* [in] */ const String& url,
        /* [in] */ const String& baseUrl,
        /* [in] */ Boolean isNavigationToDifferentPage,
        /* [in] */ Boolean isFragmentNavigation);

    /**
      * Called when the page had painted something non-empty.
      */
    // @CalledByNative
    virtual CARAPI DidFirstVisuallyNonEmptyPaint();

    /**
      * Similar to didNavigateMainFrame but also called on subframe navigations.
      * @param url The validated url for the page.
      * @param baseUrl The validated base url for the page.
      * @param isReload True if this navigation is a reload.
      */
    // @CalledByNative
    virtual CARAPI DidNavigateAnyFrame(
        /* [in] */ const String& url,
        /* [in] */ const String& baseUrl,
        /* [in] */ Boolean isReload);

    /**
      * Notifies that a load is started for a given frame.
      * @param frameId A positive, non-zero integer identifying the navigating frame.
      * @param parentFrameId The frame identifier of the frame containing the navigating frame,
      *                      or -1 if the frame is not contained in another frame.
      * @param isMainFrame Whether the load is happening for the main frame.
      * @param validatedUrl The validated URL that is being navigated to.
      * @param isErrorPage Whether this is navigating to an error page.
      * @param isIframeSrcdoc Whether this is navigating to about:srcdoc.
      */
    // @CalledByNative
    virtual CARAPI DidStartProvisionalLoadForFrame(
        /* [in] */ Int64 frameId,
        /* [in] */ Int64 parentFrameId,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ const String& validatedUrl,
        /* [in] */ Boolean isErrorPage,
        /* [in] */ Boolean isIframeSrcdoc);

    /**
      * Notifies that the provisional load was successfully committed. The RenderViewHost is now
      * the current RenderViewHost of the WebContents.
      * @param frameId A positive, non-zero integer identifying the navigating frame.
      * @param isMainFrame Whether the load is happening for the main frame.
      * @param url The committed URL being navigated to.
      * @param transitionType The transition type as defined in
      *                      {@link org.chromium.content.browser.PageTransitionTypes} for the load.
      */
    // @CalledByNative
    virtual CARAPI DidCommitProvisionalLoadForFrame(
        /* [in] */ Int64 frameId,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ const String& url,
        /* [in] */ Int32 transitionType);

    /**
      * Notifies that a load has finished for a given frame.
      * @param frameId A positive, non-zero integer identifying the navigating frame.
      * @param validatedUrl The validated URL that is being navigated to.
      * @param isMainFrame Whether the load is happening for the main frame.
      */
    // @CalledByNative
    virtual CARAPI DidFinishLoad(
        /* [in] */ Int64 frameId,
        /* [in] */ const String& validatedUrl,
        /* [in] */ Boolean isMainFrame);

    /**
      * Notifies that the document has finished loading for the given frame.
      * @param frameId A positive, non-zero integer identifying the navigating frame.
      */
    // @CalledByNative
    virtual CARAPI DocumentLoadedInFrame(
        /* [in] */ Int64 frameId);

    /**
      * Notifies that a navigation entry has been committed.
      */
    // @CalledByNative
    virtual CARAPI NavigationEntryCommitted();

    /**
      * Called when an interstitial page gets attached to the tab content.
      */
    // @CalledByNative
    virtual CARAPI DidAttachInterstitialPage();

    /**
      * Called when an interstitial page gets detached from the tab content.
      */
    // @CalledByNative
    virtual CARAPI DidDetachInterstitialPage();

    /**
      * Called when the theme color was changed.
      * @param color the new color in ARGB format
      */
    // @CalledByNative
    virtual CARAPI DidChangeThemeColor(
        /* [in] */ Int32 color);

    /**
      * Destroy the corresponding native object.
      */
    // @CalledByNative
    virtual CARAPI DetachFromWebContents();

    static CARAPI_(void*) ElaWebContentsObserverElastosCallback_Init();

private:
    static CARAPI_(void) RenderProcessGone(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean wasOomProtected);

    static CARAPI_(void) DidStartLoading(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url);

    static CARAPI_(void) DidStopLoading(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url);

    static CARAPI_(void) DidFailLoad(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean isProvisionalLoad,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& description,
        /* [in] */ const String& failingUrl);

    static CARAPI_(void) DidNavigateMainFrame(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& baseUrl,
        /* [in] */ Boolean isNavigationToDifferentPage,
        /* [in] */ Boolean isFragmentNavigation);

    static CARAPI_(void) DidFirstVisuallyNonEmptyPaint(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) DidNavigateAnyFrame(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ const String& baseUrl,
        /* [in] */ Boolean isReload);

    static CARAPI_(void) DidStartProvisionalLoadForFrame(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 frameId,
        /* [in] */ Int64 parentFrameId,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ const String& validatedUrl,
        /* [in] */ Boolean isErrorPage,
        /* [in] */ Boolean isIframeSrcdoc);

    static CARAPI_(void) DidCommitProvisionalLoadForFrame(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 frameId,
        /* [in] */ Boolean isMainFrame,
        /* [in] */ const String& url,
        /* [in] */ Int32 transitionType);

    static CARAPI_(void) DidFinishLoad(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 frameId,
        /* [in] */ const String& validatedUrl,
        /* [in] */ Boolean isMainFrame);

    static CARAPI_(void) DocumentLoadedInFrame(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 frameId);

    static CARAPI_(void) NavigationEntryCommitted(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) DidAttachInterstitialPage(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) DidDetachInterstitialPage(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) DidChangeThemeColor(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 color);

    static CARAPI_(void) DetachFromWebContents(
        /* [in] */ IInterface* obj);

    CARAPI_(Int64) NativeInit(
        /* [in] */ WebContents* webContents);

    CARAPI NativeDestroy(
        /* [in] */ Int64 nativeWebContentsObserverElastos);

private:
    Int64 mNativeWebContentsObserverElastos;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_WEBCONTENTSOBSERVERANDROID_H_
