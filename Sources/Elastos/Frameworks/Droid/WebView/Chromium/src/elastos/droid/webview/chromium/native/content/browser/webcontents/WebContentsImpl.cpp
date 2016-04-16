
#include "elastos/droid/webkit/webview/chromium/native/content/browser/webcontents/WebContentsImpl.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/WebContentsImpl_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Webcontents {

WebContentsImpl::WebContentsImpl(
    /* [in] */ Handle64 nativeWebContentsAndroid,
    /* [in] */ NavigationController* navigationController)
    : mNativeWebContentsAndroid(nativeWebContentsAndroid)
    , mNavigationController(navigationController)
{
}

//@CalledByNative
AutoPtr<WebContentsImpl> WebContentsImpl::Create(
    /* [in] */ Handle64 nativeWebContentsAndroid,
    /* [in] */ NavigationController* navigationController)
{
    return new WebContentsImpl(nativeWebContentsAndroid, navigationController);
}

//@CalledByNative
void WebContentsImpl::Destroy()
{
    mNativeWebContentsAndroid = 0;
    mNavigationController = NULL;
}

//@CalledByNative
Handle64 WebContentsImpl::GetNativePointer()
{
    return mNativeWebContentsAndroid;
}

//@Override
AutoPtr<NavigationController> WebContentsImpl::GetNavigationController()
{
    return mNavigationController;
}

//@Override
String WebContentsImpl::GetTitle()
{
    return NativeGetTitle(mNativeWebContentsAndroid);
}

//@Override
String WebContentsImpl::GetVisibleUrl()
{
    return NativeGetVisibleURL(mNativeWebContentsAndroid);
}

//@Override
void WebContentsImpl::Stop()
{
    NativeStop(mNativeWebContentsAndroid);
}

String WebContentsImpl::NativeGetTitle(
    /* [in] */ Handle64 nativeWebContentsAndroid)
{
    return Elastos_WebContentsImpl_nativeGetTitle(TO_IINTERFACE(this), nativeWebContentsAndroid);
}

String WebContentsImpl::NativeGetVisibleURL(
    /* [in] */ Handle64 nativeWebContentsAndroid)
{
    return Elastos_WebContentsImpl_nativeGetVisibleURL(TO_IINTERFACE(this), nativeWebContentsAndroid);
}

void WebContentsImpl::NativeStop(
    /* [in] */ Handle64 nativeWebContentsAndroid)
{
    Elastos_WebContentsImpl_nativeStop(TO_IINTERFACE(this), nativeWebContentsAndroid);
}

AutoPtr<IInterface> WebContentsImpl::Create(
    /* [in] */ Handle64 nativeWebContentsAndroid,
    /* [in] */ IInterface* navigationController)
{
    AutoPtr<NavigationController> nc = (NavigationController*)(IObject::Probe(navigationController));
    return TO_IINTERFACE(Create(nativeWebContentsAndroid, nc));
}

void WebContentsImpl::Destroy(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsImpl> mObj = (WebContentsImpl*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsImpl", "WebContentsImpl::Destroy, mObj is NULL");
        return;
    }
    mObj->Destroy();
}

Handle64 WebContentsImpl::GetNativePointer(
    /* [in] */ IInterface* obj)
{
    AutoPtr<WebContentsImpl> mObj = (WebContentsImpl*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("WebContentsImpl", "WebContentsImpl::GetNativePointer, mObj is NULL");
        return 0;
    }
    return mObj->GetNativePointer();
}

} // namespace Webcontents
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
