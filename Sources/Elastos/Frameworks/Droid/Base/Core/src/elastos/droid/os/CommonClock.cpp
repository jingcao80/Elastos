
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/CommonClock.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Os {

const Int64 CommonClock::TIME_NOT_SYNCED = -1;
const Int64 CommonClock::INVALID_TIMELINE_ID = 0;
const Int32 CommonClock::ERROR_ESTIMATE_UNKNOWN = 0x7FFFFFFF;
const Int32 CommonClock::STATE_INVALID = -1;
const Int32 CommonClock::STATE_INITIAL = 0;
const Int32 CommonClock::STATE_CLIENT = 1;
const Int32 CommonClock::STATE_MASTER = 2;
const Int32 CommonClock::STATE_RONIN = 3;
const Int32 CommonClock::STATE_WAIT_FOR_ELECTION = 4;
const String CommonClock::SERVICE_NAME("common_time.clock");

CommonClock::CommonClock()
{
}

CommonClock::~CommonClock()
{
    ReleaseResources();
}

ECode CommonClock::constructor()
{
    assert(0);
    //mRemote = ServiceManager::GetService(SERVICE_NAME);
    if (NULL == mRemote)
        return E_REMOTE_EXCEPTION;

    mInterfaceDesc = Object::ToString(mRemote);
    // mRemote->LinkToDeath(mDeathHandler, 0);
    RegisterTimelineChangeListener();
    return NOERROR;
}

AutoPtr<CommonClock> CommonClock::Create()
{
    AutoPtr<CommonClock> retVal = new CommonClock();

    if (FAILED(retVal->constructor())) {
        return NULL;
    }

    return retVal;
}

ECode CommonClock::ReleaseResources()
{
    UnregisterTimelineChangeListener();
    if (NULL != mRemote) {
        // mRemote.unlinkToDeath(mDeathHandler, 0);
        mRemote = NULL;
    }

    return NOERROR;
}

ECode CommonClock::GetTime(
    /* [out] */ Int64* time)
{
    FAIL_RETURN(ThrowOnDeadServer())
    assert(0);
    return NOERROR;
}

ECode CommonClock::GetEstimatedError(
    /* [out] */ Int32* error)
{
    FAIL_RETURN(ThrowOnDeadServer())
    assert(0);
    return NOERROR;
}

ECode CommonClock::GetTimelineId(
    /* [out] */ Int64* time)
{
    FAIL_RETURN(ThrowOnDeadServer())
    assert(0);
    return NOERROR;
}

ECode CommonClock::GetState(
    /* [out] */ Int32* state)
{
    FAIL_RETURN(ThrowOnDeadServer())
    assert(0);
    return NOERROR;
}

ECode CommonClock::GetMasterAddr(
    /* [out] */ IInetSocketAddress** isa)
{
    FAIL_RETURN(ThrowOnDeadServer())
    assert(0);
    return NOERROR;
}

ECode CommonClock::SetTimelineChangedListener(
    /* [in] */ ICommonClockOnTimelineChangedListener* listener)
{
    {    AutoLock syncLock(mListenerLock);
        mTimelineChangedListener = listener;
    }
    return NOERROR;
}

ECode CommonClock::SetServerDiedListener(
    /* [in] */ ICommonClockOnServerDiedListener* listener)
{
    {    AutoLock syncLock(mListenerLock);
        mServerDiedListener = listener;
    }
    return NOERROR;
}

ECode CommonClock::ThrowOnDeadServer()
{
    if (NULL == mRemote)
        return E_REMOTE_EXCEPTION;

    return NOERROR;
}

ECode CommonClock::RegisterTimelineChangeListener()
{
    assert(0);
    return NOERROR;
}

ECode CommonClock::UnregisterTimelineChangeListener()
{
    assert(0);
    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
