
#include "elastos/droid/content/CIntentSender.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include <elastos/core/StringBuilder.h>
#include <Elastos.Droid.App.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
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

namespace Elastos {
namespace Droid {
namespace Content {

//============================================================================
//              CIntentSender::FinishedDispatcher
//============================================================================
CAR_INTERFACE_IMPL_2(CIntentSender::FinishedDispatcher, Runnable, IIntentReceiver, IBinder)

CIntentSender::FinishedDispatcher::FinishedDispatcher(
    /* [in] */ CIntentSender* pi,
    /* [in] */ IIntentSenderOnFinished* who,
    /* [in] */ IHandler* handler)
    : mIntentSender(pi)
    , mWho(who)
    , mHandler(handler)
    , mResultCode(0)
{}

CIntentSender::FinishedDispatcher::~FinishedDispatcher()
{}

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
        Boolean result;
        return mHandler->Post(THIS_PROBE(IRunnable), &result);
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
    VALIDATE_NOT_NULL(description);
    *description = String("CIntentSender::FinishedDispatcher");
    return NOERROR;
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
    AutoPtr<FinishedDispatcher> dispatcher;
    if (NULL != onFinished) {
        dispatcher = new FinishedDispatcher(this, onFinished, handler);
    }

    String targetStr = Object::ToString(mTarget);

    Int32 res = 0;
    FAIL_RETURN(mTarget->Send(code, intent, resolvedType, dispatcher, requiredPermission, &res));

    if (res < 0) {
//        throw new SendIntentException();
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

