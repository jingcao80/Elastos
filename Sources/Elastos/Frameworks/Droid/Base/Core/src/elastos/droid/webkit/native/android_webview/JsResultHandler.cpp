#include "elastos/droid/webkit/native/android_webview/JsResultHandler.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClientBridge.h"
#include "elastos/droid/webkit/native/base/ThreadUtils.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Webkit::Base::ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {


#ifndef CAR_INTERFACE_IMPL_JSRESULTHANDLER
#define CAR_INTERFACE_IMPL_JSRESULTHANDLER(ClassName, SupperClassName)       \
                                                           \
    UInt32 ClassName::AddRef()                             \
    {                                                      \
        return SupperClassName::AddRef();                        \
    }                                                      \
                                                           \
    UInt32 ClassName::Release()                            \
    {                                                      \
        return SupperClassName::Release();                       \
    }                                                      \
                                                           \
    PInterface ClassName::Probe(                           \
        /* [in] */ REIID riid)                             \
    {                                                      \
        return SupperClassName::Probe(riid);               \
    }                                                      \
                                                           \
    ECode ClassName::GetInterfaceID(                       \
        /* [in] */ IInterface* object,                     \
        /* [out] */ InterfaceID* iid)                      \
    {                                                      \
        VALIDATE_NOT_NULL(iid);                            \
                                                           \
        return SupperClassName::GetInterfaceID(object, iid); \
    }
#endif
CAR_INTERFACE_IMPL_JSRESULTHANDLER(JsResultHandler, Object);
//===============================================================
//               JsResultHandler::ConfirmRunnable
//===============================================================
CAR_INTERFACE_IMPL(JsResultHandler::ConfirmRunnable, Object, IRunnable);

JsResultHandler::ConfirmRunnable::ConfirmRunnable(
    /* [in] */ JsResultHandler* owner,
    /* [in] */ const String& promptResult)
    : mOwner(owner)
    , mPromptResult(promptResult)
{
}

ECode JsResultHandler::ConfirmRunnable::Run()
{
    if (mOwner->mBridge != NULL)
        mOwner->mBridge->ConfirmJsResult(mOwner->mId, mPromptResult);
    mOwner->mBridge = NULL;

    return NOERROR;
}

//===============================================================
//               JsResultHandler::CancelRunnable
//===============================================================
CAR_INTERFACE_IMPL(JsResultHandler::CancelRunnable, Object, IRunnable);

JsResultHandler::CancelRunnable::CancelRunnable(
    /* [in] */ JsResultHandler* owner)
    : mOwner(owner)
{
}

ECode JsResultHandler::CancelRunnable::Run()
{
    if (mOwner->mBridge != NULL)
        mOwner->mBridge->CancelJsResult(mOwner->mId);
    mOwner->mBridge = NULL;

    return NOERROR;
}

//===============================================================
//                      JsResultHandler
//===============================================================

JsResultHandler::JsResultHandler(
    /* [in] */ AwContentsClientBridge* bridge,
    /* [in] */ Int32 id)
    : mBridge(bridge)
    , mId(id)
{
}

//@Override
ECode JsResultHandler::Confirm()
{
    return Confirm(String(NULL));
}

//@Override
ECode JsResultHandler::Confirm(
    /* [in] */ const String& promptResult)
{
    AutoPtr<IRunnable> runnable =  new ConfirmRunnable(this, promptResult);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

//@Override
ECode JsResultHandler::Cancel()
{
    AutoPtr<IRunnable> runnable =  new CancelRunnable(this);
    ThreadUtils::RunOnUiThread(runnable);
    return NOERROR;
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
