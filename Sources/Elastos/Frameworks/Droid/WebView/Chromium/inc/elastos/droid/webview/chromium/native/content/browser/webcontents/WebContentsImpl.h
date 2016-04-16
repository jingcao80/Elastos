
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_WEBCONTENTS_WEBCONTENTSIMPL_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_WEBCONTENTS_WEBCONTENTSIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/NavigationController.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/WebContents.h"

using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Browser::NavigationController;
using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Browser::WebContents;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Webcontents {

/**
 * The WebContentsImpl Java wrapper to allow communicating with the native WebContentsImpl
 * object.
 */
//@JNINamespace("content")
//TODO(tedchoc): Remove the package restriction once this class moves to a non-public content
//               package whose visibility will be enforced via DEPS.
/* package */
class WebContentsImpl
    : public WebContents
{
public:
    //@Override
    CARAPI_(AutoPtr<NavigationController>) GetNavigationController();

    //@Override
    CARAPI_(String) GetTitle();

    //@Override
    CARAPI_(String) GetVisibleUrl();

    //@Override
    CARAPI_(void) Stop();

    static CARAPI_(void*) ElaWebContentsImplCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeWebContentsAndroid,
        /* [in] */ IInterface* navigationController);

    static CARAPI_(void) Destroy(
        /* [in] */ IInterface* obj);

    static CARAPI_(Handle64) GetNativePointer(
        /* [in] */ IInterface* obj);

    WebContentsImpl(
        /* [in] */ Handle64 nativeWebContentsAndroid,
        /* [in] */ NavigationController* navigationController);

    //@CalledByNative
    static CARAPI_(AutoPtr<WebContentsImpl>) Create(
        /* [in] */ Handle64 nativeWebContentsAndroid,
        /* [in] */ NavigationController* navigationController);

    //@CalledByNative
    CARAPI_(void) Destroy();

    //@CalledByNative
    CARAPI_(Handle64) GetNativePointer();

    CARAPI_(String) NativeGetTitle(
        /* [in] */ Handle64 nativeWebContentsAndroid);

    CARAPI_(String) NativeGetVisibleURL(
        /* [in] */ Handle64 nativeWebContentsAndroid);

    CARAPI_(void) NativeStop(
        /* [in] */ Handle64 nativeWebContentsAndroid);

private:
    Handle64 mNativeWebContentsAndroid;
    AutoPtr<NavigationController> mNavigationController;
};

} // namespace Webcontents
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_WEBCONTENTS_WEBCONTENTSIMPL_H__
