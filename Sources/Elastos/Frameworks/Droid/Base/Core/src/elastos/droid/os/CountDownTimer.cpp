
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/CountDownTimer.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::ISynchronize;
using Elastos::Core::EIID_ISynchronize;

namespace Elastos {
namespace Droid {
namespace Os {

const Int32 CountDownTimer::MSG = 1;

CAR_INTERFACE_IMPL(CountDownTimer, Object, ICountDownTimer)

CountDownTimer::CountDownTimer()
    : mMillisInFuture(0)
    , mCountdownInterval(0)
    , mStopTimeInFuture(0)
    , mCancelled(FALSE)
{}

CountDownTimer::~CountDownTimer()
{}

ECode CountDownTimer::constructor(
    /* [in] */ Int64 millisInFuture,
    /* [in] */ Int64 countDownInterval)
{
    mMillisInFuture = millisInFuture;
    mCountdownInterval = countDownInterval;

    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mHandler = new MyHandler(wr);
    mHandler->constructor();
    return NOERROR;
}

ECode CountDownTimer::Cancel()
{
    mCancelled = TRUE;
    mHandler->RemoveMessages(MSG);
    return NOERROR;
}

ECode CountDownTimer::Start()
{
    mCancelled = FALSE;
    if (mMillisInFuture <= 0) {
        OnFinish();
        return NOERROR;
    }

    mStopTimeInFuture = SystemClock::GetElapsedRealtime() + mMillisInFuture;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

CountDownTimer::MyHandler::MyHandler(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

ECode CountDownTimer::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj == NULL) {
        return NOERROR;
    }

    ISynchronize* sync = ISynchronize::Probe(obj);
    CountDownTimer* ctd = (CountDownTimer*)sync;

    {    AutoLock syncLock(obj);
        if (ctd->mCancelled) {
            return NOERROR;
        }

        Int64 millisLeft = ctd->mStopTimeInFuture - SystemClock::GetElapsedRealtime();

        if (millisLeft <= 0) {
            ctd->OnFinish();
        }
        else if (millisLeft < ctd->mCountdownInterval) {
            AutoPtr<IMessage> msg;
            ObtainMessage(CountDownTimer::MSG, (IMessage**)&msg);
            Boolean bval;
            // no tick, just delay until done
            SendMessageDelayed(msg, millisLeft, &bval);
        }
        else {
            Int64 lastTickStart = SystemClock::GetElapsedRealtime();
            ctd->OnTick(millisLeft);

            // take into account user's onTick taking time to execute
            Int64 delay = lastTickStart + ctd->mCountdownInterval - SystemClock::GetElapsedRealtime();

            // special case: user's onTick took more than interval to
            // complete, skip to next interval
            while (delay < 0) delay += ctd->mCountdownInterval;

            AutoPtr<IMessage> msg;
            ObtainMessage(CountDownTimer::MSG, (IMessage**)&msg);
            Boolean bval;
            SendMessageDelayed(msg, delay, &bval);
        }
    }
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
