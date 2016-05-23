
#include <Elastos.CoreLibrary.Apache.h>
#include "elastos/droid/net/http/IdleCache.h"
#include "elastos/droid/net/http/Connection.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/ReturnOutValue.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;

using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;

using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

//===================================================
// IdleCache::IdleReaper
//===================================================
IdleCache::IdleReaper::IdleReaper(
    /* [in] */ IdleCache* host)
    : mHost(host)
{}

ECode IdleCache::IdleReaper::Run()
{
    Int32 check = 0;

    SetName(String("IdleReaper"));
    // AutoPtr<Elastos::Os::IProcessHelper> helper;
    // Elastos::os::CProcessHelper::AcquireSingleton((Elastos::Os::IProcessHelper**)&helper);
    // helper->SetThreadPriority(
    //         Elastos::os::Process::THREAD_PRIORITY_BACKGROUND);

    {
        {    AutoLock syncLock(mHost);

            while (check < EMPTY_CHECK_MAX) {
                Wait(CHECK_INTERVAL);

                if (mHost->mCount == 0) {
                    check++;
                } else {
                    check = 0;
                    mHost->ClearIdle();
                }
            }
            mHost->mThread = NULL;
        }
    }

    if (HttpLog::LOGV) {
        HttpLog::V(String("IdleCache IdleReaper shutdown: cached ") + StringUtils::ToString(mHost->mCached) +
                  String(" reused ") + StringUtils::ToString(mHost->mReused));
        mHost->mCached = 0;
        mHost->mReused = 0;
    }

    return NOERROR;
}

//======================================================================
// IdleCache
//======================================================================
const Int32 IdleCache::IDLE_CACHE_MAX = 8;
const Int32 IdleCache::EMPTY_CHECK_MAX = 5;
const Int32 IdleCache::TIMEOUT = 6 * 1000;
const Int32 IdleCache::CHECK_INTERVAL = 2 * 1000;

IdleCache::IdleCache()
    : mCount(0)
    , mCached(0)
    , mReused(0)
{
    mEntries = ArrayOf<Entry*>::Alloc(IDLE_CACHE_MAX);
    for (Int32 i = 0; i < IDLE_CACHE_MAX; i++) {
        (*mEntries)[i] = new Entry();
    }
}

ECode IdleCache::constructor()
{
    for (Int32 i = 0; i < IDLE_CACHE_MAX; i++) {
        (*mEntries)[i] = new Entry();
    }
    return NOERROR;
}

ECode IdleCache::CacheConnection(
    /* [in] */ IHttpHost* host,
    /* [in] */ IConnection* connection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean ret = FALSE;
    {    AutoLock syncLock(this);


        if (HttpLog::LOGV) {
            HttpLog::V(String("IdleCache size ") + StringUtils::ToString(mCount) + String(" host ")/* + host*/);
        }

        if (mCount < IDLE_CACHE_MAX) {
            Int64 time = SystemClock::GetUptimeMillis();
            for (Int32 i = 0; i < IDLE_CACHE_MAX; i++) {
                Entry* entry = (*mEntries)[i];
                if (entry->mHost == NULL) {
                    entry->mHost = host;
                    // entry->mConnection = connection;
                    entry->mTimeout = time + TIMEOUT;
                    mCount++;
                    if (HttpLog::LOGV) mCached++;
                    ret = TRUE;
                    if (mThread == NULL) {
                        mThread = new IdleReaper(this);
                        mThread->Start();
                    }
                    break;
                }
            }
        }
    }
    FUNC_RETURN(ret);
}

ECode IdleCache::GetConnection(
    /* [in] */ IHttpHost* host,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)

    {    AutoLock syncLock(this);

        Connection* ret = NULL;

        if (mCount > 0) {
            for (Int32 i = 0; i < IDLE_CACHE_MAX; i++) {
                Entry* entry = (*mEntries)[i];
                AutoPtr<IHttpHost> eHost = entry->mHost;
                // TODO:
                if (eHost != NULL /*&& eHost->Equals(host)*/) {
                    ret = (Connection*)entry->mConnection.Get();
                    entry->mHost = NULL;
                    entry->mConnection = NULL;
                    mCount--;
                    if (HttpLog::LOGV) mReused++;
                    break;
                }
            }
        }

        FUNC_RETURN(ret)
    }
    return NOERROR;
}

ECode IdleCache::Clear()
{
    {    AutoLock syncLock(this);

        for (Int32 i = 0; mCount > 0 && i < IDLE_CACHE_MAX; i++) {
            Entry* entry = (*mEntries)[i];
            if (entry->mHost != NULL) {
                entry->mHost = NULL;
                // entry->mConnection->CloseConnection();
                // entry->mConnection = NULL;
                mCount--;
            }
        }
    }
    return NOERROR;
}

ECode IdleCache::ClearIdle()
{
    {    AutoLock syncLock(this);

        if (mCount > 0) {
            Int64 time = SystemClock::GetUptimeMillis();
            for (Int32 i = 0; i < IDLE_CACHE_MAX; i++) {
                Entry* entry = (*mEntries)[i];
                if (entry->mHost != NULL && time > entry->mTimeout) {
                    entry->mHost = NULL;
                    // entry->mConnection->CloseConnection();
                    // entry->mConnection = NULL;
                    mCount--;
                }
            }
        }
    }
    return NOERROR;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
