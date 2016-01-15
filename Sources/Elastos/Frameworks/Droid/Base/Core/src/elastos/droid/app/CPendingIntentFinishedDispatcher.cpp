
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/CPendingIntentFinishedDispatcher.h"
#include "unistd.h"

namespace Elastos {
namespace Droid {
namespace App {

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Content::EIID_IIntentReceiver;

CAR_INTERFACE_IMPL(CPendingIntentFinishedDispatcher, Runnable, IIntentReceiver)

CAR_OBJECT_IMPL(CPendingIntentFinishedDispatcher)

CPendingIntentFinishedDispatcher::CPendingIntentFinishedDispatcher()
    : mResultCode(0)
{}

CPendingIntentFinishedDispatcher::~CPendingIntentFinishedDispatcher()
{
    mWho = NULL;
    mHandler = NULL;
    mIntent = NULL;
    mResultExtras = NULL;
}

ECode CPendingIntentFinishedDispatcher::constructor(
    /* [in] */ IPendingIntent* pi,
    /* [in] */ IPendingIntentOnFinished* who,
    /* [in] */ IHandler* handler)
{
    mPendingIntent = pi;
    mWho = who;
    mHandler = handler;
    return NOERROR;
}

ECode CPendingIntentFinishedDispatcher::PerformReceive(
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

    Boolean result;
    if (NULL == mHandler) {
        FAIL_RETURN(Run());
    } else {
        mHandler->Post(IRunnable::Probe(this), &result);
    }
    return NOERROR;
}

ECode CPendingIntentFinishedDispatcher::Run()
{
    return mWho->OnSendFinished(mPendingIntent, mIntent, mResultCode, mResultData, mResultExtras);
}

ECode CPendingIntentFinishedDispatcher::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
