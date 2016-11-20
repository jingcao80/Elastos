
#include "org/alljoyn/bus/NativeSessionPortListener.h"
#include "org/alljoyn/bus/CSessionOpts.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeSessionPortListener::NativeSessionPortListener(
    /* [in] */ ISessionPortListener* sessionPortListener)
{
    IWeakReferenceSource::Probe(sessionPortListener)->GetWeakReference((IWeakReference**)&mSessionPortListener);
    assert(mSessionPortListener != NULL);
}

NativeSessionPortListener::~NativeSessionPortListener()
{}

bool NativeSessionPortListener::AcceptSessionJoiner(
    /* [in] */ ajn::SessionPort sessionPort,
    /* [in] */ const char* joiner,
    /* [in] */ const ajn::SessionOpts& opts)
{
    AutoPtr<ISessionOpts> so;
    CSessionOpts::New((ISessionOpts**)&so);
    so->SetTraffic(opts.traffic);
    so->SetMultipoint(opts.isMultipoint);
    so->SetProximity(opts.proximity);
    so->SetTransports(opts.transports);

    AutoPtr<ISessionPortListener> spl;
    mSessionPortListener->Resolve(EIID_ISessionPortListener, (IInterface**)&spl);
    if (spl == NULL) {
        Logger::E("NativeSessionPortListener", "AcceptSessionJoiner(): Can't get new local reference to ISessionPortListener");
        return false;
    }

    Boolean result;
    ECode ec = spl->AcceptSessionJoiner((Int16)sessionPort, String(joiner), so, &result);
    if (FAILED(ec)) {
        Logger::E("NativeSessionPortListener", "AcceptSessionJoiner(): Exception");
        return false;
    }

    return (bool)result;
}

void NativeSessionPortListener::SessionJoined(
    /* [in] */ ajn::SessionPort sessionPort,
    /* [in] */ ajn::SessionId id,
    /* [in] */ const char* joiner)
{
    AutoPtr<ISessionPortListener> spl;
    mSessionPortListener->Resolve(EIID_ISessionPortListener, (IInterface**)&spl);
    if (spl == NULL) {
        Logger::E("NativeSessionPortListener", "SessionJoined(): Can't get new local reference to ISessionPortListener");
    }

    ECode ec = spl->SessionJoined((Int16)sessionPort, (Int32)id, String(joiner));
    if (FAILED(ec)) {
        Logger::E("NativeSessionPortListener", "SessionJoined(): Exception");
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
