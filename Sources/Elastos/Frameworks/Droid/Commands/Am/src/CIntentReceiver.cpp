
#include "CIntentReceiver.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {


CIntentReceiver::CIntentReceiver()
    : mLock()
    , mFinished(FALSE)
{}

ECode CIntentReceiver::PerformReceive(
    /* [in] */ IIntent * intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle * extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    String line("Broadcast completed: result=");
    line.AppendFormat("%d", resultCode);
    if (!data.IsNull()) {
        line.AppendFormat(", data=\"%s\"", data.string());
    }
    if (extras != NULL) {
        String extra;
        line.AppendFormat(", extras: %s", (extras->ToString(&extra), extra).string());
    }
    PFL_EX("%s", line.string());

    AutoLock lock(mLock);
    mFinished = TRUE;
    return mLock.NotifyAll();
}

CARAPI CIntentReceiver::WaitForFinish()
{
    AutoLock lock(mLock);
    ECode ec = NOERROR;
    while (!mFinished) {
        ECode ec = mLock.Wait();
    }
    return ec;
}

}
}
}
}

