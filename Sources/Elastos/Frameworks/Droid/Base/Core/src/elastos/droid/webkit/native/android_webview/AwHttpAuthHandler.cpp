#include "elastos/droid/webkit/native/android_webview/AwHttpAuthHandler.h"
#include "elastos/droid/webkit/native/android_webview/api/AwHttpAuthHandler_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

AwHttpAuthHandler::AwHttpAuthHandler(
    /* [in] */ Int64 nativeAwHttpAuthHandler,
    /* [in] */ Boolean firstAttempt)
    : mNativeAwHttpAuthHandler(nativeAwHttpAuthHandler)
    , mFirstAttempt(firstAttempt)
{
}

void AwHttpAuthHandler::Proceed(
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    if (mNativeAwHttpAuthHandler != 0) {
        NativeProceed(mNativeAwHttpAuthHandler, username, password);
        mNativeAwHttpAuthHandler = 0;
    }
}

void AwHttpAuthHandler::Cancel()
{
    if (mNativeAwHttpAuthHandler != 0) {
        NativeCancel(mNativeAwHttpAuthHandler);
        mNativeAwHttpAuthHandler = 0;
    }
}

Boolean AwHttpAuthHandler::IsFirstAttempt()
{
     return mFirstAttempt;
}

//@CalledByNative return AwHttpAuthHandler
AutoPtr<IInterface> AwHttpAuthHandler::Create(
    /* [in] */ Int64 nativeAwAuthHandler,
    /* [in] */ Boolean firstAttempt)
{
    AutoPtr<AwHttpAuthHandler> handler = new AwHttpAuthHandler(nativeAwAuthHandler, firstAttempt);
    AutoPtr<IInterface> result = TO_IINTERFACE(handler);
    return result;
}

//@CalledByNative
void AwHttpAuthHandler::HandlerDestroyed()
{
    mNativeAwHttpAuthHandler = 0;
}

void AwHttpAuthHandler::NativeProceed(
    /* [in] */ Int64 nativeAwHttpAuthHandler,
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    Elastos_AwHttpAuthHandler_nativeProceed(THIS_PROBE(IInterface), (Handle32)nativeAwHttpAuthHandler, username, password);
}

void AwHttpAuthHandler::NativeCancel(
    /* [in] */ Int64 nativeAwHttpAuthHandler)
{
    Elastos_AwHttpAuthHandler_nativeCancel(THIS_PROBE(IInterface), (Handle32)nativeAwHttpAuthHandler);
}
//callback function definition
void AwHttpAuthHandler::HandlerDestroyed(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwHttpAuthHandler> mObj = (AwHttpAuthHandler*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwHttpAuthHandler", "AwHttpAuthHandler::HandlerDestroyed, mObj is NULL");
        return;
    }
    mObj->HandlerDestroyed();
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
