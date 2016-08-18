
#include "elastos/droid/content/CIntentSender.h"
#include "elastos/droid/content/CIntentSenderFinishedDispatcher.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.App.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Content::EIID_IIIntentSender;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

//============================================================================
//              CIntentSender::FinishedDispatcher::MyRunnable
//============================================================================
CIntentSender::FinishedDispatcher::MyRunnable::MyRunnable(
    /* [in] */ FinishedDispatcher* host)
    : mHost(host)
{
}

ECode CIntentSender::FinishedDispatcher::MyRunnable::Run()
{
    return mHost->Run();
}

//============================================================================
//              CIntentSender::FinishedDispatcher
//============================================================================
CAR_INTERFACE_IMPL_2(CIntentSender::FinishedDispatcher, Object, IIntentReceiver, IBinder)

CIntentSender::FinishedDispatcher::FinishedDispatcher()
    : mResultCode(0)
{}

CIntentSender::FinishedDispatcher::~FinishedDispatcher()
{}

ECode CIntentSender::FinishedDispatcher::constructor(
    /* [in] */ IIntentSender* pi,
    /* [in] */ IIntentSenderOnFinished* who,
    /* [in] */ IHandler* handler)
{
    mIntentSender = (CIntentSender*)pi;
    mWho = who;
    mHandler = handler;
    return NOERROR;
}


ECode CIntentSender::FinishedDispatcher::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String&  data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    mIntent = intent;
    mResultCode = resultCode;
    mResultData = data;
    mResultExtras = extras;

    if (NULL == mHandler) {
        return Run();
    }
    else {
        AutoPtr<IRunnable> runnable = new MyRunnable(this);
        Boolean result;
        return mHandler->Post(runnable, &result);
    }
}

ECode CIntentSender::FinishedDispatcher::Run()
{
    return mWho->OnSendFinished(
        mIntentSender, mIntent, mResultCode, mResultData, mResultExtras);
}

ECode CIntentSender::FinishedDispatcher::ToString(
     /* [out] */ String* description)
{
    return Object::ToString(description);
}

//============================================================================
//              CIntentSender
//============================================================================

CAR_INTERFACE_IMPL_2(CIntentSender, Object, IIntentSender, IParcelable)

CAR_OBJECT_IMPL(CIntentSender)

CIntentSender::CIntentSender()
{}

CIntentSender::~CIntentSender()
{}

ECode CIntentSender::constructor()
{
    return NOERROR;
}

ECode CIntentSender::constructor(
    /* [in] */ IIIntentSender* target)
{
    mTarget = target;
    return NOERROR;
}

ECode CIntentSender::constructor(
    /* [in] */ IBinder* target)
{
    mTarget = IIIntentSender::Probe(target);
    return NOERROR;
}

ECode CIntentSender::SendIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int32 code,
    /* [in] */ IIntent* intent,
    /* [in] */ IIntentSenderOnFinished* onFinished,
    /* [in] */ IHandler* handler)
{
    return SendIntent(context, code, intent, onFinished, handler, String(NULL));
}

ECode CIntentSender::SendIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int32 code,
    /* [in] */ IIntent* intent,
    /* [in] */ IIntentSenderOnFinished* onFinished,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& requiredPermission)
{
    String resolvedType;
    if (NULL != intent) {
        AutoPtr<IContentResolver> resolver;
        FAIL_RETURN(context->GetContentResolver((IContentResolver**)&resolver));
        FAIL_RETURN(intent->ResolveTypeIfNeeded(resolver, &resolvedType));
    }
    AutoPtr<IIntentReceiver> dispatcher;
    if (NULL != onFinished) {
        CIntentSenderFinishedDispatcher::New(this, onFinished, handler, (IIntentReceiver**)&dispatcher);
    }

    Int32 res = 0;
    FAIL_RETURN(mTarget->Send(code, intent, resolvedType, dispatcher, requiredPermission, &res));

    if (res < 0) {
        Logger::I("CIntentSender", "SendIntentException: code: %d, intent: %s", code, TO_CSTR(intent));
        return E_SEND_INTENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CIntentSender::GetTargetPackage(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return ActivityManagerNative::GetDefault()->GetPackageForIntentSender(mTarget, str);
}

ECode CIntentSender::GetCreatorPackage(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return ActivityManagerNative::GetDefault()->GetPackageForIntentSender(mTarget, str);
}

ECode CIntentSender::GetCreatorUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    return ActivityManagerNative::GetDefault()->GetUidForIntentSender(mTarget, uid);
}

ECode CIntentSender::GetCreatorUserHandle(
    /* [out] */ IUserHandle** userHandle)
{
    VALIDATE_NOT_NULL(userHandle)
    Int32 uid = 0;
    FAIL_RETURN(ActivityManagerNative::GetDefault()->GetUidForIntentSender(mTarget, &uid));
    AutoPtr<IUserHandle> handle;

    if (uid > 0) {
        AutoPtr<IUserHandleHelper> handleHelper;
        FAIL_RETURN(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper));
        Int32 userId = 0;
        FAIL_RETURN(handleHelper->GetUserId(uid, &userId));
        FAIL_RETURN(CUserHandle::New(userId, (IUserHandle**)&handle));
    }

    *userHandle = handle;
    REFCOUNT_ADD(*userHandle);
    return NOERROR;
}

ECode CIntentSender::Equals(
    /* [in] */ IInterface* otherObj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)

    if (IIntentSender::Probe(otherObj) != NULL) {
        //mTarget.asBinder().equals(((IntentSender)otherObj).mTarget.asBinder());
        AutoPtr<IIIntentSender> otherTarget;
        FAIL_RETURN(IIntentSender::Probe(otherObj)->GetTarget((IIIntentSender**)&otherTarget));
        *isEqual = Object::Equals(mTarget, otherTarget);
        return NOERROR;
    }

    *isEqual = FALSE;
    return NOERROR;
}

ECode CIntentSender::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (Int32)this;
    return NOERROR;
}

ECode CIntentSender::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(128);
    sb += "IntentSender{";
//    sb.append(Integer.toHexString(System.identityHashCode(this)));
    sb += ": ";
    sb += Object::ToString(mTarget);
    sb += '}';
    return sb.ToString(str);
}

ECode CIntentSender::GetTarget(
    /* [out] */ IIIntentSender** intentSender)
{
    VALIDATE_NOT_NULL(intentSender)
    *intentSender = mTarget;
    REFCOUNT_ADD(*intentSender);
    return NOERROR;
}

ECode CIntentSender::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&obj);
    mTarget = IIIntentSender::Probe(obj.Get());
    return NOERROR;
}

ECode CIntentSender::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    IBinder* binder = IBinder::Probe(mTarget.Get());
    return dest->WriteInterfacePtr((IInterface*)binder);
}

}
}
}

