// wuweizuo automatic build .cpp file from .java file.

#include "elastos/droid/webkit/webview/chromium/native/content/browser/InterstitialPageDelegateElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/InterstitialPageDelegateElastos_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                   InterstitialPageDelegateElastos
//=====================================================================
InterstitialPageDelegateElastos::InterstitialPageDelegateElastos(
    /* [in] */ const String& htmlContent)
{
    mNativePtr = NativeInit(htmlContent);
}

Handle64 InterstitialPageDelegateElastos::GetNative()
{
    return mNativePtr;
}

ECode InterstitialPageDelegateElastos::OnProceed()
{
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::OnDontProceed()
{
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::CommandReceived(
    /* [in] */ const String& command)
{
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::Proceed()
{
    if (mNativePtr != 0) {
        NativeProceed(mNativePtr);
    }

    return NOERROR;
}

ECode InterstitialPageDelegateElastos::DontProceed()
{
    if (mNativePtr != 0) {
        NativeDontProceed(mNativePtr);
    }

    return NOERROR;
}

ECode InterstitialPageDelegateElastos::OnNativeDestroyed()
{
    mNativePtr = 0;
    return NOERROR;
}

Handle64 InterstitialPageDelegateElastos::NativeInit(
    /* [in] */ const String& htmlContent)
{
    return Elastos_InterstitialPageDelegateAndroid_nativeInit(THIS_PROBE(IInterface), htmlContent);
}

ECode InterstitialPageDelegateElastos::NativeProceed(
    /* [in] */ Handle64 nativeInterstitialPageDelegateElastos)
{
    Elastos_InterstitialPageDelegateAndroid_nativeProceed(THIS_PROBE(IInterface),
            nativeInterstitialPageDelegateElastos);
    return NOERROR;
}

ECode InterstitialPageDelegateElastos::NativeDontProceed(
    /* [in] */ Handle64 nativeInterstitialPageDelegateElastos)
{
    Elastos_InterstitialPageDelegateAndroid_nativeDontProceed(THIS_PROBE(IInterface),
            nativeInterstitialPageDelegateElastos);
    return NOERROR;
}

void InterstitialPageDelegateElastos::OnProceed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::OnProceed, mObj is NULL");
        return;
    }
    mObj->OnProceed();
}

void InterstitialPageDelegateElastos::OnDontProceed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::OnDontProceed, mObj is NULL");
        return;
    }
    mObj->OnDontProceed();
}

void InterstitialPageDelegateElastos::CommandReceived(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& command)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::CommandReceived, mObj is NULL");
        return;
    }
    mObj->CommandReceived(command);
}

void InterstitialPageDelegateElastos::OnNativeDestroyed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<InterstitialPageDelegateElastos> mObj = (InterstitialPageDelegateElastos*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterstitialPageDelegateElastos", "InterstitialPageDelegateElastos::OnNativeDestroyed, mObj is NULL");
        return;
    }
    mObj->OnNativeDestroyed();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
