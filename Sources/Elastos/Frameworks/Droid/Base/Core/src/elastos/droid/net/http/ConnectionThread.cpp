
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Apache.h>
#include "elastos/droid/net/http/ConnectionThread.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/Request.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::SystemClock;

using Elastos::Core::StringUtils;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

const Int32 ConnectionThread::WAIT_TIMEOUT = 5000;
const Int32 ConnectionThread::WAIT_TICK = 1000;

ConnectionThread::ConnectionThread()
    : mCurrentThreadTime(0)
    , mTotalThreadTime(0)
    , mWaiting(FALSE)
    , mRunning(TRUE)
    , mId(0)
{}

ECode ConnectionThread::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [in] */ IRequestQueueConnectionManager* connectionManager,
    /* [in] */ IRequestFeeder* requestFeeder)
{
    Thread::constructor();
    mContext = context;
    SetName(String("http") + StringUtils::ToString(id));
    mId = id;
    mConnectionManager = connectionManager;
    mRequestFeeder = requestFeeder;
    return NOERROR;
}

ECode ConnectionThread::RequestStop()
{
    {    AutoLock syncLock(mRequestFeeder);
        mRunning = FALSE;
        ISynchronize::Probe(mRequestFeeder)->Notify();
    }
    return NOERROR;
}

ECode ConnectionThread::Run()
{
    Process::SetThreadPriority(
            IProcess::THREAD_PRIORITY_DEFAULT +
            IProcess::THREAD_PRIORITY_LESS_FAVORABLE);

    // these are used to get performance data. When it is not in the timing,
    // mCurrentThreadTime is 0. When it starts timing, mCurrentThreadTime is
    // first set to -1, it will be set to the current thread time when the
    // next request starts.
    mCurrentThreadTime = 0;
    mTotalThreadTime = 0;

    while (mRunning) {
        if (mCurrentThreadTime == -1) {
            mCurrentThreadTime = SystemClock::GetCurrentThreadTimeMillis();
        }

        AutoPtr<IRequest> request;

        /* Get a request to process */
        mRequestFeeder->GetRequest((IRequest**)&request);

        /* wait for work */
        if (request == NULL) {
            {    AutoLock syncLock(mRequestFeeder);
                if (HttpLog::LOGV) HttpLog::V("ConnectionThread: Waiting for work");
                mWaiting = TRUE;
                ISynchronize::Probe(mRequestFeeder)->Wait();
                mWaiting = false;

                if (mCurrentThreadTime != 0) {
                    mCurrentThreadTime = SystemClock::GetCurrentThreadTimeMillis();
                }
            }
        }
        else {
            if (HttpLog::LOGV) {
                HttpLog::V("ConnectionThread: new request %s %s",
                        Object::ToString(((Request*)request.Get())->mHost).string(), TO_CSTR(request));
            }

            mConnectionManager->GetConnection(mContext, ((Request*)request.Get())->mHost, (IConnection**)&mConnection);
            ((Connection*)mConnection.Get())->ProcessRequests(request);
            if (Ptr((Connection*)mConnection.Get())->Func(((Connection*)mConnection.Get())->GetCanPersist)) {
                Boolean isRecycleConnectionOk;
                mConnectionManager->RecycleConnection(mConnection, &isRecycleConnectionOk);
                if (!isRecycleConnectionOk) {
                    ((Connection*)mConnection.Get())->CloseConnection();
                }
            } else {
                ((Connection*)mConnection.Get())->CloseConnection();
            }
            mConnection = NULL;

            if (mCurrentThreadTime > 0) {
                Int64 start = mCurrentThreadTime;
                mCurrentThreadTime = SystemClock::GetCurrentThreadTimeMillis();
                mTotalThreadTime += mCurrentThreadTime - start;
            }
        }

    }
    return NOERROR;
}

ECode ConnectionThread::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String active = mWaiting ? String("w") : String("a");
    *result = String("cid ") + StringUtils::ToString(mId) + " " + active + " " + Object::ToString(mConnection);
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
