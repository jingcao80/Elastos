#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATE_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/components/WebContentsDelegateElastos.h"

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
using Elastos::Droid::Webkit::Webview::Chromium::Components::WebContentsDelegateElastos;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * WebView-specific WebContentsDelegate.
 * This file is the Java version of the native class of the same name.
 * It should contain abstract WebContentsDelegate methods to be implemented by the embedder.
 * These methods belong to WebView but are not shared with the Chromium Android port.
 */
//@VisibleForTesting
//@JNINamespace("android_webview")
class AwWebContentsDelegate
    : public WebContentsDelegateElastos
{
public:
    // Callback filesSelectedInChooser() when done.
    //@CalledByNative
    virtual CARAPI_(void) RunFileChooser(
        /* [in] */ Int32 processId,
        /* [in] */ Int32 renderId,
        /* [in] */ Int32 mode_flags,
        /* [in] */ const String& acceptTypes,
        /* [in] */ const String& title,
        /* [in] */ const String& defaultFilename,
        /* [in] */ Boolean capture) = 0;

    //@CalledByNative
    virtual CARAPI_(Boolean) AddNewContents(
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture) = 0;

    //@Override
    //@CalledByNative
    CARAPI CloseContents() = 0;

    //@Override
    //@CalledByNative
    CARAPI ActivateContents() = 0;

    //@Override
    //@CalledByNative
    CARAPI ToggleFullscreenModeForTab(
        /* [in] */ Boolean enterFullscreen) = 0;

protected:
    // Call in response to a prior runFileChooser call.
    static CARAPI_(void) NativeFilesSelectedInChooser(
        /* [in] */ Int32 processId,
        /* [in] */ Int32 renderId,
        /* [in] */ Int32 mode_flags,
        /* [in] */ ArrayOf<String>* filePath,
        /* [in] */ ArrayOf<String>* displayName);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwWebContentsDelegateCallback_Init();

private:
    static CARAPI_(void) RunFileChooser(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 processId,
        /* [in] */ Int32 renderId,
        /* [in] */ Int32 mode_flags,
        /* [in] */ const String& acceptTypes,
        /* [in] */ const String& title,
        /* [in] */ const String& defaultFilename,
        /* [in] */ Boolean capture);

    static CARAPI_(Boolean) AddNewContents(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture);

    static CARAPI_(void) CloseContents(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ActivateContents(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ToggleFullscreenModeForTab(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean enterFullscreen);

};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATE_H__
